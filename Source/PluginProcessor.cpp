#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters/ParameterIDs.h"

PetrichorAudioProcessor::PetrichorAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Main", juce::AudioChannelSet::stereo(), true)
                         .withOutput("DEW", juce::AudioChannelSet::stereo(), false)
                         .withOutput("BLOOM", juce::AudioChannelSet::stereo(), false)
                         .withOutput("BREEZE", juce::AudioChannelSet::stereo(), false)
                         .withOutput("ROOTS", juce::AudioChannelSet::stereo(), false)
                         .withOutput("BURST", juce::AudioChannelSet::stereo(), false)
                         .withInput("Sidechain", juce::AudioChannelSet::stereo(), false)),
      apvts(*this, nullptr, "Parameters", ParameterLayout::createParameterLayout())
{
    initializePatternState();
    initializeChordState();
}

PetrichorAudioProcessor::~PetrichorAudioProcessor()
{
}

const juce::String PetrichorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PetrichorAudioProcessor::acceptsMidi() const
{
    return true;
}

bool PetrichorAudioProcessor::producesMidi() const
{
    return false;
}

bool PetrichorAudioProcessor::isMidiEffect() const
{
    return false;
}

double PetrichorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PetrichorAudioProcessor::getNumPrograms()
{
    return 1;
}

int PetrichorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PetrichorAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String PetrichorAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void PetrichorAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void PetrichorAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    dewEngine.prepare(sampleRate);
    bloomEngine.prepare(sampleRate);
    breezeEngine.prepare(sampleRate);
    rootsEngine.prepare(sampleRate);
    burstEngine.prepare(sampleRate);

    greenhouse.prepare(sampleRate, samplesPerBlock);
    mistFilter.prepare(sampleRate);
    pocketDucker.prepare(sampleRate);

    dewBuffer.setSize(2, samplesPerBlock);
    bloomBuffer.setSize(2, samplesPerBlock);
    breezeBuffer.setSize(2, samplesPerBlock);
    rootsBuffer.setSize(2, samplesPerBlock);
    burstBuffer.setSize(2, samplesPerBlock);
    reverbBuffer.setSize(2, samplesPerBlock);

    dewLevelSmooth.reset(sampleRate, 0.02);
    bloomLevelSmooth.reset(sampleRate, 0.02);
    breezeLevelSmooth.reset(sampleRate, 0.02);
    rootsLevelSmooth.reset(sampleRate, 0.02);
    burstLevelSmooth.reset(sampleRate, 0.02);
    masterGainSmooth.reset(sampleRate, 0.02);
}

void PetrichorAudioProcessor::releaseResources()
{
}

bool PetrichorAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    for (int i = 1; i <= 5; ++i)
    {
        auto auxBus = layouts.getChannelSet(false, i);
        if (!auxBus.isDisabled() && auxBus != juce::AudioChannelSet::stereo())
            return false;
    }

    auto sidechainBus = layouts.getChannelSet(true, 0);
    if (!sidechainBus.isDisabled() && sidechainBus != juce::AudioChannelSet::stereo())
        return false;

    return true;
}

void PetrichorAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    hostClock.update(getPlayHead(), getSampleRate(), buffer.getNumSamples());

    int rootNote = static_cast<int>(*apvts.getRawParameterValue(ParameterIDs::root));
    int scaleType = static_cast<int>(*apvts.getRawParameterValue(ParameterIDs::scale));

    quantizer.setRoot(rootNote);
    quantizer.setScale(static_cast<ScaleQuantizer::Scale>(scaleType));

    for (const auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn() && msg.getNoteNumber() == 12)
        {
            burstEngine.trigger();
        }
    }

    float growth = *apvts.getRawParameterValue(ParameterIDs::growth);
    float weatherStorm = *apvts.getRawParameterValue(ParameterIDs::weatherStorm);
    float weatherHumid = *apvts.getRawParameterValue(ParameterIDs::weatherHumid);

    auto gains = GrowthMapper::calculate(growth, weatherStorm, weatherHumid);

    if (growth >= 66.0f && !lastGrowthWasAbove66)
    {
        burstEngine.trigger();
    }
    lastGrowthWasAbove66 = (growth >= 66.0f);

    double currentBar = hostClock.getCurrentBar();
    if (std::floor(currentBar) > std::floor(lastBarPosition))
    {
        burstBarCounter++;
        float burstEvery = *apvts.getRawParameterValue(ParameterIDs::burstEvery);
        float burstProb = *apvts.getRawParameterValue(ParameterIDs::burstProb) + weatherStorm * 0.4f;

        if (burstBarCounter >= static_cast<int>(burstEvery) && gains.burstArmed)
        {
            if ((std::rand() % 100) < static_cast<int>(burstProb))
            {
                burstEngine.trigger();
            }
            burstBarCounter = 0;
        }
    }
    lastBarPosition = currentBar;

    dewBuffer.clear();
    bloomBuffer.clear();
    breezeBuffer.clear();
    rootsBuffer.clear();
    burstBuffer.clear();
    reverbBuffer.clear();

    bool dewEnable = *apvts.getRawParameterValue(ParameterIDs::dewEnable) > 0.5f;
    if (dewEnable)
    {
        int dewOctave = static_cast<int>(*apvts.getRawParameterValue(ParameterIDs::dewOctave));
        int dewRate = static_cast<int>(*apvts.getRawParameterValue(ParameterIDs::dewRate));

        dewEngine.process(dewBuffer, hostClock, quantizer,
                         dewOctave,
                         static_cast<float>(dewRate),
                         *apvts.getRawParameterValue(ParameterIDs::dewGate),
                         *apvts.getRawParameterValue(ParameterIDs::dewRelease),
                         *apvts.getRawParameterValue(ParameterIDs::dewPurity),
                         *apvts.getRawParameterValue(ParameterIDs::dewMaterial),
                         *apvts.getRawParameterValue(ParameterIDs::dewProb),
                         *apvts.getRawParameterValue(ParameterIDs::dewHumanize),
                         *apvts.getRawParameterValue(ParameterIDs::dewSpread));
    }

    bool bloomEnable = *apvts.getRawParameterValue(ParameterIDs::bloomEnable) > 0.5f;
    if (bloomEnable)
    {
        int bloomBars = static_cast<int>(*apvts.getRawParameterValue(ParameterIDs::bloomBars));
        int barsValue = (bloomBars == 0) ? 1 : (bloomBars == 1) ? 2 : 4;

        bloomEngine.process(bloomBuffer, hostClock, quantizer,
                           barsValue,
                           *apvts.getRawParameterValue(ParameterIDs::bloomAttack),
                           *apvts.getRawParameterValue(ParameterIDs::bloomBright),
                           *apvts.getRawParameterValue(ParameterIDs::bloomWidth),
                           *apvts.getRawParameterValue(ParameterIDs::bloomDrift));
    }

    bool breezeEnable = *apvts.getRawParameterValue(ParameterIDs::breezeEnable) > 0.5f;
    if (breezeEnable)
    {
        breezeEngine.process(breezeBuffer, hostClock,
                            *apvts.getRawParameterValue(ParameterIDs::breezeCenter),
                            *apvts.getRawParameterValue(ParameterIDs::breezeWidth),
                            *apvts.getRawParameterValue(ParameterIDs::breezeSwell),
                            *apvts.getRawParameterValue(ParameterIDs::breezeDepth),
                            *apvts.getRawParameterValue(ParameterIDs::breezeCut));
    }

    bool rootsEnable = *apvts.getRawParameterValue(ParameterIDs::rootsEnable) > 0.5f;
    if (rootsEnable)
    {
        rootsEngine.process(rootsBuffer,
                           *apvts.getRawParameterValue(ParameterIDs::rootsRate),
                           *apvts.getRawParameterValue(ParameterIDs::rootsDepth),
                           *apvts.getRawParameterValue(ParameterIDs::rootsMono));
    }

    bool burstEnable = *apvts.getRawParameterValue(ParameterIDs::burstEnable) > 0.5f;
    if (burstEnable)
    {
        burstEngine.process(burstBuffer,
                           *apvts.getRawParameterValue(ParameterIDs::burstSize),
                           *apvts.getRawParameterValue(ParameterIDs::burstTone));
    }

    float dewLevel = *apvts.getRawParameterValue(ParameterIDs::dewLevel);
    float bloomLevel = *apvts.getRawParameterValue(ParameterIDs::bloomLevel);
    float breezeLevel = *apvts.getRawParameterValue(ParameterIDs::breezeLevel);
    float rootsLevel = *apvts.getRawParameterValue(ParameterIDs::rootsLevel);
    float burstLevel = *apvts.getRawParameterValue(ParameterIDs::burstLevel);

    dewLevelSmooth.setTargetValue(juce::Decibels::decibelsToGain(dewLevel) * gains.dew);
    bloomLevelSmooth.setTargetValue(juce::Decibels::decibelsToGain(bloomLevel) * gains.bloom);
    breezeLevelSmooth.setTargetValue(juce::Decibels::decibelsToGain(breezeLevel) * gains.breeze);
    rootsLevelSmooth.setTargetValue(juce::Decibels::decibelsToGain(rootsLevel) * gains.roots);
    burstLevelSmooth.setTargetValue(juce::Decibels::decibelsToGain(burstLevel));

    juce::AudioBuffer<float> mainBuffer(buffer.getArrayOfWritePointers(), 2, buffer.getNumSamples());
    mainBuffer.clear();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float dewGain = dewLevelSmooth.getNextValue();
        float bloomGain = bloomLevelSmooth.getNextValue();
        float breezeGain = breezeLevelSmooth.getNextValue();
        float rootsGain = rootsLevelSmooth.getNextValue();
        float burstGain = burstLevelSmooth.getNextValue();

        for (int ch = 0; ch < 2; ++ch)
        {
            mainBuffer.addSample(ch, sample, dewBuffer.getSample(ch, sample) * dewGain);
            mainBuffer.addSample(ch, sample, bloomBuffer.getSample(ch, sample) * bloomGain);
            mainBuffer.addSample(ch, sample, breezeBuffer.getSample(ch, sample) * breezeGain);
            mainBuffer.addSample(ch, sample, rootsBuffer.getSample(ch, sample) * rootsGain);
            mainBuffer.addSample(ch, sample, burstBuffer.getSample(ch, sample) * burstGain);
        }
    }

    float ghMix = *apvts.getRawParameterValue(ParameterIDs::ghMix);
    float ghSize = *apvts.getRawParameterValue(ParameterIDs::ghSize);
    float ghDamp = *apvts.getRawParameterValue(ParameterIDs::ghDamp);
    float ghPredelay = *apvts.getRawParameterValue(ParameterIDs::ghPredelay);
    float ghLowcut = *apvts.getRawParameterValue(ParameterIDs::ghLowcut);

    greenhouse.setParameters(ghSize, ghDamp, ghLowcut, ghPredelay);

    juce::AudioBuffer<float> reverbInput(2, buffer.getNumSamples());
    reverbInput.clear();

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        for (int ch = 0; ch < 2; ++ch)
        {
            reverbInput.addSample(ch, sample, dewBuffer.getSample(ch, sample) * dewLevelSmooth.getCurrentValue() * 0.3f);
            reverbInput.addSample(ch, sample, bloomBuffer.getSample(ch, sample) * bloomLevelSmooth.getCurrentValue() * 0.5f);
            reverbInput.addSample(ch, sample, breezeBuffer.getSample(ch, sample) * breezeLevelSmooth.getCurrentValue() * 0.2f);
            reverbInput.addSample(ch, sample, burstBuffer.getSample(ch, sample) * burstLevelSmooth.getCurrentValue() * 0.4f);
        }
    }

    greenhouse.process(reverbInput, mainBuffer, ghMix);

    mistFilter.setCutoff(getSampleRate(), *apvts.getRawParameterValue(ParameterIDs::mistCutoff));
    mistFilter.process(mainBuffer);

    masterGainSmooth.setTargetValue(juce::Decibels::decibelsToGain(*apvts.getRawParameterValue(ParameterIDs::masterGain)));

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float gain = masterGainSmooth.getNextValue();
        for (int ch = 0; ch < 2; ++ch)
        {
            float s = mainBuffer.getSample(ch, sample) * gain;
            s = std::tanh(s);
            mainBuffer.setSample(ch, sample, s);
        }
    }

    if (getBusBuffer(buffer, false, 1).getNumChannels() == 2)
        getBusBuffer(buffer, false, 1).makeCopyOf(dewBuffer, true);
    if (getBusBuffer(buffer, false, 2).getNumChannels() == 2)
        getBusBuffer(buffer, false, 2).makeCopyOf(bloomBuffer, true);
    if (getBusBuffer(buffer, false, 3).getNumChannels() == 2)
        getBusBuffer(buffer, false, 3).makeCopyOf(breezeBuffer, true);
    if (getBusBuffer(buffer, false, 4).getNumChannels() == 2)
        getBusBuffer(buffer, false, 4).makeCopyOf(rootsBuffer, true);
    if (getBusBuffer(buffer, false, 5).getNumChannels() == 2)
        getBusBuffer(buffer, false, 5).makeCopyOf(burstBuffer, true);
}

