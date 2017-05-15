#!/bin/bash
# Remove any existing results, hiding any error message
rm -f project5.csv 2>/dev/null
# Output header row to CSV
echo "SSE","Reduction","Array Size","Avg MegaMults/Sec","Peak MegaMults/Sec" >> project5.csv
# Run for different array sizes, from 1024 to 33554432
for arr in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 6291456 8388608 12582912 16777216 33554432
do
    echo ARR_SIZE=$arr
    g++ -std=c++11 -DARR_SIZE=$arr project5.cpp simd.p5.cpp -o prog -lm -fopenmp
    ./prog >> project5.csv
done

