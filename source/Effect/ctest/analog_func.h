//
// Created by maple on 25-5-8.
//

#ifndef ANALOG_FUNC_H
#define ANALOG_FUNC_H

#ifdef __cplusplus
extern "C" {
#endif

double get2LowPassMagnitude2(double w0, double q, double w);

double get2TiltShelfMagnitude2(double w0, double g, double q, double w);


#ifdef __cplusplus
}
#endif

#endif //ANALOG_FUNC_H
