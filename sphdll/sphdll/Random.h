#pragma once
#include <ctime>
#include <cstdlib>

void initrand()
{
    srand((unsigned)(time(0)));
}

//generates a psuedo-random float between 0.0 and 0.999... 
float randfloat()
{
    return rand() / (float(RAND_MAX) + 1);
}

//generates a psuedo-random float between 0.0 and max 
float randfloat(float max)
{
    return randfloat() * max;
}

//generates a psuedo-random float between min and max 
float randfloat(float min, float max)
{
    if (min > max)
        return randfloat() * (min - max) + max;
    else
        return randfloat() * (max - min) + min;
}

//generates a psuedo-random double between 0.0 and 0.999... 
double randdouble()
{
    return rand() / (double(RAND_MAX) + 1);
}

//generates a psuedo-random double between 0.0 and max 
double randdouble(double max)
{
    return randdouble() * max;
}

//generates a psuedo-random double between min and max 
double randdouble(double min, double max)
{
    if (min > max)
        return randdouble() * (min - max) + max;
    else
        return randdouble() * (max - min) + min;
}