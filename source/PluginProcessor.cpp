#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),apvts(*this,nullptr,"Parameters", CreateParameters())
{

}

PluginProcessor::~PluginProcessor()
{

}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    AudioChain.setPlayConfigDetails(getMainBusNumInputChannels(),
                                    getMainBusNumOutputChannels(),
                                            sampleRate,
                                            samplesPerBlock);

    AudioChain.prepareToPlay(sampleRate, samplesPerBlock);
    AudioChain.AudioGroupInit();

    AudioChain.addProcessorNode(std::make_unique < proEQ >());
    // AudioChain.addProcessorNode(std::make_unique < CombFilter >());

}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        juce::ignoreUnused (channelData);
        // ..do something to the data...
    }

    AudioChain.processBlock(buffer, midiMessages);



}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}
juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::CreateParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameterLayout;

    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
        "CombFcSlider",
        "CombFcSlider",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.01f),
        1000.0f));

    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
        "CombGainSlider",
        "CombGainSlider",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f),
        0.0f));

    parameterLayout.add(std::make_unique<juce::AudioParameterChoice>(
    "FilterComboBox",
    "FilterComboBox",
    juce::StringArray{"peak","lowShelf","lowPass","highShelf","highPass","bandShelf","tiltShelf","notch","bandPass"},
    1));

    parameterLayout.add(std::make_unique<juce::AudioParameterChoice>(
        "FilterDbOctComboBox",
        "FilterDbOctComboBox",
        juce::StringArray{"6", "12", "24", "36", "48", "60", "72", "84", "96"},
        1));

    parameterLayout.add(std::make_unique<juce::AudioParameterChoice>(
        "filterIndexComboBox",
        "filterIndexComboBox",
        juce::StringArray{"1","2","3","4","5","6","7","8","9","10","11","12","13","14","15","16"},
        1));

    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
    "FilterFcSlider",
    "FilterFcSlider",
    juce::NormalisableRange<float>(20.0f, 20*1000.0f, 0.0001f),
    1000.0f));

    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
    "FilterGainSlider",
    "FilterGainSlider",
    juce::NormalisableRange<float>(-30.0f, 30.0f, 0.0001f),
    0.0f));

    parameterLayout.add(std::make_unique<juce::AudioParameterFloat>(
    "FilterQSlider",
    "FilterQSlider",
    juce::NormalisableRange<float>(0.025f, 25.0f, 0.0001f),
    0.7070f));

    return parameterLayout;

}
//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
