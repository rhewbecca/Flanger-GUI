/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor(FlangerAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{

    // LFO Sweep (Amplitude)
    sweepSlider.setRange(0.0, 1.0);
    sweepSlider.setSliderStyle(juce::Slider::Rotary);
    sweepSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    sweepSlider.addListener(this);

    sweepLabel.setText("Sweep", juce::dontSendNotification);

    addAndMakeVisible(sweepSlider);
    addAndMakeVisible(sweepLabel);

    // LFO Speed (Frequency)
    speedSlider.setRange(0.0, 10.0);
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    speedSlider.addListener(this);

    speedLabel.setText("Speed", juce::dontSendNotification);

    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel);

    // LFO wave form selector
    waveSelector.addItem("Sin", 1);
    waveSelector.addItem("Tri", 2);
    waveSelector.addItem("Sqr", 3);
    waveSelector.addItem("Saw", 4);
    waveSelector.onChange = [this] { audioProcessor.setParameter(5, waveSelector.getSelectedId()); };
    waveSelector.setSelectedId(1);

    waveSelectorLabel.setText("LFO Type", juce::dontSendNotification);

    addAndMakeVisible(waveSelector);
    addAndMakeVisible(waveSelectorLabel);

    // Interpolation selector
    interpolSelector.addItem("Lin", 1);
    interpolSelector.addItem("Sqr", 2);
    interpolSelector.addItem("Cub", 3);
    interpolSelector.onChange = [this] { audioProcessor.setParameter(6, interpolSelector.getSelectedId()); };
    interpolSelector.setSelectedId(1);

    interpolSelectorLabel.setText("Interpolation", juce::dontSendNotification);

    addAndMakeVisible(interpolSelector);
    addAndMakeVisible(interpolSelectorLabel);

    // Delay
    delaySlider.setRange(5.0, 25.0);
    delaySlider.setSliderStyle(juce::Slider::Rotary);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxAbove, false, 100, 20);
    delaySlider.addListener(this);

    delayLabel.setText("Delay/Amount", juce::dontSendNotification);

    addAndMakeVisible(delaySlider);
    addAndMakeVisible(delayLabel);

    // Feedback gain
    fbSlider.setRange(0.0, 0.99);
    fbSlider.setSliderStyle(juce::Slider::Rotary);
    fbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    fbSlider.addListener(this);

    fbLabel.setText("Feedback", juce::dontSendNotification);

    addAndMakeVisible(fbSlider);
    addAndMakeVisible(fbLabel);

    // Phase switch
    phaseSwitch.setButtonText("Invert phase");

    addAndMakeVisible(phaseSwitch);


    // WetDry Slider
    // wet = 0, dry = 1
    wetDrySlider.setRange(0.0, 1.0);
    wetDrySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 100, 20);
    wetDrySlider.addListener(this);
    wetDryLabel.setText("Wet/Dry", juce::dontSendNotification);

    addAndMakeVisible(wetDrySlider);
    addAndMakeVisible(wetDryLabel);

    // Window size
    setSize(800, 600);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Slider colors
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::pink);

    // Label colors
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::pink);

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
}

void FlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    sweepSlider.setBounds(200, 300, 100, 100);
    sweepLabel.setBounds(225, 400, 50, 20);

    speedSlider.setBounds(500, 300, 100, 100);
    speedLabel.setBounds(525, 400, 50, 20);

    waveSelector.setBounds(350, 300, 100, 20);
    waveSelectorLabel.setBounds(350, 320, 100, 20);

    interpolSelector.setBounds(680, 50, 100, 20);
    interpolSelectorLabel.setBounds(680, 20, 100, 20);

    delaySlider.setBounds(20, 50, 100, 100);
    delayLabel.setBounds(20, 20, 100, 20);

    fbSlider.setBounds(30, 200, 80, 80);
    fbLabel.setBounds(35, 280, 100, 20);

    phaseSwitch.setBounds(680, 200, 100, 20);

    wetDrySlider.setBounds(150, 450, 500, 80);
    wetDryLabel.setBounds(330, 480, 300, 80);
}

void FlangerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sweepSlider) { audioProcessor.setParameter(1, sweepSlider.getValue()); }
    else if (slider == &speedSlider) { audioProcessor.setParameter(4, speedSlider.getValue()); }
    else if (slider == &delaySlider) { audioProcessor.setParameter(0, delaySlider.getValue()); }
    else if (slider == &fbSlider) { audioProcessor.setParameter(3, fbSlider.getValue()); }
}
