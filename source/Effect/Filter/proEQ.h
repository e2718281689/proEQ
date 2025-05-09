//
// Created by maple on 25-5-6.
//

#ifndef PROEQ_H
#define PROEQ_H

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../ProcessorBase.h"
#include "../ctest/cproEQ.h"

class proEQ  : public ProcessorBase, public juce::AudioProcessorValueTreeState::Listener
{
public:
    proEQ()
    {

    }

    ~proEQ()
    {

    }

    void parameterChanged(const juce::String& parameterID, float newValue)
    {

    }

    void init(juce::AudioProcessorValueTreeState *apvts) override
    {
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        proEq_Unit.channel = 2;
        proEq_Unit.ct = nullptr;
        proEq_Unit.enable = true;
        proEq_Unit.sample_rate = sampleRate;
        AudioEffectproEqInit(&proEq_Unit,2,sampleRate);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        size_t numSamples = buffer.getNumSamples();

        auto* pcm_L = buffer.getWritePointer (0);
        auto* pcm_R = buffer.getWritePointer (1);

        double *pcm = new double[2 * numSamples];
        for (size_t i = 0; i < numSamples; ++i)
        {
            pcm[2 * i + 1 ]=pcm_L[i];
            pcm[2 * i + 0 ]=pcm_R[i];
        }

        AudioEffectproEqApply(&proEq_Unit,pcm,pcm,(int)numSamples);

        for (size_t i = 0; i < numSamples; ++i)
        {
            pcm_L[i] = pcm[2 * i + 1 ];
            pcm_R[i] = pcm[2 * i + 0 ];
        }

        delete[] pcm;
    }

    void reset() override
    {

    }


    const juce::String getName() const override { return "Comb Filter"; }

private:

    proEqUnit proEq_Unit;
    juce::AudioProcessorValueTreeState* Apvts;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (proEQ)
};



#endif //PROEQ_H
