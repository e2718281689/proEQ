//
// Created by maple on 25-5-8.
//
#include "analog_func.h"
#include "helpers.h"
#include "math.h"



double get2Magnitude2(const filter_coeff coeff, const double w)
{
    const double w_2 = w * w;
    const double t1 = coeff.coeff[2] - coeff.coeff[0] * w_2;
    const double denominator = coeff.coeff[1] * coeff.coeff[1] * w_2 + t1 * t1;
    const double t2 = coeff.coeff[5] - coeff.coeff[3] * w_2;
    const double numerator = coeff.coeff[4] * coeff.coeff[4] * w_2 + t2 * t2;
    return numerator / denominator;
}

double get2LowPassMagnitude2(double w0, double q, double w)
{
    filter_coeff coeff = {1, w0 / q, w0 * w0, 0, 0, w0 * w0};
    return get2Magnitude2(coeff , w);
}

double doubleget2HighPassMagnitude2(double w0, double q, double w)
{
    filter_coeff coeff = {1, w0 / q, w0 * w0, 1, 0, 0};
    return get2Magnitude2(coeff, w);
}

double get2BandPassMagnitude2(double w0, double q, double w)
{
    filter_coeff coeff = {1, w0 / q, w0 * w0, 0, w0 / q, 0};
    return get2Magnitude2(coeff, w);
}

double get2NotchMagnitude2(double w0, double q, double w)
{
    filter_coeff coeff ={1, w0 / q, w0 * w0, 1, 0, w0 * w0};
    return get2Magnitude2(coeff, w);
}

double get2PeakMagnitude2(double w0, double g, double q, double w)
{
    filter_coeff coeff = {1, w0 / sqrt(g) / q, w0 * w0, 1, w0 * sqrt(g) / q, w0 * w0};
    return get2Magnitude2(coeff, w);
}

double get2TiltShelfMagnitude2(double w0, double g, double q, double w)
{
    const double A = sqrt(g);
    filter_coeff coeff = {1, sqrt(A) * w0 / q, A * w0 * w0, A, sqrt(A) * w0 / q, w0 * w0};
    return get2Magnitude2(coeff, w);
}