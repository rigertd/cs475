/********************************************************************
* Author:       David Rigert
* Class:        CS475 Spring 2017
* Assignment:   Project #4
* File:         project4.cpp
* Created:      2017-05-04
* Modified:     2017-05-11
* Description:  False Sharing
*
*               This program runs a multithreaded grain growing simulation.
*               The state of the simulation is output once per month in CSV
*               format and includes the following information:
*                   year, month, rain inches, temp, grain height, deer count, brush fire
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

#define _USE_MATH_DEFINES
#include <cmath>

#include <omp.h>

// Square function
inline int sqr(int n)
{
    return n * n;
}

// Random float function
// We don't need rand_r because only 1 thread calls this
float getRand(float low, float high)
{
    return low 
           + static_cast<float>(rand()) 
           * (high - low) 
           / static_cast<float>(RAND_MAX);
}

// Random integer function
inline int getRand(int low, int high)
{
    return static_cast<int>(
        getRand(static_cast<float>(low), static_cast<float>(high))
        );
}

// Global state (from project notes)
struct State
{
    int     nowYear;
    int     nowMonth;
    float   nowPrecip;
    float   nowTemp;
    float   nowHeight;
    int     nowNumDeer;
    int     extraGrowthMonthsLeft;
};

// Global state variable
State state = {0};

// Global constants (from project notes)
const float GRAIN_GROWS_PER_MONTH =     8.0;
const float ONE_DEER_EATS_PER_MONTH =   0.5;

const float AVG_PRECIP_PER_MONTH =      6.0;    // average
const float AMP_PRECIP_PER_MONTH =      6.0;    // plus or minus
const float RANDOM_PRECIP =             2.0;    // plus or minus noise

const float AVG_TEMP =                  50.0;   // average
const float AMP_TEMP =                  20.0;   // plus or minus
const float RANDOM_TEMP =               10.0;   // plus or minus noise

const float MIDTEMP =                   40.0;
const float MIDPRECIP =                 10.0;

const float FIRE_THRESHOLD =            60.0;
const int   EXTRA_GROWTH_MONTHS =       3;

// We only need a fixed number of threads
const int numThreads = 4;

// Update the global temp based on current month
void updateTemp(float ang)
{
    float temp = AVG_TEMP - AMP_TEMP * std::cos(ang);
    state.nowTemp = temp + getRand(-RANDOM_TEMP, RANDOM_TEMP);
}

// Update the global precipitation based on current month
void updatePrecip(float ang)
{
    float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * std::sin(ang);
    state.nowPrecip = precip + getRand(-RANDOM_PRECIP, RANDOM_PRECIP);
    state.nowPrecip = state.nowPrecip < 0. ? 0. : state.nowPrecip;
}

// Forward declarations for simulation functions
void GrainDeer();
void Grain();
void Watcher();
void BrushFire();

int main(int argc, char *argv[])
{
    // Test for OpenMP support
#ifndef _OPENMP
    std::cerr << "OpenMP support is required to run this program." << std::endl;
    return 1;
#endif

    // Always use same random sequence
    srand(0);

    // Set number of threads
    ::omp_set_num_threads(numThreads);

    // Initialize global state
    state.nowMonth = 0;
    state.nowYear = 2017;
    state.nowNumDeer = 1;
    state.nowHeight = 1.;
    state.extraGrowthMonthsLeft = 0;

    // Compute temperature and precipitation (from project notes)
    float ang = (30. * static_cast<float>(state.nowMonth) + 15.) * ( M_PI / 180. );

    updateTemp(ang);
    updatePrecip(ang);

    // Section definition from project notes
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            GrainDeer();
        }

        #pragma omp section
        {
            Grain();
        }

        #pragma omp section
        {
            Watcher();
        }

        #pragma omp section
        {
            BrushFire();
        }
    }

    return 0;
}

// Simulates changes in the number of deer based on grain supplies.
void GrainDeer()
{
    while (state.nowYear < 2023)
    {
        // Declare local variable to store new number of deer
        int nextNumDeer = state.nowNumDeer;
        
        // Update deer quantity based on grain height
        if (nextNumDeer < state.nowHeight)
        {
            // Increase by 1
            nextNumDeer++;
        }
        else if (nextNumDeer > state.nowHeight)
        {
            // Decrease by 1, but clamp at 0
            nextNumDeer = nextNumDeer < 1 ? 0 : nextNumDeer - 1;
        }
        #pragma omp barrier
        
        // Copy into global variable
        state.nowNumDeer = nextNumDeer;
        #pragma omp barrier
        
        // Wait for brush fire update
        #pragma omp barrier
        
        // Do nothing and wait for other threads
        #pragma omp barrier
    }
}

// Simulates changes in grain growth based on growing conditions and deer count.
void Grain()
{
    while (state.nowYear < 2023)
    {
        // Declare local variable to store new grain height
        float nextHeight = state.nowHeight;
        
        // Calculate growing conditions
        float tempFactor = std::exp(-sqr((state.nowTemp - MIDTEMP) / 10.));
        float precipFactor = std::exp(-sqr((state.nowPrecip - MIDPRECIP) / 10.));
        
        // Update height based on growing conditions and number of deer
        float heightGrowth = tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
        // Double growth if there was a fire in the last 2 months
        if (state.extraGrowthMonthsLeft > 0)
        {
            heightGrowth *= 2;
        }
        nextHeight += heightGrowth;
        nextHeight -= static_cast<float>(state.nowNumDeer) * ONE_DEER_EATS_PER_MONTH;
        nextHeight = nextHeight < 0 ? 0 : nextHeight;
        #pragma omp barrier
        
        // Copy into global variable
        state.nowHeight = nextHeight;
        #pragma omp barrier
        
        // Wait for brush fire update
        #pragma omp barrier
        
        // Do nothing and wait for other threads
        #pragma omp barrier
    }
}

// Observes the changes in state and prints them out.
void Watcher()
{
    while (state.nowYear < 2023)
    {
        // Do nothing and wait for other threads
        #pragma omp barrier
        
        // Do nothing and wait for other threads
        #pragma omp barrier
        
        // Do nothing and wait for other threads
        #pragma omp barrier
        
        // Print results
        std::cout << state.nowYear << ","
            << state.nowMonth << ","
            << state.nowPrecip << ","
            << state.nowTemp << ","
            << state.nowHeight << ","
            << state.nowNumDeer << ","
            << (state.extraGrowthMonthsLeft == EXTRA_GROWTH_MONTHS ? 1 : 0) << std::endl;

        // Increment time variables
        if (++state.nowMonth > 11)
        {
            state.nowMonth = 0;
            state.nowYear++;
        }

        // Update temp and precipitation
        float ang = (30. * static_cast<float>(state.nowMonth) + 15.) * ( M_PI / 180. );

        updateTemp(ang);
        updatePrecip(ang);
        #pragma omp barrier
    }
}

// Higher temperatures increase chance of a brush fire that wipes out all grain.
// However, the ash makes grain grow an extra GRAIN_GROWS_PER_MONTH 
// for the next EXTRA_GROWTH_MONTHS months.
void BrushFire()
{
    int extraGrowthMonths = 0;
    
    while (state.nowYear < 2023)
    {
        // Store previous grain height in local variable
        float oldHeight = state.nowHeight;
        #pragma omp barrier
        
        // Wait for other threads
        #pragma omp barrier
        
        // Decrement global state for extra growth months
        if (state.extraGrowthMonthsLeft > 0)
        {
            state.extraGrowthMonthsLeft--;
        }

        // If fire risk is over threshold, and the grain is not all fresh growth,
        // randomly determine if we have a brush fire
        float fireRisk = state.nowTemp - (state.nowPrecip * 2);
        if (oldHeight > 0.f && fireRisk > FIRE_THRESHOLD)
        {
            // Chance depends on fire risk
            int result = getRand(0, RAND_MAX);
            if (result % 100 < fireRisk)
            {
                // Wipe out all grain and set extra growth counter
                state.nowHeight = 0.;
                state.extraGrowthMonthsLeft = EXTRA_GROWTH_MONTHS;
            }
        }
        #pragma omp barrier
        
        // Do nothing and wait for other threads
        #pragma omp barrier
    }
}

