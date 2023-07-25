#!/bin/bash
data_path="data/sift"
result_path="data/sift_memdisk"

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

memindex_path_prefix=${result_path}/index_mem_sift_learn_R32_L50_A1.2
ssdindex_path_prefix=${result_path}/index_ssd_sift_learn_R32_L50_A1.2

csv_file_mem="${result}_mem.csv"
csv_file_ssd="${result}_ssd.csv"

rm ${groundtruth}

# tests/utils/compute_groundtruth --data_type float --dist_fn l2 --base_file ${data_file} --query_file ${query_file} --gt_file ${groundtruth} --K ${gt_K}

echo "========================================================================================================="
echo "| Memory index"
echo "========================================================================================================="

# tests/build_memory_index  --data_type float --dist_fn l2 --data_path ${data_file} --index_path_prefix ${memindex_path_prefix} -R ${R} -L ${L} --alpha ${alpha}

tests/search_memory_index  --data_type float --dist_fn l2 --index_path_prefix ${memindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result} --csv_file ${csv_file_mem}


echo "========================================================================================================="
echo "| SSD index"
echo "========================================================================================================="

# Using 0.003GB search memory budget for 100K vectors implies 32 byte PQ compression
# tests/build_disk_index --data_type float --dist_fn l2 --data_path ${data_file} --index_path_prefix ${ssdindex_path_prefix} -R ${R} -L ${L} -B 3 -M 1 --PQ_disk_bytes 0 --build_PQ_bytes 0

tests/search_disk_index  --data_type float --dist_fn l2 --index_path_prefix ${ssdindex_path_prefix} --query_file ${query_file}  --gt_file ${groundtruth} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result} --num_nodes_to_cache 10000 --csv_file ${csv_file_ssd}


