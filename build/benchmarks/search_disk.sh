#!/bin/bash

#! 检查/app路径是否存在
if [ ! -d "/app" ]; then
  echo "May not Docker env."
  exit 1
fi

base_path="/app/DiskANN/build/data"

shared_path=${base_path}/bigann_shards
search_path=${base_path}/bigann_search

base_file="/app/data/bigann-1B/base.1B.u8bin"
query_file="/app/data/bigann-1B/query.public.10K.u8bin"


mkdir -p ${search_path}
# rm -f ${search_path}/

# num_points=100000
npts_arr=("100000" "200000" "300000" "400000" "500000" "1000000" "10000000" "100000000")
# npts_arr=("1000000")

cache_size_arr=("10000" "20000" "30000" "40000" "50000" "100000" "1000000" "10000000")
# cache_size_arr=("100000")
B_arr=("0.0066" "0.0132" "0.0198" "0.0264" "0.033" "0.066" "0.66" "6.6")
# npts_arr=("100000" "200000" "300000")
# N_threads_arr=("4" "8" "16" "32" "64" "128")

# 使用空格将数组元素连接成一个字符串
params="${npts_arr[0]}"
# for num_points in "${npts_arr[@]}"; do
for index in "${!npts_arr[@]}"; do
    num_points=${npts_arr[$index]}
    cache_size=${cache_size_arr[$index]}
    B=${B_arr[$index]}

    echo "num_points: ${num_points} cache_size: ${cache_size} B: ${B}"

    shard_file=${shared_path}/bigann_shard_${num_points}.data
    # ./apps/utils/crop_data uint8 ${base_file} ${search_path}/bigann_shard_${num_points} ${num_points}

    if [[ ! -f "$shard_file" ]]; then
        echo "$shard_file not exists."
        exit -1
    fi

    if [[ ! -d "${search_path}/${num_points}" ]]; then
        mkdir -p ${search_path}/${num_points}

        ./apps/utils/compute_groundtruth  --data_type uint8 --dist_fn l2 --base_file ${shard_file} --query_file ${query_file} --gt_file ${search_path}/bigann_search_${num_points}_gt100 --K 100

        ./apps/build_disk_index --data_type uint8 --dist_fn l2 --data_path ${shard_file} --index_path_prefix ${search_path}/disk_index_bigann_${num_points}_R64_L75_A1.2 -R 64 -L 75 -T 144 -B ${B} -M 1

        mv ${search_path}/disk_index_bigann_${num_points}_R64_L75_A1.2* ${search_path}/${num_points}/
        mv ${search_path}/bigann_search_${num_points}_gt100 ${search_path}/${num_points}/
    fi

    if [[ ! -d "${search_path}/${num_points}/search_res" ]]; then
        mkdir -p ${search_path}/${num_points}/search_res
    fi

    ./apps/search_disk_index  --data_type uint8 --dist_fn l2 --index_path_prefix ${search_path}/${num_points}/disk_index_bigann_${num_points}_R64_L75_A1.2 --query_file ${query_file}  --gt_file ${search_path}/${num_points}/bigann_search_${num_points}_gt100 -K 10 -T 64 -W 4 -L 10 20 30 40 50 100 --result_path ${search_path}/${num_points}/search_res/res --num_nodes_to_cache ${cache_size} 1> ${search_path}/${num_points}/search_disk_index_bigann_${num_points}_R64_L75_A1.2.log 2>&1
done




