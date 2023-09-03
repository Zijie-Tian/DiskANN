#!/bin/bash


# ./apps/utils/fvecs_to_bin float data/sift/sift_base.fvecs data/sift/sift_base.fbin
# ./apps/utils/fvecs_to_bin float data/sift/sift_query.fvecs data/sift/sift_query.fbin


# ./apps/utils/compute_groundtruth  --data_type float --dist_fn l2 --base_file data/sift/sift_base.fbin --query_file  data/sift/sift_query.fbin --gt_file data/sift/sift_query_learn_gt100 --K 100
# # Using 0.003GB search memory budget for 100K vectors implies 32 byte PQ compression
# #! Using 0.03GB search memory budget for 1M vectors implies 32 byte PQ compression


./apps/build_disk_index --data_type float --dist_fn l2 --data_path data/sift/sift_base.fbin --index_path_prefix data/sift/disk_index_sift_base_R64_L75_A1.2 -R 64 -L 75 -T 144 -B 0.03 -M 1


./apps/search_disk_index  --data_type float --dist_fn l2 --index_path_prefix data/sift/disk_index_sift_base_R64_L75_A1.2 --query_file data/sift/sift_query.fbin  --gt_file data/sift/sift_query_learn_gt100 -K 10 -T 32 -W 4 -L 10 20 30 40 50 100 --result_path data/sift/res --num_nodes_to_cache 10000

