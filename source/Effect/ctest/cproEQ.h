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
#define CHANNEL_NUM 2
#define HISTORICAL_VALUE_NUM 6

typedef struct _proEqtext
{
    uint8_t         channel;
    uint32_t 		sample_rate;
    double 		buf[CHANNEL_NUM][FILTER_NUM][HISTORICAL_VALUE_NUM];
}proEqtext;

typedef struct _proEqUnit
{
    proEqtext      *ct;
    uint8_t              channel;
    uint32_t 			 sample_rate;
    uint32_t      		 enable;


}proEqUnit;


void AudioEffectproEqInit(proEqUnit* unit, uint8_t channel, uint32_t sample_rate);

void AudioEffectproEqApply(proEqUnit *unit, double *pcm_in, double *pcm_out, uint32_t n);

void AudioEffectproEqConfigure(proEqUnit *unit);

#ifdef __cplusplus
}
#endif

#endif /* CPROEQ_H */
