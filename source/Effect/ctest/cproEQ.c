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

double eq_coeff[16][5] ={0};

uint8_t eq_switch[16]= {1,1,1,1,
                        1,1,1,1,
                        0,0,0,0,
                        0,0,0,0};



void AudioEffectproEqConfigure(proEqUnit *unit)
{
	if(!unit->enable)
	{
		return;
	}

	if(unit->ct != NULL)
	{
	    filter_coeff xxx[16]={0};
	    updateCoeffs(lowPass,16,1000,44100,0,0.707,xxx);
        for (int i = 0; i < 16; ++i)
        {
            eq_coeff[i][0]=xxx[i].coeff[3];
            eq_coeff[i][1]=xxx[i].coeff[4];
            eq_coeff[i][2]=xxx[i].coeff[5];
            eq_coeff[i][3]=xxx[i].coeff[1];
            eq_coeff[i][4]=xxx[i].coeff[2];
        }
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
	double out = 0;
	double input = 0;

	for(ch = 0; ch<(ct->channel); ch++)
	{
		for (s = 0; s < n; s++)
		{
			input = pcm_in[ (ct->channel) * s + ch ];

			for (filter = 0; filter < 16 ;filter++)
			{
				if(eq_switch[filter] == 1)
				{
					ct->buf[ch][filter][IN2] = ct->buf[ch][filter][IN1];
					ct->buf[ch][filter][IN1] = ct->buf[ch][filter][IN0];
					ct->buf[ch][filter][IN0] = input;

					ct->buf[ch][filter][OUT2] = ct->buf[ch][filter][OUT1];
					ct->buf[ch][filter][OUT1] = ct->buf[ch][filter][OUT0];

					ct->buf[ch][filter][OUT0] =   (eq_coeff[filter][A0] * input)
										        + (eq_coeff[filter][A1] * ct->buf[ch][filter][IN1])
										        + (eq_coeff[filter][A2] * ct->buf[ch][filter][IN2])
										        - (eq_coeff[filter][B1] * ct->buf[ch][filter][OUT1])
										        - (eq_coeff[filter][B2] * ct->buf[ch][filter][OUT2]);

					out = ct->buf[ch][filter][OUT0];
					input = out;
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
		AudioEffectproEqConfigure(unit);
	}
}

void AudioEffectproEqApply(proEqUnit *unit, double *pcm_in, double *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		pro_eq_apply(unit->ct,pcm_in,pcm_out,n);
	}
}
