#!/bin/bash


#! 检查/app路径是否存在
if [ ! -d "/app" ]; then
  echo "May not Docker env."
  exit 1
fi

base_path="/app/DiskANN/build/data"
shards_path=${base_path}/bigann_shards

mkdir -p ${shards_path}

base_file="/app/data/bigann-1B/base.1B.u8bin"
query_file="/app/data/bigann-1B/query.public.10K.u8bin"

PREFIX="bigann_shard"

# npts_arr=("100000000")
npts_arr=("100000" "200000" "300000" "400000" "500000" "1000000" "10000000" "100000000")

# 使用空格将数组元素连接成一个字符串
params="${npts_arr[0]}"
for i in "${npts_arr[@]:1}"; do
    params="$params $i"
done
echo "$params"

if [ "$EXEC_STRATEGY" != "no_crop" ]; then 
  ./apps/utils/multi_crop uint8 ${base_file} ${shards_path}/${PREFIX} ${params}
  # ./tests/utils/multi_crop uint8 ${data_file} ${input_path}/${INDEX_FILE}_base ${params}
fi