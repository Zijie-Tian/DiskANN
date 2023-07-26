#!/bin/bash
data_path="data/sift"
result_path="data/sift_search"

mkdir -p ${result_path}

# Experiment setup parameters
type='float'
data_file="${data_path}/sift_base.fbin"
learn_file="${data_path}/sift_learn.fbin"
query_file="${data_path}/sift_query.fbin"

index_prefix="${result_path}/idx_learn_str"
result="${result_path}/res"

groundtruth="${result_path}/gt100_base"

# Search parameters
gt_K=100
search_K=10
R=32
L=50
alpha=1.2

memindex_path_prefix=data/sift_memdisk/index_mem_sift_learn_R32_L50_A1.2
ssdindex_path_prefix=data/sift_memdisk/index_ssd_sift_learn_R32_L50_A1.2

csv_file_mem="${result}_mem.csv"
csv_file_ssd="${result}_ssd.csv"


# echo "========================================================================================================="
# echo "| Memory index"
# echo "========================================================================================================="

# tests/search_memory_index  --data_type float --dist_fn l2 --index_path_prefix ${memindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result} --csv_file ${csv_file_mem}


echo "========================================================================================================="
echo "| SSD index"
echo "========================================================================================================="

tests/search_disk_index  --data_type float --dist_fn l2 --index_path_prefix ${ssdindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result} --num_nodes_to_cache 10000 --csv_file ${csv_file_ssd}


