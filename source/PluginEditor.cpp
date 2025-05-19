#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    addAndMakeVisible (inspectButton);

    // this chunk of code instantiates and opens the melatonin inspector
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }

        inspector->setVisible (true);
    };


    CombFcSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    CombFcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(CombFcSlider);

    CombGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    CombGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(CombGainSlider);


    FilterFcSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    FilterFcSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(FilterFcSlider);

    FilterGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    FilterGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(FilterGainSlider);

    FilterQSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    FilterQSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(FilterQSlider);



    FilterComboBox.addItemList(FilterModitems, 1);
    FilterComboBox.setSelectedId(1);
    addAndMakeVisible(FilterComboBox);

    FilterDbOctComboBox.addItemList(FilterDbOctitems, 1);
    FilterDbOctComboBox.setSelectedId(1);
    addAndMakeVisible(FilterDbOctComboBox);

    filterIndexComboBox.addItemList(filterIndexitems, 1);
    filterIndexComboBox.setSelectedId(1);
    addAndMakeVisible(filterIndexComboBox);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    auto area = getLocalBounds();
    g.setColour (juce::Colours::white);
    g.setFont (16.0f);
    auto helloWorld = juce::String ("Hello from ") + PRODUCT_NAME_WITHOUT_VERSION + " v" VERSION + " running in " + CMAKE_BUILD_TYPE;
    g.drawText (helloWorld, area.removeFromTop (10), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    // layout the positions of your child components here
    auto area = getLocalBounds();
    area.removeFromBottom(50);
    // inspectButton.setBounds (20,20,50,25);
    // CombFcSlider.setBounds (50,100,100,100);
    // CombGainSlider.setBounds (250,100,100,100);

    FilterComboBox.setBounds (50,50,100,20);
    FilterDbOctComboBox.setBounds (50,80,100,20);
    filterIndexComboBox.setBounds (50,110,100,20);


    FilterFcSlider.setBounds(50,200,100,100);
    FilterGainSlider.setBounds(150,200,100,100);
    FilterQSlider.setBounds(250,200,100,100);
}
