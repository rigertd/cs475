/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #3
* File:         project3.cpp
* Created:      2017-04-25
* Modified:     2017-04-26
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
#include <memory>

#include <omp.h>

#define RUNCOUNT 5
#define CACHE_LINE_SIZE 64

// Use 1 int as padding by default (for MSVC compatibility)
#ifndef NUMPAD
#define NUMPAD 1
#endif

struct s
{
    float value;
    int pad[NUMPAD];
};

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

    const int bigNumber = 50000000;

    double maxMegaAdds = 0.;
    double sumMegaAdds = 0.;

    // C++ memory alignment 
    // http://stackoverflow.com/questions/20791428/what-is-the-recommended-way-to-align-memory-in-c11
    // http://en.cppreference.com/w/cpp/memory/align
    std::size_t needed = numThreads * sizeof(s) + CACHE_LINE_SIZE;
    char *alloc = new char[needed];
    void *tmp = static_cast<void*>(alloc);
    s *Array = reinterpret_cast<s*>(std::align(CACHE_LINE_SIZE, sizeof(s), tmp, needed));

    for (int i = 0; i < RUNCOUNT; ++i)
    {
        double startTime = ::omp_get_wtime();

#pragma omp parallel for default(none), shared(Array, numThreads)
        for (int j = 0; j < numThreads; ++j)
        {
            // Use separate seed buffer for each thread
            unsigned seedp = std::time(nullptr);

            // Use temporary local variable if LOCAL macro is defined
#ifdef LOCAL
            float tmp = Array[j].value;
#endif
            for (int k = 0; k < bigNumber; ++k)
            {
#ifdef LOCAL
                tmp = tmp + static_cast<float>(rand_r(&seedp));
#else
                Array[j].value = Array[j].value + static_cast<float>(rand_r(&seedp));
#endif
            }
#ifdef LOCAL
            // Store temp result back in array
            Array[j].value = tmp;
#endif
        }

        double endTime = ::omp_get_wtime();
        double megaAdds = static_cast<double>(static_cast<long>(bigNumber) * numThreads) / (endTime - startTime) / 1000000.;
        sumMegaAdds += megaAdds;
        if (megaAdds > maxMegaAdds) maxMegaAdds = megaAdds;
    }

    // Deallocate array
    delete [] alloc;

    double avgMegaAdds = sumMegaAdds / static_cast<double>(RUNCOUNT);

    std::cout << numThreads << ","
        << (NUMPAD * sizeof(int)) << ","
        << avgMegaAdds << ","
        << maxMegaAdds << std::endl;

    return 0;
}
