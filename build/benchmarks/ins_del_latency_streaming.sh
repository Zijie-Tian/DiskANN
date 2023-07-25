#!/bin/bash
data_path="data/sift"
result_path="data/sift_streaming"

mkdir -p ${result_path}

# Experiment setup parameters
data="${data_path}/sift_base.fbin"
learn="${data_path}/sift_learn.fbin"
query="$data_path/sift_query.fbin"

index_prefix="${result_path}/idx_learn_str"
result="${result_path}/res"
ins_thr=16      # Insert threads.
cons_thr=16     # Delete Consolidate threads.
inserts=100000  # Number of inserts.
active=20000    # Active window size.
cons_int=10000  # Consolidate interval.

# Search parameters
gt_K=100        # Groundtruth K.
search_K=10     # Search top-K's K.
R=32            # Max degree of node.
L=50            # List Length of building Graph.


tests/test_streaming_scenario  --data_type float --dist_fn l2 --data_path ${data}  --index_path_prefix ${index_prefix} -R 32 -L 600 --alpha 1.2 --insert_threads ${ins_thr} --consolidate_threads ${cons_thr}  --max_points_to_insert ${inserts}  --active_window ${active} --consolidate_interval ${cons_int} --start_point_norm 508;
