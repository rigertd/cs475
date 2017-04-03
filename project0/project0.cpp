/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #0
* File:         project0.cpp
* Created:      2017-04-02
* Modified:     2017-04-02
* Description:  A simple OpenMP experiment written in C11.
*
*               This program times the execution of multiplying
*               values from two large floating point arrays
*               and storing the result in a third array.
*
*               The execution time is displayed in the console.
********************************************************************/

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <omp.h>

// Define NUMT as 4 if not defined in compiler args
#ifndef NUMT
#define NUMT 4
#endif

#define ARRSIZE     10000000
#define RUNCOUNT    100

int main(int argc, char *argv[]) {
#ifndef _OPENMP
    fprintf(stderr, "OpenMP support is required to run this program.\n");
    return 1;
#endif

    float *A = new float[ARRSIZE];
    float *B = new float[ARRSIZE];
    float *C = new float[ARRSIZE];
    assert(A != nullptr);
    assert(B != nullptr);
    assert(C != nullptr);

    omp_set_num_threads(NUMT);
    printf("Using %d threads\n", NUMT);

    double maxMegaMults = 0.;
    double sumMegaMults = 0.;

    for (int i = 0; i < RUNCOUNT; ++i) {
        double startTime = omp_get_wtime();

        // Split loop into NUMT segments and run each in its own thread
        #pragma omp parallel for
        for (int j = 0; j < ARRSIZE; ++j) {
            C[j] = A[j] * B[j];
        }

        double endTime = omp_get_wtime();

        // Compute millions of multiply operations per second
        double megaMults = (double)ARRSIZE / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;

        // Update max performance if current run is higher
        if (megaMults > maxMegaMults) {
            maxMegaMults = megaMults;
        }
    }

    // Compute the average performance for all runs
    double avgMegaMults = sumMegaMults / (double)RUNCOUNT;
    printf("   Peak = %8.2lf MegaMults/Sec\n", maxMegaMults);
    printf("Average = %8.2lf MegaMults/Sec\n", avgMegaMults);

    delete[] A;
    delete[] B;
    delete[] C;

    return 0;
}