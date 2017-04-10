/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #1
* File:         project1.cpp
* Created:      2017-04-09
* Modified:     2017-04-09
* Description:  Numeric integration with OpenMP written in C++11.
*
*               This program calculates the approximate volume of the 3D
*               space between two objects, times the execution time of the
*               operation, and prints the results to the console as a line
*               of comma separated values in the following order:
*
*                   threads, nodes, volume, avg megaops/sec, peak megaops/sec
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

#define XMIN     0.
#define XMAX     3.
#define YMIN     0.
#define YMAX     3.

/**
 * Coordinates of the two shapes, TOP and BOT.
 * Copied from project 1.
 * (http://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj01.html)
 */
#define TOPZ00  0.
#define TOPZ10  1.
#define TOPZ20  0.
#define TOPZ30  0.

#define TOPZ01  1.
#define TOPZ11  6.
#define TOPZ21  1.
#define TOPZ31  0.

#define TOPZ02  0.
#define TOPZ12  1.
#define TOPZ22  0.
#define TOPZ32  4.

#define TOPZ03  3.
#define TOPZ13  2.
#define TOPZ23  3.
#define TOPZ33  3.

#define BOTZ00  0.
#define BOTZ10  -3.
#define BOTZ20  0.
#define BOTZ30  0.

#define BOTZ01  -2.
#define BOTZ11  10.
#define BOTZ21  -2.
#define BOTZ31  0.

#define BOTZ02  0.
#define BOTZ12  -5.
#define BOTZ22  0.
#define BOTZ32  -6.

#define BOTZ03  -3.
#define BOTZ13   2.
#define BOTZ23  -8.
#define BOTZ33  -3.

// Forward declaration of Height function
float Height(int, int);

// Define number of nodes as 100 if not defined during compilation
#ifndef NUMNODES
#define NUMNODES 100
#endif
// Define number of times to run the operation
#define RUNCOUNT 20

// Stores number of threads
int numThreads;

int main(int argc, char *argv[])
{
    // Test for OpenMP support
#ifndef _OPENMP
    std::cerr << "OpenMP support is required to run this program." << std::endl;
    return 1;
#endif

    // Command line argument error handling
    if (argc == 2) {
        char *end;
        // Get number of threads
        int arg = std::strtol(argv[1], &end, 10);
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
    //std::cout << "Using " << numThreads <<  " threads" << std::endl;
    //std::cout << "Using " << NUMNODES << " nodes" << std::endl;

    double maxMegaOps = 0.;
    double sumMegaOps = 0.;
    float totalVolume = 0.;

    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

        // Calculate volume
        totalVolume = 0.;
        #pragma omp parallel for collapse(2)
        for (int iv = 0; iv < NUMNODES; ++iv)
        {
            for (int iu = 0; iu < NUMNODES; ++iu)
            {
                float tileArea = ((XMAX - XMIN) / static_cast<float>(NUMNODES - 1)) * ((YMAX - YMIN) / static_cast<float>(NUMNODES - 1));
                // Halve the result if lowest or highest v
                if (iv == 0 || iv == NUMNODES - 1) tileArea /= 2;
                // Halve the result if lowest or highest u
                if (iu == 0 || iu == NUMNODES - 1) tileArea /= 2;
                // Multiply by height to get volume
                float volume = Height(iu, iv) * tileArea;
                #pragma omp critical
                totalVolume += volume;
            }
        }

        double endTime = ::omp_get_wtime();
        double megaOps = static_cast<double>(NUMNODES * NUMNODES) / (endTime - startTime) / 1000000.;
        sumMegaOps += megaOps;
        if (megaOps > maxMegaOps) maxMegaOps = megaOps;
    }

    /*
    std::cout << "Total volume = "
              << std::setprecision(6) << totalVolume << std::endl;
    */
    double avgMegaOps = sumMegaOps / static_cast<double>(RUNCOUNT);
    /*
    std::cout << std::right << std::setw(10) << "Peak = "
              << std::fixed << std::setprecision(2) << maxMegaOps
              << " MegaOps/Sec" << std::endl;

    std::cout << std::right << std::setw(10) << "Average = "
              << std::fixed << std::setprecision(2) << avgMegaOps
              << " MegaOps/Sec" << std::endl;
    */

    std::cout << numThreads << ","
              << NUMNODES << ","
              << totalVolume << ","
              << avgMegaOps << ","
              << maxMegaOps << std::endl;

    return 0;
}

/**
 * Calculates the height between the two shapes based on the number of nodes.
 * Copied from project 1.
 * (http://web.engr.oregonstate.edu/~mjb/cs575/Projects/proj01.html)
 */
float Height(int iu, int iv)    // iu,iv = 0 .. numNodes-1
{
    float u = static_cast<float>(iu) / static_cast<float>(NUMNODES - 1);
    float v = static_cast<float>(iv) / static_cast<float>(NUMNODES - 1);

    // the basis functions:

    float bu0 = (1. - u) * (1. - u) * (1. - u);
    float bu1 = 3. * u * (1. - u) * (1. - u);
    float bu2 = 3. * u * u * (1. - u);
    float bu3 = u * u * u;

    float bv0 = (1. - v) * (1. - v) * (1. - v);
    float bv1 = 3. * v * (1. - v) * (1. - v);
    float bv2 = 3. * v * v * (1. - v);
    float bv3 = v * v * v;

    // finally, we get to compute something:


    float top = bu0 * (bv0*TOPZ00 + bv1*TOPZ01 + bv2*TOPZ02 + bv3*TOPZ03)
        + bu1 * (bv0*TOPZ10 + bv1*TOPZ11 + bv2*TOPZ12 + bv3*TOPZ13)
        + bu2 * (bv0*TOPZ20 + bv1*TOPZ21 + bv2*TOPZ22 + bv3*TOPZ23)
        + bu3 * (bv0*TOPZ30 + bv1*TOPZ31 + bv2*TOPZ32 + bv3*TOPZ33);

    float bot = bu0 * (bv0*BOTZ00 + bv1*BOTZ01 + bv2*BOTZ02 + bv3*BOTZ03)
        + bu1 * (bv0*BOTZ10 + bv1*BOTZ11 + bv2*BOTZ12 + bv3*BOTZ13)
        + bu2 * (bv0*BOTZ20 + bv1*BOTZ21 + bv2*BOTZ22 + bv3*BOTZ23)
        + bu3 * (bv0*BOTZ30 + bv1*BOTZ31 + bv2*BOTZ32 + bv3*BOTZ33);

    return top - bot;   // if the bottom surface sticks out above the top surface
                        // then that contribution to the overall volume is negative
}

