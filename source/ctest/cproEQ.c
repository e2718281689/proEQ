//
// Created by maple on 25-5-6.
//
#include "cproEQ.h"



#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#define CONSTRAIN(val, min, max) \
	((val) >= (max) ? (max) : ((val) <= (min) ? (min) : (val)))

#define ABS(x) ((x) < 0 ? (-(x)) : (x))

#define MAP_FLOAT(sourceValue,sourceRangeMin,sourceRangeMax,targetRangeMin,targetRangeMax) \
		((targetRangeMin) + (((targetRangeMax) - (targetRangeMin)) * ((sourceValue) - (sourceRangeMin))) / ((sourceRangeMax) - (sourceRangeMin)))


float eq_coeff[16][5] ={
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	},
	{
	    0.91158229624973,
        -1.823164592499459,
        0.91158229624973,
        -1.815361508827218,
        0.830981225202565
	}
};


enum coeff
{
      A0=0, A1, A2, B1, B2
};

enum buf_index
{
      IN0=0, IN1, IN2, OUT0, OUT1, OUT2
};

void AudioEffectproEqConfigure(proEqUnit *unit)
{
	if(!unit->enable)
	{
		return;
	}

	if(unit->ct != NULL)
	{

	}
}

uint8_t eq_switch[16]= {1,1,0,0,
						1,1,0,0,
						1,1,0,0,
						1,1,0,0};

void pro_eq_init(proEqtext* ct, uint8_t channel, uint32_t sample_rate)
{
	ct->sample_rate=sample_rate;
	ct->channel=channel;
}

void pro_eq_apply(proEqtext* ct, float *pcm_in, float *pcm_out, uint32_t n)
{

	uint16_t filter = 0, s = 0, ch = 0;
	float out = 0;
	float input = 0;

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
void AudioEffectproEqApply(proEqUnit *unit, float *pcm_in, float *pcm_out, uint32_t n)
{
	if((unit->enable) && (unit->ct != NULL))
	{
		pro_eq_apply(unit->ct,pcm_in,pcm_out,n);
	}
}
