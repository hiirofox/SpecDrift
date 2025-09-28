/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
LModelAudioProcessor::LModelAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
	)
#endif
{

}


juce::AudioProcessorValueTreeState::ParameterLayout LModelAudioProcessor::createParameterLayout()
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;
	layout.add(std::make_unique<juce::AudioParameterFloat>("lt", "lt", 0, 1, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("rt", "rt", 0, 1, 0.5));
	layout.add(std::make_unique<juce::AudioParameterFloat>("fb", "fb", -1, 1, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("pow", "pow", -8, 8, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("bbr", "bbr", -1, 1, 0));
	layout.add(std::make_unique<juce::AudioParameterFloat>("dry", "dry", 0, 0.5, 0));
	return layout;
}

LModelAudioProcessor::~LModelAudioProcessor()
{
}

//==============================================================================
const juce::String LModelAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LModelAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool LModelAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double LModelAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LModelAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LModelAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LModelAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LModelAudioProcessor::getProgramName(int index)
{
	return "SpecDrift";
}

void LModelAudioProcessor::changeProgramName(int index, const juce::String& newName)
{

}

//==============================================================================
void LModelAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
}

void LModelAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LModelAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
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


void LModelAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	int isMidiUpdata = 0;
	juce::MidiMessage MidiMsg;//先处理midi事件
	int MidiTime;
	juce::MidiBuffer::Iterator MidiBuf(midiMessages);
	while (MidiBuf.getNextEvent(MidiMsg, MidiTime))
	{
		if (MidiMsg.isNoteOn())
		{
			int note = MidiMsg.getNoteNumber() - 24;
		}
		if (MidiMsg.isNoteOff())
		{
			int note = MidiMsg.getNoteNumber() - 24;
		}
	}
	midiMessages.clear();

	const int numSamples = buffer.getNumSamples();
	float* wavbufl = buffer.getWritePointer(0);
	float* wavbufr = buffer.getWritePointer(1);
	const float* recbufl = buffer.getReadPointer(0);
	const float* recbufr = buffer.getReadPointer(1);

	float SampleRate = getSampleRate();

	float lt = *Params.getRawParameterValue("lt");
	float rt = *Params.getRawParameterValue("rt");
	float fb = *Params.getRawParameterValue("fb");
	float pw = *Params.getRawParameterValue("pow");
	float bbr = *Params.getRawParameterValue("bbr");
	float dry = *Params.getRawParameterValue("dry");


	lt = (expf(lt * 8.0) - 1.0) / (expf(8.0) - 1.0);
	rt = (expf(rt * 8.0) - 1.0) / (expf(8.0) - 1.0);

	float bbrsign = bbr > 0 ? 1 : -1;
	bbr *= bbrsign;
	bbr = (expf(bbr * 8.0) - 1.0) / (expf(8.0) - 1.0);
	bbr *= bbrsign;

	if (fb < 0)dry = -dry;

	stftl.SetDelay(lt, rt, 0, fb, pw, bbr, dry);
	stftr.SetDelay(lt, rt, 0, fb, pw, bbr, dry);

	stftl.ProcessBlock(recbufl, wavbufl, numSamples);
	stftr.ProcessBlock(recbufr, wavbufr, numSamples);

	swf.ProcessBlockIn(wavbufl, numSamples);
}

//==============================================================================
bool LModelAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LModelAudioProcessor::createEditor()
{
	return new LModelAudioProcessorEditor(*this);

	//return new juce::GenericAudioProcessorEditor(*this);//自动绘制(调试)
}

//==============================================================================
void LModelAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// 创建一个 XML 节点
	juce::XmlElement xml("LMEQ_Settings");

	// 保存包络数据 - 使用XML方式（推荐，更安全）
	auto envelopeXml = enveFunc.saveToXml();
	if (envelopeXml) {
		xml.addChildElement(envelopeXml.release());
	}

	// 或者使用二进制方式保存包络数据（更紧凑）
	/*
	juce::MemoryBlock envelopeDataBlock;
	enveFunc.saveToBinary(envelopeDataBlock);
	if (envelopeDataBlock.getSize() > 0) {
		juce::String base64EnvelopeData = envelopeDataBlock.toBase64Encoding();
		xml.setAttribute("ENVELOPE_DATA", base64EnvelopeData);
	}
	*/

	// 保存其他数据
	/*juce::MemoryBlock eqDataBlock;
	eqDataBlock.append(&manager, sizeof(ResonatorManager));
	juce::String base64Data = eqDataBlock.toBase64Encoding();
	xml.setAttribute("VIB_MANAGER", base64Data);*/

	auto state = Params.copyState();
	xml.setAttribute("Knob_Data", state.toXmlString());

	// 转换为字符串并保存
	juce::String xmlString = xml.toString();
	destData.append(xmlString.toRawUTF8(), xmlString.getNumBytesAsUTF8());
}

void LModelAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// 将 data 转换为字符串以解析 XML
	juce::String xmlString(static_cast<const char*>(data), sizeInBytes);

	// 解析 XML
	std::unique_ptr<juce::XmlElement> xml(juce::XmlDocument::parse(xmlString));
	if (xml == nullptr || !xml->hasTagName("LMEQ_Settings"))
	{
		DBG("Error: Unable to load XML settings");
		return;
	}

	// 加载包络数据 - 使用XML方式
	auto* envelopeXml = xml->getChildByName("EnvelopeData");
	if (envelopeXml) {
		bool success = enveFunc.loadFromXml(envelopeXml);
		if (!success) {
			DBG("Warning: Failed to load envelope data, using default");
			enveFunc.resetToDefault();
		}
	}
	else {
		// 如果没有找到包络数据，重置为默认值
		enveFunc.resetToDefault();
	}

	// 或者使用二进制方式加载包络数据
	/*
	juce::String base64EnvelopeData = xml->getStringAttribute("ENVELOPE_DATA");
	if (base64EnvelopeData.isNotEmpty()) {
		juce::MemoryBlock envelopeDataBlock;
		if (envelopeDataBlock.fromBase64Encoding(base64EnvelopeData)) {
			bool success = enveFunc.loadFromBinary(envelopeDataBlock);
			if (!success) {
				DBG("Warning: Failed to load envelope data, using default");
				enveFunc.resetToDefault();
			}
		}
	} else {
		enveFunc.resetToDefault();
	}
	*/

	// 加载其他数据
	/*
	juce::String base64Data = xml->getStringAttribute("VIB_MANAGER");
	juce::MemoryBlock eqDataBlock;
	eqDataBlock.fromBase64Encoding(base64Data);
	if (eqDataBlock.getData() != NULL)
	{
		std::memcpy(&manager, eqDataBlock.getData(), sizeof(ResonatorManager));
	}
	*/

	auto KnobDataXML = xml->getStringAttribute("Knob_Data");
	if (KnobDataXML.isNotEmpty()) {
		Params.replaceState(juce::ValueTree::fromXml(KnobDataXML));
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LModelAudioProcessor();
}
