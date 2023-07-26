#!/bin/bash  
BLUE='\033[0;34m'
WHITE= '\033[0;37m' 
RED= '\033[0;31m'

data_path="data/sift"
result_path="data/sift_streaming"

mkdir -p ${result_path}

# Experiment setup parameters
type='float'
data="${data_path}/sift_base.fbin"
learn="${data_path}/sift_learn.fbin"
query="$data_path/sift_query.fbin"
index_prefix="${result_path}/idx_learn_str"
result="${result_path}/res"
ins_thr=16
cons_thr=16
inserts=100000
active=20000
cons_int=10000
index=${index_prefix}.after-streaming-act${active}-cons${cons_int}-max${inserts}
gt="${result_path}/gt100_learn-act${active}-cons${cons_int}-max${inserts}"

# Search parameters
gt_K=100
search_K=10

tests/utils/compute_groundtruth  --data_type float --dist_fn l2 --base_file ${data} --query_file  ${query} --gt_file ${gt} --K ${gt_K}

echo "========================================================================================================="

tests/build_memory_index  --data_type float --dist_fn l2 --data_path ${data} --index_path_prefix ${result_path}/index_sift_learn_R32_L50_A1.2 -R 32 -L 50 --alpha 1.2

echo "========================================================================================================="

# Static search
tests/search_memory_index  --data_type float --dist_fn l2 --index_path_prefix ${result_path}/index_sift_learn_R32_L50_A1.2 --query_file ${query} --gt_file ${gt} -K ${search_K} -L 10 20 30 40 50 100 --result_path ${result}

echo "========================================================================================================="

tests/test_streaming_scenario  --data_type ${type} --dist_fn l2 --data_path ${data}  --index_path_prefix ${index_prefix} -R 64 -L 600 --alpha 1.2 --insert_threads ${ins_thr} --consolidate_threads ${cons_thr}  --max_points_to_insert ${inserts}  --active_window ${active} --consolidate_interval ${cons_int} --start_point_norm 508;

echo "========================================================================================================="

tests/utils/compute_groundtruth --data_type ${type} --dist_fn l2 --base_file ${index}.data  --query_file ${query}  --K ${gt_K} --gt_file ${gt} --tags_file  ${index}.tags

echo "========================================================================================================="

# Search after streaming
tests/search_memory_index  --data_type ${type} --dist_fn l2 --index_path_prefix ${index} --result_path ${result} --query_file ${query}  --gt_file ${gt}  -K ${search_K} -L 20 40 60 80 100 -T 64 --dynamic true --tags 1