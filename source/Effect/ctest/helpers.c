//
// Created by maple on 25-5-8.
//
#include "helpers.h"
#include "math.h"

double db_to_gain(double db)
{
    return pow(10, db * 0.05);
}
// inline double max(double a, double b)
// {
//     return (a > b) ? a : b;
// }
// inline double min(double a, double b)
// {
//     return (a < b) ? a : b;
// }
double dot_product(const filter_middle_coeff x, const filter_middle_coeff y)
{
    double result = 0.0;
    for (int i = 0; i < 3; ++i) {
        result += x.coeff[i] * y.coeff[i];
    }
    return result;
}
bandwidth_coeff get_bandwidth(const double w0, const double q)
{
    const double bw = 2 * asinh(0.5 / q) / log(2);
    const double scale = pow(2, bw / 2);
    // return {w0 / scale, w0 * scale};

    bandwidth_coeff coeff={w0 / scale, w0 * scale};
    return  coeff;
}