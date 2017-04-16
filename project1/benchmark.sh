#!/bin/bash
# Set alias for GCC 5.4
GPP5="/usr/local/common/gcc-5.4.0/bin/g++"
# Remove any existing results, hiding any error message
rm -f project1.csv 2>/dev/null
# Number of nodes
for s in 2 4 8 16 32 64 128 256 512 1024 2048 3072 4096 6144 8192 10240
do
	echo NUMS=$s
	$GPP5 -DNUMNODES=$s project1.cpp -o prog -O0 -lm -fopenmp
    # Number of threads
    for t in 1 2 4 6 8
    do
        echo NUMT=$t
    	./prog $t >> project1.csv
    done
done
