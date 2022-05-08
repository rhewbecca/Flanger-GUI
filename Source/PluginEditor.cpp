/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const int GUI_WIDTH = 800;
const int GUI_HEIGHT = 600;
float scaleUI = 1.0f;  // this can be 0.5, 0.666 or 1.0

//==============================================================================
FlangerAudioProcessorEditor::FlangerAudioProcessorEditor (FlangerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    // LFO Sweep (Amplitude)
    sweepSlider.setRange(0.0, 50.0);
    //sweepSlider.setSliderStyle(juce::Slider::Rotary);
    sweepSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    sweepSlider.addListener(this);

    sweepLabel.setText("Sweep", juce::dontSendNotification);

    addAndMakeVisible(sweepLabel);
    addAndMakeVisible(sweepSlider);
    

    // LFO Speed (Frequency)
    speedSlider.setRange(0.0, 10.0);
    //speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    speedSlider.addListener(this);

    speedLabel.setText("Speed", juce::dontSendNotification);

    addAndMakeVisible(speedLabel);
    addAndMakeVisible(speedSlider);
    

    // LFO wave form selector
    waveSelector.addItem("Sine wave",1);
    waveSelector.addItem("Saw wave",2);
    waveSelector.addItem("Triangular Wave",3);
    waveSelector.addItem("Square Wave", 4);
    //waveSelector.onChange = [this] { waveSelectorChanged(); };
    waveSelector.setSelectedId(1);

    waveSelectorLabel.setText("LFO Type", juce::dontSendNotification);

    addAndMakeVisible(waveSelectorLabel);
    addAndMakeVisible(waveSelector);
    

    // Interpolation selector
    interpolSelector.addItem("Linear", 1);
    interpolSelector.addItem("Square", 2);
    interpolSelector.addItem("Cubic", 3);
    //interpolSelector.onChange = [this] { interpolSelectorChanged(); };
    interpolSelector.setSelectedId(1);

    interpolSelectorLabel.setText("Interpolation", juce::dontSendNotification);

    addAndMakeVisible(interpolSelector);
    addAndMakeVisible(interpolSelectorLabel);

    // Delay
    delaySlider.setRange(5.0, 25.0);
    //delaySlider.setSliderStyle(juce::Slider::Rotary);
    delaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    delaySlider.addListener(this);

    delayLabel.setText("Delay/Amount", juce::dontSendNotification);

    addAndMakeVisible(delayLabel);
    addAndMakeVisible(delaySlider);
    

    // Feedback gain
    // must be < 1
    fbSlider.setRange(0.0, 0.99);
    //fbSlider.setSliderStyle(juce::Slider::Rotary);
    fbSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
    fbSlider.addListener(this);
    fbLabel.setText("Feedback", juce::dontSendNotification);
    addAndMakeVisible(fbLabel);
    addAndMakeVisible(fbSlider);

    // Phase switch
    phaseSwitch.setButtonText("Invert phase");
    addAndMakeVisible(phaseSwitch);

    // WetDry Slider
    // wet = 0, dry = 1
    wetDrySlider.setRange (0.0, 1.0);
    wetDrySlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 100, 20);
    wetDrySlider.addListener(this);
    wetDryLabel.setText ("Wet/Dry", juce::dontSendNotification);
    addAndMakeVisible(wetDryLabel);
    addAndMakeVisible (wetDrySlider);
    

    // Window size
    // Resizable vertically and horizonally
    setResizable(true, true);
    // min width, min height, max width, max height
    setResizeLimits(GUI_WIDTH * scaleUI / 1.5, GUI_HEIGHT * scaleUI / 1.5, GUI_WIDTH * scaleUI * 3, GUI_HEIGHT * scaleUI * 3);
    setSize(GUI_WIDTH * scaleUI, GUI_HEIGHT * scaleUI);
}

FlangerAudioProcessorEditor::~FlangerAudioProcessorEditor()
{
}

//==============================================================================
void FlangerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
    // Slider colors
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::red);
    getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::pink);

    // Label colors
    getLookAndFeel().setColour(juce::Label::textColourId, juce::Colours::pink);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
}

void FlangerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    juce::FlexBox externalFlex;
    externalFlex.flexWrap = juce::FlexBox::Wrap::wrap;
    externalFlex.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
    externalFlex.flexDirection = juce::FlexBox::Direction::row;

        juce::FlexBox knobBox;
        knobBox.flexWrap = juce::FlexBox::Wrap::wrap;
        knobBox.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        knobBox.flexDirection = juce::FlexBox::Direction::column;
    
            juce::FlexBox sweepBox;
            sweepBox.flexDirection = juce::FlexBox::Direction::column;
            sweepBox.items.add(juce::FlexItem(sweepLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            sweepBox.items.add(juce::FlexItem (sweepSlider).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            

            juce::FlexBox speedBox;
            speedBox.flexDirection = juce::FlexBox::Direction::column;
            speedBox.items.add(juce::FlexItem(speedLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            speedBox.items.add(juce::FlexItem(speedSlider).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            

            juce::FlexBox delayBox;
            delayBox.flexDirection = juce::FlexBox::Direction::column;
            delayBox.items.add(juce::FlexItem(delayLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            delayBox.items.add(juce::FlexItem(delaySlider).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1));
            

            juce::FlexBox fbBox;
            fbBox.flexDirection = juce::FlexBox::Direction::column;
            fbBox.items.add(juce::FlexItem(fbLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1, 1));
            fbBox.items.add(juce::FlexItem(fbSlider).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1,1));
            

            juce::FlexBox wetDryBox;
            wetDryBox.flexDirection = juce::FlexBox::Direction::column;
            wetDryBox.items.add(juce::FlexItem(wetDryLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1, 1));
            wetDryBox.items.add(juce::FlexItem(wetDrySlider).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1,1));
            

        knobBox.items.add(juce::FlexItem(sweepBox).withFlex(2, 1));                                                 // [4]
        knobBox.items.add(juce::FlexItem(speedBox).withFlex(2, 1));
        knobBox.items.add(juce::FlexItem(delayBox).withFlex(2, 1));
        knobBox.items.add(juce::FlexItem(fbBox).withFlex(2, 1));
        knobBox.items.add(juce::FlexItem(wetDryBox).withFlex(3, 0));
        knobBox.performLayout(getLocalBounds().toFloat());

        juce::FlexBox sideBar;
        sideBar.flexDirection = juce::FlexBox::Direction::column;
        juce::FlexItem side(200, (float)getHeight(), sideBar);
            juce::FlexBox interpolBox;
            interpolBox.flexDirection = juce::FlexBox::Direction::column;
            interpolBox.items.add(juce::FlexItem(interpolSelectorLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(0,1));
            interpolBox.items.add(juce::FlexItem(interpolSelector).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(0,1));

            juce::FlexBox waveBox;
            waveBox.flexDirection = juce::FlexBox::Direction::column;
            waveBox.items.add(juce::FlexItem(waveSelectorLabel).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(0,1));
            waveBox.items.add(juce::FlexItem(waveSelector).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(0,1));

        sideBar.items.add(juce::FlexItem(interpolBox).withFlex(2.5).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1,1, 50.0f));                                                 // [4]
        sideBar.items.add(juce::FlexItem(waveBox).withFlex(2.5).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1, 1, 50.0f));
        sideBar.items.add(juce::FlexItem(phaseSwitch).withFlex(2.5).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(1, 1, 50.0f));

    externalFlex.items.add(juce::FlexItem(knobBox).withFlex(2.5).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(2, 1));
    externalFlex.items.add(juce::FlexItem(side).withFlex(2.5).withMinHeight(50.0f).withMinWidth(50.0f).withFlex(0,1,150.0f));
    externalFlex.performLayout(getLocalBounds().toFloat());
}

void FlangerAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &sweepSlider) { audioProcessor.set_sweep(sweepSlider.getValue()); }
    else if (slider == &speedSlider) { audioProcessor.set_speed(speedSlider.getValue()); }
    else if (slider == &delaySlider) { audioProcessor.set_delay(delaySlider.getValue()); }
    else if (slider == &fbSlider) { audioProcessor.set_fb(fbSlider.getValue()); }
    else if (slider == &wetDrySlider) { audioProcessor.set_wetDry(wetDrySlider.getValue()); }
}
