//
// Created by maple on 25-5-8.
//
#include "filter_design.h"
#include "stdint.h"
#include "math.h"
#include "helpers.h"
#include "martin_coeff.h"

uint32_t updatePassCoeffs(const uint8_t n, const uint32_t startIdx,
                        const double w0, const double q0,
                        filter_coeff coeffs[16],
                        filter_coeff (*firstOrderFunc)(const double ),
                        filter_coeff (*func)(const double ,const double )
                        )
{
    if (n == 1)
    {
        const filter_coeff coeff = firstOrderFunc(w0);
        // coeffs[startIdx] = {coeff.coeff[0], coeff.coeff[1], 0.0, coeff.coeff[2], coeff.coeff[3], 0.0};
        coeffs[startIdx].coeff[0] = coeff.coeff[0];
        coeffs[startIdx].coeff[1] = coeff.coeff[1];
        coeffs[startIdx].coeff[2] = 0.0;
        coeffs[startIdx].coeff[3] = coeff.coeff[2];
        coeffs[startIdx].coeff[4] = coeff.coeff[3];
        coeffs[startIdx].coeff[5] = 0.0;
        return 1;
    }
    const uint32_t number = n / 2;
    const double theta0 = pi / (double)(number) / 4;
    const double scale = pow(sqrt(2.0) * q0, 1 / (double)(number));
    const double rescale_base = log10(sqrt(2.0) * q0) / pow((double)(n), 1.5) * 12;
    for (uint32_t i = 0; i < number; i++)
    {
        const double centered = (double)(i) - (double)(number) / 2 + 0.5;
        const double rescale = centered * rescale_base;
        const double theta = theta0 * (double)(2 * i + 1);
        const double qs = 1.0 / 2.0 / cos(theta) * scale * pow(2, rescale);
        coeffs[i + startIdx] = func(w0, qs);
    }
    return number;
}

uint32_t updateShelfCoeffs(const uint32_t n, const uint32_t startIdx,
                         const double w0, const double g0, const double q0,
                         filter_coeff coeffs[16],
                         filter_coeff (*firstOrderFunc)(const double, const double),
                         filter_coeff (*func)(const double, const double, const double)
                         )
{
    if (n == 1)
    {
        const filter_coeff coeff = firstOrderFunc(w0, g0);
        // coeffs[startIdx] = {coeff[0], coeff[1], 0.0, coeff[2], coeff[3], 0.0};
        coeffs[startIdx].coeff[0] = coeff.coeff[0];
        coeffs[startIdx].coeff[1] = coeff.coeff[1];
        coeffs[startIdx].coeff[2] = 0.0;
        coeffs[startIdx].coeff[3] = coeff.coeff[2];
        coeffs[startIdx].coeff[4] = coeff.coeff[3];
        coeffs[startIdx].coeff[5] = 0.0;
        return 1;
    }
    const uint32_t number = n / 2;
    const double _g = pow(g0, 1.0 / (double)(number));
    const double theta0 = pi / (double)(number) / 4;
    const double scale = pow(sqrt(2.0) * q0, 1 / (double)(number));
    const double rescale_base = log10(sqrt(2.0) * q0) / pow((double)(n), 1.5) * 12;
    for (uint32_t i = 0; i < number; i++) {
        const double centered = (double)(i) - (double)(number) / 2 + 0.5;
        const double rescale = centered * rescale_base;
        const double theta = theta0 * (double)(2 * i + 1);
        const double _q = 1.0 / 2.0 / cos(theta) * scale * pow(2, rescale);
        coeffs[i + startIdx] = func(w0, _g, _q);
    }
    return number;
}

uint32_t updateBandPassCoeffs(const uint32_t n, const uint32_t startIdx,
                            const double w0, const double q0,
                            filter_coeff coeffs[16],
                            filter_coeff (*func)(const double, const double)
                            )
{
    if (n < 2) { return 0; }
    const uint32_t number = n / 2;
    const double halfbw = asinh(0.5 / q0) / log(2);
    const double w = w0 / pow(2, halfbw);
    const double g = db_to_gain(-6 / (double)(n));
    const double _q = sqrt(1 - g * g) * w * w0 / g / (w0 * w0 - w * w);

    const filter_coeff singleCoeff = func(w0, _q);
    for (uint32_t i = 0; i < n / 2; ++i) {
        coeffs[i + startIdx] = singleCoeff;
    }
    return number;
}

