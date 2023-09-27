// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <functional>
#ifdef _WINDOWS
#include <numeric>
#endif
#include <string>
#include <vector>

#include "distance.h"
#include "parameters.h"

namespace diskann
{
struct QueryStats
{
    float total_us = 0; // total time to process query in micros
    float io_us = 0;    // total time spent in IO
    float cpu_us = 0;   // total time spent in CPU

    // float ivf_dist_us = 0; // time spent in IVF distance computation
    float pqdist_us = 0;  // time spent in PQ distance computation
    float compute_dist_us = 0; // time spent in PQ distance computation
    float sort_us = 0;     // time spent in sorting
    float single_pqdist_us = 0; // time spent in single PQ distance computation

    // some others
    float cache_hit_rate = 0; // cache hit rate

    std::string iter_ids; // ids of iterations
    uint32_t iter_ids_len = 0;         // # of results returned

    unsigned n_4k = 0;         // # of 4kB reads
    unsigned n_8k = 0;         // # of 8kB reads
    unsigned n_12k = 0;        // # of 12kB reads
    unsigned n_ios = 0;        // total # of IOs issued
    unsigned read_size = 0;    // total # of bytes read
    unsigned n_cmps_saved = 0; // # cmps saved
    unsigned n_cmps = 0;       // # cmps
    unsigned n_cache_hits = 0; // # cache_hits
    unsigned n_cache_misses = 0; // # cache_misses
    unsigned n_hops = 0;       // # search hops

    unsigned n_nnbrs = 0; // # avg neighbors
    unsigned n_dist = 0; // # avg distance
    unsigned n_chunks = 0;
};

template <typename T> 
inline void get_stats_arr(QueryStats *stats, uint64_t len, std::vector<T>& ret, 
        const std::function<T(const QueryStats &)> &member_fn) {
    ret.clear();
    ret.resize(len);
    for(int i = 0; i < len; ++i) {
        ret[i] = member_fn(stats[i]);
    }
}

template <typename T>
inline T get_percentile_stats(QueryStats *stats, uint64_t len, float percentile,
                              const std::function<T(const QueryStats &)> &member_fn)
{
    std::vector<T> vals(len);
    for (uint64_t i = 0; i < len; i++)
    {
        vals[i] = member_fn(stats[i]);
    }

    std::sort(vals.begin(), vals.end(), [](const T &left, const T &right) { return left < right; });

    auto retval = vals[(uint64_t)(percentile * len)];
    vals.clear();
    return retval;
}

template <typename T>
inline double get_mean_stats(QueryStats *stats, uint64_t len, const std::function<T(const QueryStats &)> &member_fn)
{
    double avg = 0;
    for (uint64_t i = 0; i < len; i++)
    {
        avg += (double)member_fn(stats[i]);
    }
    return avg / len;
}
} // namespace diskann