bool PetrichorAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PetrichorAudioProcessor::createEditor()
{
    return new PetrichorAudioProcessorEditor(*this);
}

void PetrichorAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.appendChild(patternState.createCopy(), nullptr);
    state.appendChild(chordState.createCopy(), nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void PetrichorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            auto newState = juce::ValueTree::fromXml(*xmlState);
            apvts.replaceState(newState);

            auto pattern = newState.getChildWithName("DewPattern");
            if (pattern.isValid())
                patternState = pattern.createCopy();

            auto chords = newState.getChildWithName("BloomChords");
            if (chords.isValid())
                chordState = chords.createCopy();
        }
    }
}

void PetrichorAudioProcessor::initializePatternState()
{
    patternState.removeAllChildren(nullptr);
    for (int i = 0; i < 16; ++i)
    {
        int defaultValue = (i % 4 == 0 || i % 4 == 1 || i % 4 == 3) ? ((i % 4 == 0) ? 1 : 2) : 0;
        if (i == 4 || i == 11) defaultValue = 0;
        else if (i % 2 == 0) defaultValue = 1;
        else defaultValue = 2;

        juce::ValueTree step("Step");
        step.setProperty("value", defaultValue, nullptr);
        patternState.appendChild(step, nullptr);
    }
}

void PetrichorAudioProcessor::initializeChordState()
{
    chordState.removeAllChildren(nullptr);

    juce::ValueTree chordA("ChordA");
    juce::ValueTree chordB("ChordB");

    std::array<int, 6> defaultA = {0, 2, 3, 5, 7, 10};
    std::array<int, 6> defaultB = {3, 7, 10, 14, 17, 21};

    for (int note : defaultA)
    {
        juce::ValueTree noteTree("Note");
        noteTree.setProperty("value", note, nullptr);
        chordA.appendChild(noteTree, nullptr);
    }

    for (int note : defaultB)
    {
        juce::ValueTree noteTree("Note");
        noteTree.setProperty("value", note, nullptr);
        chordB.appendChild(noteTree, nullptr);
    }

    chordState.appendChild(chordA, nullptr);
    chordState.appendChild(chordB, nullptr);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PetrichorAudioProcessor();
}
