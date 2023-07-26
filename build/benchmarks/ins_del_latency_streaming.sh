#!/bin/bash
data_path="data/sift"
result_path="data/sift_streaming"

mkdir -p ${result_path}

# Experiment setup parameters
type='float'
data="${data_path}/sift_base.fbin"
learn="${data_path}/sift_learn.fbin"
query="$data_path/sift_query.fbin"

index_prefix="${result_path}/idx_str"
result="${result_path}/res"
ins_thr=16      # Insert threads.
cons_thr=16     # Delete Consolidate threads.
inserts=100000  # Number of inserts.
active=20000    # Active window size.
cons_int=10000  # Consolidate interval.

index=${index_prefix}.after-streaming-act${active}-cons${cons_int}-max${inserts}
gt=${result_path}/gt100_learn-act${active}-cons${cons_int}-max${inserts}

# Search parameters
gt_K=100        # Groundtruth K.
search_K=10     # Search top-K's K.
R=32            # Max degree of node.
L=50            # List Length of building Graph.

csv_file_mem="${index}_mem.csv"

# Start streaming
# apps/test_streaming_scenario  --data_type ${type} --dist_fn l2 --data_path ${data}  --index_path_prefix ${index_prefix} -R 32 -L 600 --alpha 1.2 --insert_threads ${ins_thr} --consolidate_threads ${cons_thr}  --max_points_to_insert ${inserts}  --active_window ${active} --consolidate_interval ${cons_int} --start_point_norm 508;

./apps/test_streaming_scenario  --data_type ${type} --dist_fn l2 --data_path /home/tzj/Code/DiskANN/build/data/sift/sift_base.fbin --index_path_prefix /home/tzj/Code/DiskANN/build/data/sift_streaming/idx_str -R 64 -L 600 --alpha 1.2 --insert_threads 16 --consolidate_threads 16 --max_points_to_insert 100000  --active_window 20000 --consolidate_interval 10000 --start_point_norm 508;


# Learn index
# tests/build_memory_index  --data_type float --dist_fn l2 --data_path ${data_file} --index_path_prefix ${memindex_path_prefix} -R ${R} -L ${L} --alpha ${alpha}


# # Compute groundtruth
# apps/utils/compute_groundtruth --data_type ${type} --dist_fn l2 --base_file ${index}.data  --query_file ${query}  --K 100 --gt_file ${gt} --tags_file  ${index}.tags
# # Search
# apps/search_memory_index  --data_type ${type} --dist_fn l2 --index_path_prefix ${index} --result_path ${result} --query_file ${query}  --gt_file ${gt}  -K 10 -L 20 40 60 80 100 -T 64 --dynamic true --tags 1 --csv_file ${csv_file_mem}