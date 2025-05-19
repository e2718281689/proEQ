#pragma once

#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

using SliderVts = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxVts = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

//==============================================================================
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::StringArray FilterModitems{"peak","lowShelf","lowPass","highShelf","highPass","bandShelf","tiltShelf","notch","bandPass"};
    juce::StringArray FilterDbOctitems{"6db/oct","12db/oct","24db/oct","36db/oct","48db/oct","60db/oct","72db/oct","84db/oct","96db/oct"};
    juce::StringArray filterIndexitems{"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"};

    juce::Slider CombFcSlider;
    juce::Slider CombGainSlider;

    juce::Slider FilterFcSlider;
    juce::Slider FilterGainSlider;
    juce::Slider FilterQSlider;

    juce::ComboBox FilterComboBox;
    juce::ComboBox FilterDbOctComboBox;
    juce::ComboBox filterIndexComboBox;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PluginProcessor& processorRef;
    std::unique_ptr<melatonin::Inspector> inspector;

    SliderVts sliderAttachment{ processorRef.apvts, "CombFcSlider", CombFcSlider };
    SliderVts slider1Attachment{ processorRef.apvts, "CombGainSlider", CombGainSlider };

    SliderVts FilterFcSliderAttachment{ processorRef.apvts, "FilterFcSlider", FilterFcSlider };
    SliderVts FilterGainSliderAttachment{ processorRef.apvts, "FilterGainSlider", FilterGainSlider };
    SliderVts FilterQSliderAttachment{ processorRef.apvts, "FilterQSlider", FilterQSlider };

    ComboBoxVts filterComboBoxAttachment{ processorRef.apvts, "FilterComboBox", FilterComboBox };
    ComboBoxVts filterDbOctComboBoxAttachment{ processorRef.apvts, "FilterDbOctComboBox", FilterDbOctComboBox };
    ComboBoxVts filterIndexComboBoxAttachment{ processorRef.apvts, "filterIndexComboBox", filterIndexComboBox };

    juce::TextButton inspectButton { "Inspect the UI" };
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
