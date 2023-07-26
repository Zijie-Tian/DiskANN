#!/bin/bash
data_path="data/sift"
index_path="data/sift_streaming/"
result_path="data/sift_search_str"

mkdir -p ${result_path}

# Experiment setup parameters
type='float'
data_file="${data_path}/sift_base.fbin"
learn_file="${data_path}/sift_learn.fbin"
query_file="${data_path}/sift_query.fbin"

result="${result_path}/res"

groundtruth="${result_path}/gt100_base"

# Search parameters
gt_K=100
search_K=10
R=32
L=50
alpha=1.2

memindex_path_prefix="${index_path}/idx_str.tzj-test-start-90000-act20000-cons10000-max100000"
ssdindex_path_prefix="${index_path}/idx_str.tzj-test-start-90000-act20000-cons10000-max100000"
# ssdindex_path_prefix="${index_path}/idx_str.after-streaming-act20000-cons10000-max100000"

csv_file_mem="${result}_mem.csv"
csv_file_ssd="${result}_ssd.csv"

echo "========================================================================================================="
echo "| Memory index"
echo "========================================================================================================="

apps/search_memory_index  --data_type float --dist_fn l2 --index_path_prefix ${memindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 60 70 80 90 100 --result_path ${result} --csv_file ${csv_file_mem}


# echo "========================================================================================================="
# echo "| SSD index"
# echo "========================================================================================================="

# apps/search_disk_index  --data_type float --dist_fn l2 --index_path_prefix ${ssdindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result} --num_nodes_to_cache 10000 --csv_file ${csv_file_ssd}


