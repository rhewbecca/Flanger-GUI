/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FlangerAudioProcessor::FlangerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // Read and Write pointers initialized
    delayBufferRead = 0;
    delayBufferWrite = 0;

    // Default values
    delayDefault = 0.0025;
    wetDefault = 1;
    fbDefault = 0.0;
    sweepDefault = 0.010;
    gDefault = 1.0;
    speedDefault = 0.2;
    waveDefault = kSineWave;
    interpolDefault = kLinear;
    stereo = 0;


    wave = 0;
    interpol = 0;

}


FlangerAudioProcessor::~FlangerAudioProcessor()
{
}

//==============================================================================
const juce::String FlangerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int FlangerAudioProcessor::getNumParameters()
{
    return kNumParameters;
}

float FlangerAudioProcessor::getParameter(int index)
{
    switch (index)
    {
    case kDelayParam: return delay;
    case kSweepParam: return sweep;
    case kDepthParam: return g;
    case kWetParam: return wet;
    case kWaveParam: return wave;
    case kInterpolParam: return interpol;
    case kFbParam: return fb;
    case kFrequencyParam: return speed;
    case kStereoParam: return stereo;
    default:return 0.0f;
    }
}

// Setters functions implementation: (DA COLLEGARE CON EDITOR)
void FlangerAudioProcessor::setParameter(int index, float newValue) {
    switch (index)
    {
    case kDelayParam:
        delay = newValue;
        break;
    case kSweepParam:
        sweep = newValue;
        break;
    case kDepthParam:
        g = newValue;
        break;
    case kFbParam:
        fb = newValue;
        break;
    case kFrequencyParam:
        speed = newValue;
        break;
    case kWaveParam:
        wave = (int)newValue;
        break;
    case kInterpolParam:
        interpol = (int)newValue;
        break;
    case kStereoParam:
        stereo = (int)newValue;
        break;
    case kWetParam:
        wet = (int)newValue;
    default:
        break;
    }
}


const juce::String FlangerAudioProcessor::getParameterName(int index) {
    switch (index)
    {
    case kDelayParam: return "delay";
    case kSweepParam: return "sweep width";
    case kDepthParam: return "depth";
    case kFbParam: return "feedback";
    case kFrequencyParam: return "frequency";
    case kWaveParam: return "waveform";
    case kInterpolParam: return "interpolation";
    case kStereoParam: return "stereo";
    case kWetParam: return "wet";
    default: break;
    }

    return juce::String();
}

const juce::String FlangerAudioProcessor::getParameterText(int index) {
    return juce::String(getParameter(index), 2);
}

const juce::String FlangerAudioProcessor::getInputChannelName(int channelIndex) const {
    return juce::String(channelIndex + 1);
}

const juce::String FlangerAudioProcessor::getOutputChannelName(int channelIndex) const {
    return juce::String(channelIndex + 1);
}

bool FlangerAudioProcessor::isInputChannelStereoPair(int index) const
{
    return true;
}

bool FlangerAudioProcessor::isOutputChannelStereoPair(int index) const
{
    return true;
}


bool FlangerAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool FlangerAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool FlangerAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double FlangerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FlangerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FlangerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FlangerAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String FlangerAudioProcessor::getProgramName(int index)
{
    return {};
}

void FlangerAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void FlangerAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback initialisation that you need..

    // 10
    // Retrieve the delay buffer length from the sample rate
    delayBufferLength = (int)(2 * sampleRate);
    // non-zero length check
    if (delayBufferLength < 1) {
        delayBufferLength = 1;
    }
    // Allocate and initialize the delay buffer
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();

    lfoPhase = 0.0;
    inverseSampleRate = 1.0 / 44100.0;
}

void FlangerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FlangerAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
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
#endif

// LFO DA IMPLEMENTARE
float FlangerAudioProcessor::lfo(int ph, int waveform) {
    switch (waveform)
    {
    case kTrWave:
        if (ph < 0.25f)
            return 0.5f + 2.0f * ph;
        else if (ph < 0.75f)
            return 1.0f - 2.0f * (ph - 0.25f);
        else
            return 2.0f * (ph - 0.75f);
    case kSqWave:
        if (ph < 0.5f)
            return 1.0f;
        else
            return 0.0f;
    case kSawWave:
        if (ph < 0.5f)
            return 0.5f + ph;
        else
            return ph - 0.5f;
    case kSineWave:
    default:
        return 0.5f + 0.5f * sinf(2.0 * 3.14 * ph);
    }
}

void FlangerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    // Helpful information about this block of samples:

    const int numInputChannels = getNumInputChannels();     // How many input channels for our effect?
    const int numOutputChannels = getNumOutputChannels();   // How many output channels for our effect?
    const int numSamples = buffer.getNumSamples();          // How many samples in the buffer for this block?

    int channel, dpw; // dpr = delay read pointer; dpw = delay write pointer
    float dpr, currentDelay, ph;
    float channel0EndPhase = lfoPhase;

    // Go through each channel of audio that's passed in. In this example we apply identical
    // effects to each channel, regardless of how many input channels there are. For some effects, like
    // a stereo chorus or panner, you might do something different for each channel.

    for (channel = 0; channel < numInputChannels; ++channel)
    {
        // channelData is an array of length numSamples which contains the audio for one channel
        float* channelData = buffer.getWritePointer(channel);

        // delayData is the circular buffer for implementing delay on this channel
        float* delayData = delayBuffer.getWritePointer(juce::jmin(channel, delayBuffer.getNumChannels() - 1));

        // Make a temporary copy of any state variables declared in PluginProcessor.h which need to be
        // maintained between calls to processBlock(). Each channel needs to be processed identically
        // which means that the activity of processing one channel can't affect the state variable for
        // the next channel.

        dpw = delayBufferWrite;
        ph = lfoPhase;

        // For stereo flanging, keep the channels 90 degrees out of phase with each other
        if (stereo != 0 && channel != 0)
            ph = fmodf(ph + 0.25, 1.0);

        for (int i = 0; i < numSamples; ++i) {

            const float in = channelData[i];
            float interpolatedSample = 0.0;

            // Recalculate the read pointer position with respect to the write pointer. A more efficient
            // implementation might increment the read pointer based on the derivative of the LFO without
            // running the whole equation again, but this format makes the operation clearer.

            //FUNZIONE LFO DA IMPLEMENTARE, wave parametro della funzione (mancante)
            currentDelay = delay + sweep * lfo(ph, wave);
            dpr = fmodf((float)dpw - (float)(currentDelay * getSampleRate()) + (float)delayBufferLength,
                (float)delayBufferLength);

            // In this example, the output is the input plus the contents of the delay buffer (weighted by delayMix)
            // The last term implements a tremolo (variable amplitude) on the whole thing.

            if (interpol == kLinear) {

                // Find the fraction by which the read pointer sits between two
                // samples and use this to adjust weights of the samples
                float fraction = dpr - floorf(dpr);
                int previousSample = (int)floorf(dpr);
                int nextSample = (previousSample + 1) % delayBufferLength;
                interpolatedSample = fraction * delayData[nextSample]
                    + (1.0f - fraction) * delayData[previousSample];
            }
            else if (interpol == kQuadratic) {
                int sample1 = (int)floorf(dpr);
                int sample2 = (sample1 + 1) % delayBufferLength;
                int sample0 = (sample1 - 1 + delayBufferLength) % delayBufferLength;

                float fraction = dpr - floorf(dpr);
                float a0 = 0.5f * (delayData[sample0] - delayData[sample2]);
                float a1 = 1 / (delayData[sample0] - 2.0f * delayData[sample1] + delayData[sample2]);
                float a2 = a0 * a1;

                interpolatedSample = delayData[sample1] - 0.25f * fraction * a2 * (delayData[sample0] - delayData[sample2]);
            }
            else if (interpol == kCubic) {

                // Cubic interpolation will produce cleaner results at the expense
                // of more computation. This code uses the Catmull-Rom variant of
                // cubic interpolation. To reduce the load, calculate a few quantities
                // in advance that will be used several times in the equation:

                int sample1 = (int)floorf(dpr);
                int sample2 = (sample1 + 1) % delayBufferLength;
                int sample3 = (sample2 + 1) % delayBufferLength;
                int sample0 = (sample1 - 1 + delayBufferLength) % delayBufferLength;

                float fraction = dpr - floorf(dpr);
                float frsq = fraction * fraction;

                float a0 = -0.5f * delayData[sample0] + 1.5f * delayData[sample1]
                    - 1.5f * delayData[sample2] + 0.5f * delayData[sample3];
                float a1 = delayData[sample0] - 2.5f * delayData[sample1]
                    + 2.0f * delayData[sample2] - 0.5f * delayData[sample3];
                float a2 = -0.5f * delayData[sample0] + 0.5f * delayData[sample2];
                float a3 = delayData[sample1];

                interpolatedSample = a0 * fraction * frsq + a1 * frsq + a2 * fraction + a3;

            }


            // Store the current information in the delay buffer. With feedback, what we read is
            // included in what gets stored in the buffer, otherwise it's just a simple delay line
            // of the input signal.

            delayData[dpw] = in + (interpolatedSample * fb);

            // Increment the write pointer at a constant rate. The read pointer will move at different
            // rates depending on the settings of the LFO, the delay and the sweep width.

            if (++dpw >= delayBufferLength)
                dpw = 0;

            // Store the output sample in the buffer, replacing the input
            channelData[i] = in + gDefault * interpolatedSample;

            // Update the LFO phase, keeping it in the range 0-1
            ph += speed * inverseSampleRate;

            if (ph >= 1.0)
                ph -= 1.0;
        }

        // Use channel 0 only to keep the phase in sync between calls to processBlock()
        // Otherwise quadrature phase on multiple channels will create problems.
        if (channel == 0)
            channel0EndPhase = ph;
    }

    // Having made a local copy of the state variables for each channel, now transfer the result
    // back to the main state variable so they will be preserved for the next call of processBlock()

    delayBufferWrite = dpw;
    lfoPhase = channel0EndPhase;
}
//==============================================================================


//==============================================================================
bool FlangerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* FlangerAudioProcessor::createEditor()
{
    return new FlangerAudioProcessorEditor(*this);
}

//==============================================================================
void FlangerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void FlangerAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FlangerAudioProcessor();
}