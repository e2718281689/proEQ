//
// Created by maple on 25-5-6.
//
#include "cproEQ.h"
#include "filter_design.h"


#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#define CONSTRAIN(val, min, max) \
	((val) >= (max) ? (max) : ((val) <= (min) ? (min) : (val)))

#define ABS(x) ((x) < 0 ? (-(x)) : (x))

#define MAP_double(sourceValue,sourceRangeMin,sourceRangeMax,targetRangeMin,targetRangeMax) \
		((targetRangeMin) + (((targetRangeMax) - (targetRangeMin)) * ((sourceValue) - (sourceRangeMin))) / ((sourceRangeMax) - (sourceRangeMin)))


enum coeff
{
    A0=0, A1, A2, B1, B2
};

enum buf_index
{
    IN0=0, IN1, IN2, OUT0, OUT1, OUT2
};

typedef uint8_t FilterType;

void proEq_inform_Configure(proEqtext* ct, uint8_t filter_index, uint8_t filter, uint8_t db_oct, float f, float gDB, float q0 , uint8_t enable)
{
    memset(ct->eq_coeff[filter_index], 0, sizeof(float) * FILTER_MAX_ORDER_NUM * FILTER_COEFFS_NUM);
    memset(ct->eq_filter_order_switch[filter_index], 0, sizeof(float) * FILTER_MAX_ORDER_NUM);

    uint8_t number = db_oct/6;
    filter_coeff xxx[8]={0};
    uint32_t filter_order =0;
    float fs = ct->sample_rate;
    filter_order = updateCoeffs(filter,number,f,fs,gDB,q0,xxx);
    for (int order = 0; order < filter_order; ++order)
    {

        ct->eq_coeff[filter_index][order][0]=xxx[order].coeff[3]/xxx[order].coeff[0];
        ct->eq_coeff[filter_index][order][1]=xxx[order].coeff[4]/xxx[order].coeff[0];
        ct->eq_coeff[filter_index][order][2]=xxx[order].coeff[5]/xxx[order].coeff[0];
        ct->eq_coeff[filter_index][order][3]=xxx[order].coeff[1]/xxx[order].coeff[0];
        ct->eq_coeff[filter_index][order][4]=xxx[order].coeff[2]/xxx[order].coeff[0];

        ct->eq_filter_order_switch[filter_index][order] = 1;
    }

    ct->eq_filter_switch[filter_index] = 1;

    memset(ct->buf[0][filter_index], 0, sizeof(float) * FILTER_MAX_ORDER_NUM * HISTORICAL_VALUE_NUM );
}

void AudioEffectproEqConfigure(proEqUnit *unit, uint8_t filter_index)
{
	if(!unit->enable)
	{
		return;
	}

	if(unit->ct != NULL)
	{

	    proEq_inform_Configure(unit->ct,
	                            filter_index,
	                            unit->filter[filter_index].filter_mod,
	                            unit->filter[filter_index].filter_dboct,
	                            unit->filter[filter_index].filter_f,
	                            unit->filter[filter_index].filter_gain,
	                            unit->filter[filter_index].filter_q,
	                            unit->filter[filter_index].enable);

	}
}



void pro_eq_init(proEqtext* ct, uint8_t channel, uint32_t sample_rate)
{
	ct->sample_rate=sample_rate;
	ct->channel=channel;
}

void pro_eq_apply(proEqtext* ct, double *pcm_in, double *pcm_out, uint32_t n)
{
	uint16_t filter = 0, s = 0, ch = 0;
    uint16_t order = 0;
	float out = 0;
	float input = 0;

	for(ch = 0; ch<(ct->channel); ch++)
	{
		for (s = 0; s < n; s++)
		{
			input = pcm_in[ (ct->channel) * s + ch ];

			for (filter = 0; filter < 16 ;filter++)
			{
				if(ct->eq_filter_switch[filter] == 1)
				{
				    for (order = 0; order < FILTER_MAX_ORDER_NUM ;order++)
				    {
				        if (ct->eq_filter_order_switch[filter][order] == 1)
				        {
				            ct->buf[ch][filter][order][IN2] = ct->buf[ch][filter][order][IN1];
				            ct->buf[ch][filter][order][IN1] = ct->buf[ch][filter][order][IN0];
				            ct->buf[ch][filter][order][IN0] = input;

				            ct->buf[ch][filter][order][OUT2] = ct->buf[ch][filter][order][OUT1];
				            ct->buf[ch][filter][order][OUT1] = ct->buf[ch][filter][order][OUT0];

				            ct->buf[ch][filter][order][OUT0] =   (ct->eq_coeff[filter][order][A0] * input)
                                                        + (ct->eq_coeff[filter][order][A1] * ct->buf[ch][filter][order][IN1])
                                                        + (ct->eq_coeff[filter][order][A2] * ct->buf[ch][filter][order][IN2])
                                                        - (ct->eq_coeff[filter][order][B1] * ct->buf[ch][filter][order][OUT1])
                                                        - (ct->eq_coeff[filter][order][B2] * ct->buf[ch][filter][order][OUT2]);

				            out = ct->buf[ch][filter][order][OUT0];
				            input = out;
				        }

				    }
				}
				else
				{
					out = input;
				}
			}
			pcm_out[ (ct->channel) * s + ch  ] = out;
		}
	}
}

void AudioEffectproEqInit(proEqUnit* unit, uint8_t channel, uint32_t sample_rate)
{

    if(unit->channel == 0)
    {
		unit->channel = channel;
    }

	channel = unit->channel;

	if(!unit->enable)
	{
		return;
	}

	if(unit->ct == NULL)
	{
		unit->ct = (proEqtext *)malloc(sizeof(proEqtext));
		if(unit->ct == NULL)
		{
			unit->enable = 0;
//			DBG("VocalCutContext mem malloc err! %ld\n",sizeof(proEqtext));
		}
	}

	if(unit->ct != NULL)
	{
		memset(unit->ct, 0, sizeof(proEqtext));

		pro_eq_init(unit->ct,channel,sample_rate);
		// AudioEffectproEqConfigure(unit);
	}
}

void AudioEffectproEqApply(proEqUnit *unit, double *pcm_in, double *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		pro_eq_apply(unit->ct,pcm_in,pcm_out,n);
	}
}
