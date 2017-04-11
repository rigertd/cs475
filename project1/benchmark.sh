#!/bin/bash
# Remove any existing results, hiding any error message
rm -f project1.csv 2>/dev/null
# Number of threads
for t in 1 2 4 6 8
do
	echo NUMT=$t
	# Number of nodes
	for s in 2 4 8 16 32 64 128 256 512 1024 2048 3072 4096 6144 8192 10240
	do
		echo NUMS=$s
		g++5 -DNUMNODES=$s project1.cpp -o prog -O3 -lm -fopenmp
		./prog $t >> project1.csv
	done
done