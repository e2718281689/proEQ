//
// Created by maple on 25-5-8.
//

#ifndef MARTIN_COEFF_H
#define MARTIN_COEFF_H

#ifdef __cplusplus
extern "C" {
    #endif


#include "helpers.h"

filter_middle_coeff solve_a(const double w0, const double b, const double c);
filter_middle_coeff get_AB(filter_middle_coeff a);
uint8_t check_AB(filter_middle_coeff A);
filter_middle_coeff get_ab(filter_middle_coeff A);
filter_middle_coeff get_phi(const double w);


filter_coeff get1LowPass(double w0);

filter_coeff get1HighPass(double w0);

filter_coeff get1TiltShelf(double w0, double g);

filter_coeff get1LowShelf(double w0, double g);

filter_coeff get1HighShelf(double w0, double g);

filter_coeff get2LowPass(double w0, double q);

filter_coeff get2HighPass(double w0, double q);

filter_coeff get2BandPass(double w0, double q);

filter_coeff get2Notch(double w0, double q);

filter_coeff get2Peak(double w0, double g, double q);

filter_coeff get2TiltShelf(double w0, double g, double q);

filter_coeff get2LowShelf(double w0, double g, double q);

filter_coeff get2HighShelf(double w0, double g, double q);
    
    #ifdef __cplusplus
}
#endif

#endif //MARTIN_COEFF_H
