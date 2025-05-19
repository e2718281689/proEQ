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


        FilterDbOctitems.set(0,6);
        FilterDbOctitems.set(1,12);
        FilterDbOctitems.set(2,24);
        FilterDbOctitems.set(3,36);
        FilterDbOctitems.set(4,48);
        FilterDbOctitems.set(5,60);
        FilterDbOctitems.set(6,72);
        FilterDbOctitems.set(7,84);
        FilterDbOctitems.set(8,96);






    }

    ~proEQ()
    {
        Apvts->removeParameterListener("FilterComboBox", this);
        Apvts->removeParameterListener("FilterDbOctComboBox", this);
        Apvts->removeParameterListener("FilterFcSlider", this);
        Apvts->removeParameterListener("FilterGainSlider", this);
        Apvts->removeParameterListener("FilterQSlider", this);
        Apvts->removeParameterListener("filterIndexComboBox", this);
    }

    void parameterChanged(const juce::String& parameterID, float newValue)
    {
        if (parameterID.equalsIgnoreCase("FilterComboBox"))
        {
            juce::Logger::outputDebugString("FilterComboBox =" + FilterModitems[(size_t)newValue]);
            // juce::Logger::outputDebugString("FilterComboBox =" + juce::String((size_t)newValue));
            proEq_Unit.filter[filter_index].filter_mod = newValue;
        }
        if (parameterID.equalsIgnoreCase("FilterDbOctComboBox"))
        {
            juce::Logger::outputDebugString("FilterDbOctComboBox =" + juce::String(FilterDbOctitems[(size_t)newValue]));
            proEq_Unit.filter[filter_index].filter_dboct = FilterDbOctitems[(size_t)newValue];
        }
        if (parameterID.equalsIgnoreCase("FilterFcSlider"))
        {
            juce::Logger::outputDebugString("FilterFcSlider =" + juce::String(newValue));
            proEq_Unit.filter[filter_index].filter_f = newValue;
        }
        if (parameterID.equalsIgnoreCase("FilterGainSlider"))
        {
            juce::Logger::outputDebugString("FilterGainSlider =" + juce::String(newValue));
            proEq_Unit.filter[filter_index].filter_gain = newValue;
        }
        if (parameterID.equalsIgnoreCase("FilterQSlider"))
        {
            juce::Logger::outputDebugString("FilterQSlider =" + juce::String(newValue));
            proEq_Unit.filter[filter_index].filter_q = newValue;
        }

        if (parameterID.equalsIgnoreCase("filterIndexComboBox"))
        {
            juce::Logger::outputDebugString("filterIndexComboBox =" + juce::String(newValue));
            filter_index = newValue;
        }

        AudioEffectproEqConfigure(&proEq_Unit,filter_index);


    }

    void init(juce::AudioProcessorValueTreeState *apvts) override
    {
        Apvts = apvts;
        Apvts->addParameterListener("FilterComboBox", this);
        Apvts->addParameterListener("FilterDbOctComboBox", this);
        Apvts->addParameterListener("FilterFcSlider", this);
        Apvts->addParameterListener("FilterGainSlider", this);
        Apvts->addParameterListener("FilterQSlider", this);
        Apvts->addParameterListener("filterIndexComboBox", this);


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

    int filter_index = 0;
    juce::StringArray FilterModitems{"peak","lowShelf","lowPass","highShelf","highPass"};
    juce::HashMap<int, int> FilterDbOctitems;
    
    proEqUnit proEq_Unit= {};
    juce::AudioProcessorValueTreeState* Apvts;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (proEQ)
};



#endif //PROEQ_H
