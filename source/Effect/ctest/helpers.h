//
// Created by maple on 25-5-8.
//

#ifndef HELPERS_H
#define HELPERS_H
#include "stdint.h"
#include "stdlib.h"
#ifdef __cplusplus
extern "C" {
    #endif

enum FilterType
{
    peak=0,
    lowShelf,
    lowPass,
    highShelf,
    highPass,
    bandShelf,
    tiltShelf,
    notch,
    bandPass
};


typedef struct _filter_coeff
{
    double coeff[6];

}filter_coeff;

typedef struct _filter_middle_coeff
{
    double coeff[3];

}filter_middle_coeff;

typedef struct _bandwidth_coeff
{
    double coeff[2];

}bandwidth_coeff;

#define pi (3.14159265359)
#define ppi (2 * pi)
#define pi2 (pi * pi)
#define piHalf (0.5 * pi)

typedef  uint8_t bool;
#define false (0)
#define true (1)

// typedef uint32_t size_t;

double db_to_gain(double db);
// double max(double a, double b);
// double min(double a, double b);


filter_middle_coeff linear_solve(const filter_middle_coeff A[3],const filter_middle_coeff b);
bandwidth_coeff get_bandwidth(const double w0, const double q);
double dot_product(const filter_middle_coeff x, const filter_middle_coeff y);


    #ifdef __cplusplus
}
#endif

#endif //HELPERS_H
