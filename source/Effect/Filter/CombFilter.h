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
        delayLine.reset();

    }

    ~CombFilter()
    {


    }

    void parameterChanged(const juce::String& parameterID, float newValue)
    {

    }

    void init(juce::AudioProcessorValueTreeState *apvts) override
    {
        Apvts = apvts;
    }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override
    {
        juce::dsp::ProcessSpec Spec;
        Spec.numChannels = 2;
        Spec.sampleRate = sampleRate;
        Spec.maximumBlockSize = samplesPerBlock;

        delayLine.reset();
        delayLine.setMaximumDelayInSamples(44100);
        delayLine.prepare(Spec);

        delayLine.setDelay (441);
    }

    void processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) override
    {
        float* pcm_L = buffer.getWritePointer (0);
        float* pcm_R = buffer.getWritePointer (1);

        float* stere[2]={pcm_L,pcm_R};

        size_t numSamples = buffer.getNumSamples();

        for (size_t ch = 0; ch < 2; ch++)
        {
            for(size_t i = 0; i < numSamples; i++)
            {
                stere[ch][i] = stere[ch][i] + 0.5 * delayLine.popSample(ch);
                delayLine.pushSample(ch,stere[ch][i]);

            }
        }
    }

    void reset() override
    {
            delayLine.reset();
    }


    const juce::String getName() const override { return "Comb Filter"; }

private:

    juce::dsp::DelayLine<float> delayLine;

    juce::AudioProcessorValueTreeState* Apvts;
};



#endif //COMBFILTER_H