uint32_t updateNotchCoeffs(const uint32_t n, const uint32_t startIdx,
                         const double w0, const double q0,
                         filter_coeff coeffs[16],
                         filter_coeff (*func)(const double, const double)
                         )
{
    if (n < 2) { return 0; }
    const uint32_t number = n / 2;
    const double halfbw = asinh(0.5 / q0) / log(2);
    const double w = w0 / pow(2, halfbw);
    const double g = db_to_gain(-6 / (double)(n));
    const double _q = g * w * w0 / sqrt((1 - g * g)) / (w0 * w0 - w * w);

    const filter_coeff singleCoeff = func(w0, _q);
    for (uint32_t i = 0; i < n / 2; ++i) {
        coeffs[i + startIdx] = singleCoeff;
    }
    return number;
}

uint32_t updateBandShelfCoeffs(const uint32_t n, const uint32_t startIdx,
                             const double w0, const double g0, const double q0,
                             filter_coeff coeffs[16],
                             filter_coeff (*lowShelfFirstOrderFunc)(const double, const double),
                             filter_coeff (*lowShelfFunc)(const double, const double, const double),
                             filter_coeff (*highShelfFirstOrderFunc)(const double, const double),
                             filter_coeff (*highShelfFunc)(const double, const double, const double)
                             )
{
    if (n < 2) { return 0; }
    const double halfbw = asinh(0.5 / q0) / log(2);
    const double scale = pow(2, halfbw);
    const double w1 = w0 / scale;
    const double w2 = w0 * scale;
    const double f1 = w1 > 10.0 * 2 * pi / 48000, f2 = w2 < 22000.0 * 2 * pi / 48000;
    uint32_t n1 = 1;
    uint32_t n2 = 0;
    if (f1 && f2) {
        n1 = updateShelfCoeffs(n, startIdx, w1, 1 / g0, sqrt(2) / 2, coeffs, lowShelfFirstOrderFunc,lowShelfFunc);
        n2 = updateShelfCoeffs(n, startIdx + n1, w2, g0, sqrt(2) / 2, coeffs, lowShelfFirstOrderFunc,lowShelfFunc);
    } else if (f1) {
        n1 = updateShelfCoeffs(n, startIdx, w1, g0, sqrt(2) / 2, coeffs, highShelfFirstOrderFunc, highShelfFunc);
    } else if (f2) {
        n1 = updateShelfCoeffs(n, startIdx, w2, g0, sqrt(2) / 2, coeffs, lowShelfFirstOrderFunc,lowShelfFunc);
    } else {
        filter_coeff coeffs_one = {1, 1, 1, g0, g0, g0};
        coeffs[startIdx] = coeffs_one;
    }
    return n1 + n2;
}



uint32_t updateCoeffs( enum FilterType filterType, const uint8_t n,
                    const double f, const double fs, const double gDB, const double q0,
                    filter_coeff coeffs[16])
{
    const double w0 = ppi * f / fs;
    const double g0 = db_to_gain(gDB);
    switch (filterType) {
        case peak:
            switch (n) {
                case 0:
                case 1: return 0;
                case 2: {
                    coeffs[0] = get2Peak(w0, g0, q0);
                    return 1;
                }
                default: {
                    return updateBandShelfCoeffs(n, 0, w0, g0, q0, coeffs,
                                                    get1LowShelf,
                                                    get2LowShelf,
                                                    get1HighShelf,
                                                    get2HighShelf);
                }
            }
        case lowShelf:
            return updateShelfCoeffs(
                n, 0,
                w0, g0, sqrt(q0 * sqrt(2)) / sqrt(2),
                coeffs,get1LowShelf,get2LowShelf);
        case lowPass:
            return updatePassCoeffs(
                n, 0,
                w0, q0,
                coeffs,get1LowPass,get2LowPass);
        case highShelf:
            return updateShelfCoeffs(
                n, 0,
                w0, g0, sqrt(q0 * sqrt(2)) / sqrt(2),
                coeffs,get1HighShelf,get2HighShelf);
        case highPass:
            return updatePassCoeffs(
                n, 0,
                w0, q0,
                coeffs,get1HighPass,get2HighPass);
        case bandShelf:
            return updateBandShelfCoeffs(n, 0, w0, g0, q0, coeffs,
                                            get1LowShelf,
                                            get2LowShelf,
                                            get1HighShelf,
                                            get2HighShelf);
        case tiltShelf:
            return updateShelfCoeffs(
                n, 0,
                w0, g0, sqrt(q0 * sqrt(2)) / sqrt(2),
                coeffs,get1TiltShelf,get2TiltShelf);
        case notch:
            return updateNotchCoeffs(n, 0, w0, q0, coeffs,get2Notch);
        case bandPass:
            return updateBandPassCoeffs(n, 0, w0, q0, coeffs,get2BandPass);
        default:
            return 0;
    }
}