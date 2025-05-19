//
// Created by maple on 25-5-6.
//

#ifndef COMBFILTER_H
#define COMBFILTER_H

#include <juce_audio_processors/juce_audio_processors.h>
#include "../ProcessorBase.h"
#include <juce_dsp/juce_dsp.h>
class CombFilter  : public ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    CombFilter()
    {

    }

    ~CombFilter()
    {
        Apvts->removeParameterListener("CombFcSlider", this);

    }

    void parameterChanged(const juce::String& parameterID, float newValue)
    {
        if (parameterID.equalsIgnoreCase ("CombFcSlider"))
        {
            delayLine.setDelay (Spec.sampleRate / newValue);
            juce::Logger::outputDebugString ("Comb delay = " + juce::String (Spec.sampleRate / newValue));
        }
        if (parameterID.equalsIgnoreCase ("CombGainSlider"))
        {
            CombGain = newValue;
            juce::Logger::outputDebugString ("Comb Gain = " + juce::String (CombGain));
        }
    }

    void init(juce::AudioProcessorValueTreeState *apvts) override
    {
        Apvts = apvts;

        Apvts->addParameterListener("CombFcSlider", this);
        Apvts->addParameterListener("CombGainSlider", this);

    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {

        Spec.numChannels = 2;
        Spec.sampleRate = sampleRate;
        Spec.maximumBlockSize = samplesPerBlock;

        delayLine.reset();
        delayLine.setMaximumDelayInSamples(44100);
        delayLine.prepare(Spec);

        float newValue = Apvts->getParameterAsValue("CombFcSlider").getValue();
        delayLine.setDelay (Spec.sampleRate/newValue);

        CombGain = Apvts->getParameterAsValue("CombGainSlider").getValue();
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
    /*
        float* pcm_L = buffer.getWritePointer (0);
        float* pcm_R = buffer.getWritePointer (1);

        float* stere[2]={pcm_L,pcm_R};

        size_t numSamples = buffer.getNumSamples();

        float *pcm = new float[2 * numSamples];
        for (size_t i = 0; i < numSamples; ++i)
        {
            pcm[2 * i + 1 ]=pcm_L[i];
            pcm[2 * i + 0 ]=pcm_R[i];
        }

        uint16_t filter = 0, s = 0, ch = 0;
        float out = 0;
        float input = 0;

        for(ch = 0; ch<(2); ch++)
        {

            for (s = 0; s < numSamples; s++)
            {
                input = pcm[ (2) * s + ch ];

                delayLine.pushSample(ch,input);
                input = input + + CombGain * delayLine.popSample(ch);
                out = input;

                pcm[ (2) * s + ch  ] = out ;
            }
        }


        for (size_t i = 0; i < numSamples; ++i)
        {
            pcm_L[i] = pcm[2 * i + 1 ];
            pcm_R[i] = pcm[2 * i + 0 ];
        }

        delete[] pcm;

*/
        // for (size_t ch = 0; ch < 2; ch++)
        // {
        //     for(size_t i = 0; i < numSamples; i++)
        //     {
        //         float input = stere[ch][i];
        //         delayLine.pushSample(ch,input);
        //         input = input + CombGain * delayLine.popSample(ch);
        //         stere[ch][i] = input;
        //     }
        // }
    }

    void reset() override
    {
            delayLine.reset();
    }


    const juce::String getName() const override { return "Comb Filter"; }

private:

    float CombGain;
    juce::dsp::ProcessSpec Spec;
    juce::dsp::DelayLine<float> delayLine;
    juce::AudioProcessorValueTreeState* Apvts;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CombFilter)
};



#endif //COMBFILTER_H
