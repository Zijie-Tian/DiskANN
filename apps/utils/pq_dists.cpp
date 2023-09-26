#include <mutex>
#include <numeric>
#include <random>
#include <omp.h>
#include <cstring>
#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <atomic>
#include <xmmintrin.h>

// Need to replace calls to these functions with calls to vector& based
// functions above
void aggregate_coords(const uint32_t *ids, const size_t n_ids, const uint8_t *all_coords, const size_t ndims,
                      uint8_t *out)
{
    for (size_t i = 0; i < n_ids; i++)
    {
        memcpy(out + i * ndims, all_coords + ids[i] * ndims, ndims * sizeof(uint8_t));
    }
}

void pq_dist_lookup(const uint8_t *pq_ids, const size_t n_pts, const size_t pq_nchunks, const float *pq_dists,
                    float *dists_out)
{
    _mm_prefetch((char *)dists_out, _MM_HINT_T0);
    _mm_prefetch((char *)pq_ids, _MM_HINT_T0);
    _mm_prefetch((char *)(pq_ids + 64), _MM_HINT_T0);
    _mm_prefetch((char *)(pq_ids + 128), _MM_HINT_T0);
    memset(dists_out, 0, n_pts * sizeof(float));
    for (size_t chunk = 0; chunk < pq_nchunks; chunk++)
    {
        const float *chunk_dists = pq_dists + 256 * chunk;
        if (chunk < pq_nchunks - 1)
        {
            _mm_prefetch((char *)(chunk_dists + 256), _MM_HINT_T0);
        }
        for (size_t idx = 0; idx < n_pts; idx++)
        {
            uint8_t pq_centerid = pq_ids[pq_nchunks * idx + chunk];
            dists_out[idx] += chunk_dists[pq_centerid];
        }
    }
}

int main(int argc, char const *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: ./main <n_chunks> <n_ids>" << std::endl;
        return 0;
    }

    int n_ids, n_chunks;
    sscanf(argv[1], "%d", &n_chunks);
    sscanf(argv[2], "%d", &n_ids);

    std::random_device rd;  // 用于生成种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister 19937生成器
    std::uniform_int_distribution<> distrib(1, 256);  // 定义一个范围为1到6的均匀分布

    float* pq_dists = (float*)malloc(256 * n_chunks * sizeof(float));
    memset(pq_dists, 0, 256 * n_chunks * sizeof(float));

    // generate random pq distances
    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < n_chunks; j++) {
            pq_dists[i + j * 256] = distrib(gen);
        }
    }

    // generate query random ids    
    uint32_t* ids = (uint32_t*)malloc(n_ids * n_chunks * sizeof(uint32_t));
    uint8_t* pq_coord_scratch = (uint8_t*)malloc(n_ids * n_chunks * sizeof(uint8_t));

    for (int i = 0; i < n_ids; i++) {
        for (int j = 0; j < n_chunks; j++) {
            ids[i] = distrib(gen);
        }
    }

    // lambda to batch compute query<-> node distances in PQ space
    auto compute_dists = [n_chunks, pq_coord_scratch, pq_dists](const uint32_t *ids, const uint64_t n_ids,
                                                            float *dists_out) {
        // aggregate_coords(ids, n_ids, data, n_chunks, pq_coord_scratch);
        pq_dist_lookup(pq_coord_scratch, n_ids, n_chunks, pq_dists, dists_out);
    };

    // lambda to compute the topk of a set of distances
    uint8_t *query = (uint8_t*)malloc(n_chunks * sizeof(uint8_t));
    float *dists = (float*)malloc(n_ids * sizeof(float));
    
    auto start_time = std::chrono::high_resolution_clock::now();
    compute_dists(ids, n_ids, dists);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "duration: " << duration.count() << " us" << std::endl;

    return 0;
}

