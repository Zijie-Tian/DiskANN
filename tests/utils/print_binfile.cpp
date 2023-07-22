#include "common_includes.h"
#include <boost/program_options.hpp>

#include <iostream>
#include "utils.h"

namespace po = boost::program_options;

//! 写一个能打印那个文件的程序。

int main() {
    // load query bin
    std::string query_file;
    po::options_description desc{"Arguments"};
    desc.add_options()("help,h", "Print information on arguments");
    desc.add_options()("query_file", po::value<std::string>(&query_file)->required(), 
                       "query file in bin format");

    float*        query = nullptr;
    unsigned* gt_ids = nullptr;
    float*    gt_dists = nullptr;
    size_t    query_num, query_dim, query_aligned_dim, gt_num, gt_dim;
    diskann::load_aligned_bin<float>(query_file.c_str(), query, query_num, query_dim,
                                query_aligned_dim);

    return 0;
}

