/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #5
* File:         project5.cpp
* Created:      2017-05-14
* Modified:     2017-05-14
* Description:  Vectorized Array Multiplication and Reduction using SSE
*
*               This program demonstrates the effects of using SSE on
*               matrix multiplication operations.
*               To specify the array size, use the following compiler option.
*                   -DARR_SIZE    Size of array to multiply
*               The output is provided in the following format:
*
*                   sse use, reduction use, array size, avg megamults/sec, peak megamults/sec
*
********************************************************************/
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include "simd.p5.h"

#include <omp.h>

#define RUNCOUNT 10

#ifndef ARR_SIZE
#define ARR_SIZE 1024
#endif

// Declare global arrays
float dA[ARR_SIZE];
float dB[ARR_SIZE];
float dC[ARR_SIZE];

// Random float function
// We don't need rand_r because only 1 thread calls this
float getRand(float low, float high)
{
    return low 
           + static_cast<float>(rand()) 
           * (high - low) 
           / static_cast<float>(RAND_MAX);
}

int main(int argc, char *argv[])
{
    // Test for OpenMP support
#ifndef _OPENMP
    std::cerr << "OpenMP support is required to run this program." << std::endl;
    return 1;
#endif

    // Seed random number generator
    srand((unsigned)std::time(NULL));

    // Fill global source arrays with random values between 0 and 1
    for (int i = 0; i < ARR_SIZE; ++i)
    {
        dA[i] = getRand(0., 1.);
        dB[i] = getRand(0., 1.);
    }

    double maxMegaMults = 0.;
    double sumMegaMults = 0.;

    // ======= Multiplication Tests =======
    // SIMD SSE version
    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

        SimdMul(dA, dB, dC, ARR_SIZE);

        double endTime = ::omp_get_wtime();
        double megaMults = static_cast<double>(ARR_SIZE) / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;
        if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }

    double avgMegaMults = sumMegaMults / static_cast<double>(RUNCOUNT);

    std::cout << "\"true\","
        << "\"false\","
        << ARR_SIZE << ","
        << avgMegaMults << ","
        << maxMegaMults << std::endl;


    maxMegaMults = 0.;
    sumMegaMults = 0.;

    // Non-SIMD version
    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

        for (int j = 0; j < ARR_SIZE; ++j)
        {
            dC[j] = dA[j] * dB[j];
        }

        double endTime = ::omp_get_wtime();
        double megaMults = static_cast<double>(ARR_SIZE) / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;
        if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }

    avgMegaMults = sumMegaMults / static_cast<double>(RUNCOUNT);

    std::cout << "\"false\","
        << "\"false\","
        << ARR_SIZE << ","
        << avgMegaMults << ","
        << maxMegaMults << std::endl;

    // ======= Multiplication and Reduction Tests =======
    // SIMD SSE version
    float runningSum = 0.;

    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

        runningSum = SimdMulSum(dA, dB, ARR_SIZE);

        double endTime = ::omp_get_wtime();
        double megaMults = static_cast<double>(ARR_SIZE) / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;
        if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }

    avgMegaMults = sumMegaMults / static_cast<double>(RUNCOUNT);

    std::cout << "\"true\","
        << "\"true\","
        << ARR_SIZE << ","
        << avgMegaMults << ","
        << maxMegaMults << std::endl;
    

    maxMegaMults = 0.;
    sumMegaMults = 0.;
    runningSum = 0.;

    // Non-SIMD version
    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

        for (int j = 0; j < ARR_SIZE; ++j)
        {
            runningSum += dA[j] * dB[j];
        }

        double endTime = ::omp_get_wtime();
        double megaMults = static_cast<double>(ARR_SIZE) / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;
        if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }

    avgMegaMults = sumMegaMults / static_cast<double>(RUNCOUNT);

    std::cout << "\"false\","
        << "\"true\","
        << ARR_SIZE << ","
        << avgMegaMults << ","
        << maxMegaMults << std::endl;

    return 0;
}
