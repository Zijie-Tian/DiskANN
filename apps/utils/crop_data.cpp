// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

#include <mutex>
#include <numeric>
#include <random>
#include <omp.h>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <atomic>

#include "utils.h"
#include "timer.h"
#include "math_utils.h"
#ifndef _WINDOWS
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <pthread.h>
#include <sched.h>

bool fileExists(const std::string &filename) {
  if (FILE *file = fopen(filename.c_str(), "r")) {
    fclose(file);
    return true;
  }
  return false;
}

template<typename T, typename TagT = uint32_t>
void dump_to_disk(const T *all_pts, const uint64_t ndims,
                  const std::string           &filename,
                  const std::vector<uint32_t> &tags) {
  T    *new_data = new T[ndims * tags.size()];
  TagT *new_tags = new TagT[tags.size()];

  std::string tag_filename = filename + ".tags";
  std::string data_filename = filename + ".data";
  diskann::cout << "# points : " << tags.size() << "\n";
  diskann::cout << "Tag file : " << tag_filename << "\n";
  diskann::cout << "Data file : " << data_filename << "\n";

  // std::ofstream tag_writer(tag_filename);
  //! 这个东西是按照 tags 进行确定某一个点是否要保存的。
  for (uint64_t i = 0; i < tags.size(); i++) {
    //    tag_writer << tags[i] << std::endl;
    *(new_tags + i) = tags[i];
    memcpy(new_data + (i * ndims), all_pts + (tags[i] * ndims),
           ndims * sizeof(T));
  }
  //  tag_writer.close();
  diskann::save_bin<TagT>(tag_filename, new_tags, tags.size(), 1);
  diskann::save_bin<T>(data_filename, new_data, tags.size(), ndims);
  delete new_data;
  delete new_tags;
}

template<typename T, typename TagT = uint32_t>
void shard_data(const std::string &data_file, const std::string &dst_prefix,
                size_t ndst_pts, size_t &aligned_dim) {
  assert(fileExists(data_file));

  T       *all_points = nullptr;
  uint64_t npts, ndims;
  diskann::load_bin<T>(data_file, all_points, npts, ndims);
  aligned_dim = ROUND_UP(ndims, 8);

  std::vector<uint32_t> tags(npts, 0);
  std::iota(tags.begin(), tags.end(), 0);
  //! 这个地方不能进行shuffle，因为后面的代码写错了。先不shuffle了。
  //   std::random_shuffle(tags.begin(), tags.end());
  std::vector<uint32_t> base_tags(tags.begin(), tags.begin() + ndst_pts);

  dump_to_disk<T, TagT>(all_points, aligned_dim, dst_prefix, base_tags);
}

int main(int argc, char const *argv[]) {
  if (argc < 5) {
    std::cout << "Correct usage: " << argv[0]
              << " <type[int8/uint8/float]> <datafile> <dst_prefix> <npts_dst>"
              << "\n WARNING: Other parameters set inside CPP source."
              << std::endl;
    exit(-1);
  } else {
    std::cout << "This driver file only works with uint32 type tags"
              << std::endl;
  }

  size_t aligned_dim;
  if (strcmp(argv[1], "int8") == 0) {
    shard_data<int8_t, uint32_t>(argv[2], argv[3], atoi(argv[4]), aligned_dim);
  } else if (strcmp(argv[1], "uint8") == 0) {
    shard_data<uint8_t, uint32_t>(argv[2], argv[3], atoi(argv[4]), aligned_dim);
  } else if (strcmp(argv[1], "float") == 0) {
    shard_data<float, uint32_t>(argv[2], argv[3], atoi(argv[4]), aligned_dim);
  } else {
    std::cout << "Unsupported type" << std::endl;
    exit(-1);
  }
  return 0;
}
