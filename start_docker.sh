#! /bin/bash

docker run -it --rm --cap-add SYS_ADMIN --privileged -v $(pwd):/app/DiskANN -v /data/BIGANN:/app/data/bigann-1B -v /data/sift:/app/data/sift --name raw_diskann --hostname DiskANN tzj/diskann bash
