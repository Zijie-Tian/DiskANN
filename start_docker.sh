#! /bin/bash

docker run -it --rm --cap-add SYS_ADMIN --privileged -v $(pwd):/app/DiskANN -v /home/tzj/ANN_data/bigann-1B:/app/data/bigann-1B --name raw_diskann --hostname DiskANN tzj/diskann bash
