//
// Created by maple on 25-5-6.
//
#ifndef CPROEQ_H
#define CPROEQ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stddef.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

#define FILTER_NUM 16
#define FILTER_COEFFS_NUM 5
#define FILTER_MAX_ORDER_NUM 8
#define CHANNEL_NUM 2
#define HISTORICAL_VALUE_NUM 6



typedef struct _filter_informa
{
    uint8_t     filter_mod;
    uint8_t     filter_dboct;
    float       filter_f;
    float       filter_gain;
    float       filter_q;
    uint8_t     enable;
}filter_informa;

typedef struct _proEqtext
{


    float                eq_coeff[FILTER_NUM][FILTER_MAX_ORDER_NUM][FILTER_COEFFS_NUM];
    float 		         buf[CHANNEL_NUM][FILTER_NUM][FILTER_MAX_ORDER_NUM][HISTORICAL_VALUE_NUM];
    uint8_t              eq_filter_order_switch[FILTER_NUM][FILTER_MAX_ORDER_NUM];
    uint8_t              eq_filter_switch[FILTER_NUM];

    uint8_t              channel;
    uint32_t 		     sample_rate;

}proEqtext;

typedef struct _proEqUnit
{
    filter_informa       filter[FILTER_NUM];

    proEqtext            *ct;
    uint8_t              channel;
    uint32_t 			 sample_rate;
    uint32_t      		 enable;


}proEqUnit;


void AudioEffectproEqInit(proEqUnit* unit, uint8_t channel, uint32_t sample_rate);

void AudioEffectproEqApply(proEqUnit *unit, double *pcm_in, double *pcm_out, uint32_t n);

void AudioEffectproEqConfigure(proEqUnit *unit, uint8_t filter_index);

#ifdef __cplusplus
}
#endif

#endif /* CPROEQ_H */
