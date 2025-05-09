//
// Created by maple on 25-5-8.
//

#ifndef FILTER_DESIGN_H
#define FILTER_DESIGN_H

#ifdef __cplusplus
extern "C" {
    #endif



#include  "helpers.h"


uint32_t updateCoeffs( enum FilterType filterType, const uint8_t n,
                    const double f, const double fs, const double gDB, const double q0,
                    filter_coeff coeffs[16]);

    #ifdef __cplusplus
}
#endif

#endif //FILTER_DESIGN_H
