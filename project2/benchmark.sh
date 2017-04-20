#!/bin/bash
# Remove any existing results, hiding any error message
rm -f project2.csv 2>/dev/null
# static or dynamic scheduling
for s in static dynamic
do
    echo SCHEDULING=$s
    # chunksize
    for c in 1 4096
    do
        echo CHUNKSIZE=$c
        g++ -DSCHEDULING=$s -DCHUNKSIZE=$c project2.cpp -o prog -O1 -lm -fopenmp
        # Number of threads
        for t in 1 2 4 6 8 10 12 16
        do
            echo NUMT=$t
            ./prog $t >> project2.csv
        done
    done
done
