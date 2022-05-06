/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor (FlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    // LFO Sweep (Amplitude)
    sweepSlider.setRange(0.0, 50.0);
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
    waveSelector.addItem("Sin",1);
    waveSelector.addItem("Saw",2);
    waveSelector.addItem("Tri",3);
    //waveSelector.onChange = [this] { waveSelectorChanged(); };
    waveSelector.setSelectedId(1);

    waveSelectorLabel.setText("LFO Type", juce::dontSendNotification);

    addAndMakeVisible(waveSelector);
    addAndMakeVisible(waveSelectorLabel);

    // Interpolation selector
    interpolSelector.addItem("Lin", 1);
    interpolSelector.addItem("Sqr", 2);
    interpolSelector.addItem("Cub", 3);
    //interpolSelector.onChange = [this] { interpolSelectorChanged(); };
    interpolSelector.setSelectedId(1);

    interpolSelectorLabel.setText("Interpolation", juce::dontSendNotification);

    addAndMakeVisible(interpolSelector);
    addAndMakeVisible(interpolSelectorLabel);

    // Window size
    setSize (800, 500);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
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

    interpolSelector.setBounds(700, 50, 100, 20);
    interpolSelectorLabel.setBounds(700, 20, 100, 20);
}

void FlangerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sweepSlider) { audioProcessor.set_sweep(sweepSlider.getValue()); }
    else if (slider == &speedSlider) { audioProcessor.set_speed(speedSlider.getValue()); }
}
