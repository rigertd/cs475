/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #2
* File:         project2.cpp
* Created:      2017-04-18
* Modified:     2017-04-18
* Description:  Static vs Dynamic and Small vs Large Chunksize
*
*               This program compares the performance of static and dynamic
*               scheduling, and of a small chunksize and a large chunksize.
*               To specify static/dynamic and the chunksize, use the following
*               compiler options.
*                   -DSCHEDULING=static/dynamic,
*                   -DCHUNKSIZE=1/4096
*               The output is provided in the following format:
*
*                   threads, scheduling, chunksize, avg megamults/sec, peak megamults/sec
*
*               This program uses the following command-line syntax:
*
*                   ./prog num_threads
********************************************************************/
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <limits>

#include <omp.h>

#define ARRAYSIZE 32 * 1024
#define RUNCOUNT 20

// Macro to string conversion from Stack Overflow:
// http://stackoverflow.com/questions/20631922/expand-macro-inside-string-literal
#define STRING2(X) #X
#define STRING(X) STRING2(X)

// Default definition for scheduling type is static
#ifndef SCHEDULING
#define SCHEDULING static
#endif

// Default definition for chunksize is 1
#ifndef CHUNKSIZE
#define CHUNKSIZE 1
#endif

// Global floating point array of ARRAYSIZE
float Array[ARRAYSIZE];


/**
* Returns a random floating point number between 0 and 1.
* Copied from project 2.
* (http://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj02.html)
*/
float Ranf(float low, float high)
{
    float r = (float)rand();
    return low + r * (high - low) / static_cast<float>(RAND_MAX);
}

int main(int argc, char *argv[])
{
    // Test for OpenMP support
#ifndef _OPENMP
    std::cerr << "OpenMP support is required to run this program." << std::endl;
    return 1;
#endif

    int numThreads = 0;

    // Command line argument error handling
    if (argc == 2) {
        char *end;
        // Get number of threads
        long arg = std::strtol(argv[1], &end, 10);
        // If there are still characters in the buffer, invalid input
        if (*end != '\0') {
            std::cerr << "num_threads must be an integer." << std::endl
                << "Usage: " << argv[0] << " num_threads" << std::endl;
            return 1;
        }
        // Must be greater than 0
        else if (arg < 1) {
            std::cerr << "num_threads must be greater than 0." << std::endl
                << "Usage: " << argv[0] << " num_threads" << std::endl;
            return 1;
        }
        // Too big to fit in an int (would probably run out of memory anyway)
        else if (arg > std::numeric_limits<int>::max()) {
            std::cerr << "num_threads cannot be larger than "
                << std::numeric_limits<int>::max() << "." << std::endl
                << "Usage: " << argv[0] << " num_threads" << std::endl;
            return 1;
        }
        numThreads = static_cast<int>(arg);
    }
    // Not enough or too many arguments specified
    else {
        std::cerr << "Syntax error." << std::endl
            << "Usage: " << argv[0] << " num_threads" << std::endl;
        return 1;
    }

    // Set number of threads based on command line argument
    ::omp_set_num_threads(numThreads);

    // Fill global array with random values
    for (int i = 0; i < ARRAYSIZE; ++i)
    {
        Array[i] = Ranf(-1.f, 1.f);
    }

    double maxMegaMults = 0.;
    double sumMegaMults = 0.;

    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

#pragma omp parallel for default(none), shared(Array), schedule(SCHEDULING,CHUNKSIZE)
        for (int j = 0; j < ARRAYSIZE; ++j)
        {
            // Use a local variable to keep track of product separately in each thread.
            float product = 1.f;

            for (int k = 0; k <= j; ++k)
            {
                product *= Array[k];
            }
        }

        double endTime = ::omp_get_wtime();
        double megaMults = static_cast<double>(static_cast<long>(ARRAYSIZE) * (static_cast<long>(ARRAYSIZE + 1) / 2)) / (endTime - startTime) / 1000000.;
        sumMegaMults += megaMults;
        if (megaMults > maxMegaMults) maxMegaMults = megaMults;
    }

    double avgMegaMults = sumMegaMults / static_cast<double>(RUNCOUNT);
    /*
    std::cout << std::right << std::setw(10) << "Peak = "
    << std::fixed << std::setprecision(2) << maxMegaOps
    << " MegaOps/Sec" << std::endl;

    std::cout << std::right << std::setw(10) << "Average = "
    << std::fixed << std::setprecision(2) << avgMegaOps
    << " MegaOps/Sec" << std::endl;
    */

    std::cout << numThreads << ",\""
        << STRING(SCHEDULING) << "\","
        << CHUNKSIZE << ","
        << avgMegaMults << ","
        << maxMegaMults << std::endl;

    return 0;
}
