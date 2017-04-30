#!/bin/bash
GXX=/usr/local/common/gcc-5.2.0/bin/g++
# Remove any existing results, hiding any error message
rm -f project3_1.csv project3_2.csv 2>/dev/null
# Fix #1: integer padding
echo Padding fix
for p in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
do
    echo NUMPAD=$p
    $GXX -std=c++11 -DNUMPAD=$p project3.cpp -o prog -O0 -lm -fopenmp
    # threads
    for t in 1 2 4 6 8 
    do
        echo Threads=$t
        ./prog $t >> project3_1.csv
    done
done
# Fix #2: local variable
echo Local variable fix
$GXX -std=c++11 -DNUMPAD=0 -DLOCAL project3.cpp -o prog -O0 -lm -fopenmp
# threads
for t in 1 2 4 6 8 
do
    echo Threads=$t
    ./prog $t >> project3_2.csv
done

