/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #3
* File:         project3.cpp
* Created:      2017-04-25
* Modified:     2017-04-25
* Description:  False Sharing
*
*               This program demonstrates the effects of false sharing
*               on performance and measures the effectiveness of padding.
*               To specify the padding size, use the following compiler option.
*                   -DNUMPAD    Number of integers to use for padding between values
*               The output is provided in the following format:
*
*                   threads, padding bytes, avg mega-additions/sec, peak mega-additions/sec
*
*               This program uses the following command-line syntax:
*
*                   ./prog num_threads
********************************************************************/
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <limits>

#include <omp.h>

#define RUNCOUNT 10

// Use no padding by default
#ifndef NUMPAD
#define NUMPAD 1
#endif

struct s
{
    float value;
    int pad[NUMPAD];
} Array[4];

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

    // Seed random number generator
    std::srand(time(0));

    // Set number of threads based on command line argument
    ::omp_set_num_threads(numThreads);

    const int bigNumber = 100000000;

    double maxMegaAdds = 0.;
    double sumMegaAdds = 0.;

    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

#pragma omp parallel for default(none), shared(Array)
        for (int j = 0; j < 4; ++j)
        {
            for (int k = 0; k < bigNumber; ++k)
            {
                Array[j].value = Array[j].value + static_cast<float>(std::rand());
            }
        }

        double endTime = ::omp_get_wtime();
        double megaAdds = static_cast<double>(static_cast<long>(bigNumber) * 4) / (endTime - startTime) / 1000000.;
        sumMegaAdds += megaAdds;
        if (megaAdds > maxMegaAdds) maxMegaAdds = megaAdds;
    }

    double avgMegaAdds = sumMegaAdds / static_cast<double>(RUNCOUNT);

    std::cout << numThreads << ","
        << (NUMPAD * sizeof(int)) << ","
        << avgMegaAdds << ","
        << maxMegaAdds << std::endl;

    return 0;
}
