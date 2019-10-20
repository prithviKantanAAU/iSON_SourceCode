/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BiQuad.h"
#include <chrono>
#include <thread>
#include <string>
#include <cmath>

//==============================================================================
RhythmSynthAudioProcessor::RhythmSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	walkingParamMeasurements = fopen("Walking Measurements.csv", "w");
	fprintf(walkingParamMeasurements, "Step Index,Foot,Step Time,Stance Time,Swing Time,Heel,M1,Toe,Heel+M1,M1+Toe,Heel+Toe,Heel+M1+Toe,Rollover Pattern\n");
	//fprintf(rolloverPatternMeasurements,"Stride Index,Rollover-L,Rollover-R\n");
	//fprintf(performanceParamMeasurements,"\n");
	accSamplesElapsed = 0;
	setAudioParamRanges();
	initializeMappingConfiguration();
	startTimer(5);
}

RhythmSynthAudioProcessor::~RhythmSynthAudioProcessor()
{
	dspFaust.stop();
	stopTimer();
}

void RhythmSynthAudioProcessor::stopFaust()
{
	dspFaust.stop();
}

//==============================================================================
const String RhythmSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

void RhythmSynthAudioProcessor::initializeMappingConfiguration()
{
	initializeMappingMatrix();

	for (int i = 0; i < 10; i++)
	{
		mappedParamIndex[i] = i;
		mappingTypes[i] = 3;
		mappingDirections[i] = 1;
		numQuantizations[i] = 10;
	}
	//mappingDirections[0] = 2;
	desired_GlobalParams[5] = 0.2;
	calcSlopeFactor(5);
	mappingDirections[5] = 2;
	numQuantizations[5] = 10;
	
}

void RhythmSynthAudioProcessor::setMappingConfiguration(int gaitParamIndex, int audioMapping)
{
	mappedParamIndex[gaitParamIndex] = audioMapping;
}

void RhythmSynthAudioProcessor::setStepFrequency(float freq)
{
	f = freq;
}

void RhythmSynthAudioProcessor::setStepSizeWindow(int window)
{
	gaitAnalysis.stepSizeWindow = window;
}

void RhythmSynthAudioProcessor::setAutoCadenceDetect(bool state)
{
	isAutoTempoEnabled = state;

}

void RhythmSynthAudioProcessor::setDesiredCadence(float percent, bool isBaselineSet)
{
	if (!isBaselineSet)
		return;
	else
	{
		desiredCadence = 60 / baseline_stepDuration_Mean * (1 + 0.01*percent);
		dspFaust.setParamValue("/PreTest/Set_Tempo", desiredCadence);
		calcDesiredParameter(desiredCadence, 0);
	}
}

void RhythmSynthAudioProcessor::setOnOff(bool state)
{
	if (state)
	{
		hasStarted = true;
		dspFaust.start();
		dspFaust.setParamValue("/PreTest/Zita_Light/Level", 5.8);
		dspFaust.setParamValue("/PreTest/Zita_Light/Dry/Wet_Mix", 0.49);
		dspFaust.setParamValue("/PreTest/Set_Tempo", 100);
	}	
}

void RhythmSynthAudioProcessor::setInputMode(bool mode)
{
	stepInputMode = mode;
}

void RhythmSynthAudioProcessor::setState(bool state)
{
	if (state)
	{
		dspFaust.setParamValue("/BasicMelodyFramework/Tap_Along", 1);
	}
	else
		dspFaust.setParamValue("/BasicMelodyFramework/Tap_Along", 0);
}

bool RhythmSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RhythmSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RhythmSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RhythmSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RhythmSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int RhythmSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RhythmSynthAudioProcessor::setCurrentProgram (int index)
{
}

const String RhythmSynthAudioProcessor::getProgramName (int index)
{
    return {};
}

void RhythmSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void RhythmSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void RhythmSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RhythmSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void RhythmSynthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
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
    }
}

//==============================================================================
bool RhythmSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* RhythmSynthAudioProcessor::createEditor()
{
    return new RhythmSynthAudioProcessorEditor (*this);
}

//==============================================================================
void RhythmSynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void RhythmSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void RhythmSynthAudioProcessor::hiResTimerCallback()
{
	//std::this_thread::sleep_for(std::chrono::microseconds(600));
	streamData(1);
}

void RhythmSynthAudioProcessor::setBaselines()
{
	baseline_stepDuration_Mean = gaitAnalysis.stepDuration_Mean_Long;
	baseline_cadence = 1/ baseline_stepDuration_Mean * 60;
	baseline_stanceDuration_Mean = gaitAnalysis.stanceDuration_Mean_Long;
	baseline_swingDuration_Mean = gaitAnalysis.swingDuration_Mean_Long;
	baseline_stepDuration_CoV = gaitAnalysis.stepDuration_CoV_Long + offset_stepTimeCoV;
	baseline_stanceDuration_CoV = gaitAnalysis.stanceDuration_CoV_Long;
	baseline_swingDuration_CoV = gaitAnalysis.swingDuration_CoV_Long;
	baseline_stepDuration_Asymmetry = gaitAnalysis.stepDuration_Asymmetry_Long + offset_stepTimeAsymm;
	baseline_stanceDuration_Asymmetry = gaitAnalysis.stanceDuration_Asymmetry_Long;
	baseline_swingDuration_Asymmetry = gaitAnalysis.swingDuration_Asymmetry_Long + offset_swingAsymm;

	baseline_s1_CoV = gaitAnalysis.supportTime_s1_CoV_Long;
	baseline_s12_CoV = gaitAnalysis.supportTime_s12_CoV_Long;
	baseline_s123_CoV = gaitAnalysis.supportTime_s123_CoV_Long;
	baseline_s23_CoV = gaitAnalysis.supportTime_s23_CoV_Long;
	baseline_s3_CoV = gaitAnalysis.supportTime_s3_CoV_Long;

	baseline_s1_Asymmetry = gaitAnalysis.s1_Asymmetry_Long;
	baseline_s12_Asymmetry = gaitAnalysis.s12_Asymmetry_Long;
	baseline_s123_Asymmetry = gaitAnalysis.s123_Asymmetry_Long + offset_flatFootAsymm;
	baseline_s23_Asymmetry = gaitAnalysis.s23_Asymmetry_Long;
	baseline_s3_Asymmetry = gaitAnalysis.s3_Asymmetry_Long;

	baseline_globalParams[0] = baseline_cadence;
	baseline_globalParams[1] = baseline_stepDuration_CoV;
	baseline_globalParams[2] = abs(baseline_swingDuration_Asymmetry);
	baseline_globalParams[3] = abs(baseline_s123_Asymmetry);
	baseline_globalParams[4] = abs(baseline_stepDuration_Asymmetry);

	for (int i = 0; i < 5; i++)
	{
		calcDesiredParameter(percentageChanges[i], i);
	}
		
}

void RhythmSynthAudioProcessor::setRandomSeed(float value)
{
	value = (int)value;
	dspFaust.setParamValue("/PreTest/Random_Seed", value);
}

void RhythmSynthAudioProcessor::initializeMappingMatrix()
{
	for (int i = 0; i < numGaitParamsMapped; i++)
	{
		for (int j = 0; j < numAudioParamsMapped; j++)
		{
			if (i == 0)
				mappingMatrix[i][j] = true;
			else
				mappingMatrix[i][j] = false;
		}
	}
}

void RhythmSynthAudioProcessor::updateMappingType(int value, int gaitParamIndex)
{
	mappingTypes[gaitParamIndex] = value + 1;
}
void RhythmSynthAudioProcessor::updateQuantSteps(int value, int audioParamIndex)
{
	numQuantizations[audioParamIndex] = value + 1;
}

void RhythmSynthAudioProcessor::setMappingMatrixValue(bool value, int rowIndex, int colIndex)
{
	mappingMatrix[rowIndex - 1][colIndex - 1] = value;
}

void RhythmSynthAudioProcessor::calcDesiredParameter(float value, int gaitParamIndex)
{
	if (!baselineSet)
	{
		return;
	}
	if (mappingDirections[gaitParamIndex] != 2)
	{
		if (value > 0)
		{
			mappingDirections[gaitParamIndex] = 3;
		}
		else
		{
			mappingDirections[gaitParamIndex] = 1;
		}		
	}
	desired_GlobalParams[gaitParamIndex] = baseline_globalParams[gaitParamIndex] * (100+value)/100;
	if (gaitParamIndex == 0)
	{
		dspFaust.setParamValue("/PreTest/Set_Tempo", desired_GlobalParams[gaitParamIndex]);
	}
	calcSlopeFactor(gaitParamIndex);
}

void RhythmSynthAudioProcessor::calcSlopeFactor(int gaitParamIndex)
{
		slopeFactor_GlobalParams[gaitParamIndex] = abs(baseline_globalParams[gaitParamIndex] - desired_GlobalParams[gaitParamIndex]);
}

void RhythmSynthAudioProcessor::collectCurrentGlobalParams()
{
	current_GlobalParams[0] = currentCadence;
	current_GlobalParams[1] = stepDuration_CoV_Short;
	current_GlobalParams[2] = abs(swingDuration_Asymmetry_Short);
	current_GlobalParams[3] = abs(s123_Asymmetry_Short);
	current_GlobalParams[4] = abs(stepDuration_Asymmetry_Short);
	current_GlobalParams[5] = supportTime_s1_Mean_Short;

	/*current_GlobalParams[0] = param1Filter.doBiQuad(current_GlobalParams[0], 0);
	current_GlobalParams[1] = param2Filter.doBiQuad(current_GlobalParams[1], 0);
	current_GlobalParams[2] = param3Filter.doBiQuad(current_GlobalParams[2], 0);
	current_GlobalParams[3] = param4Filter.doBiQuad(current_GlobalParams[3], 0);
	current_GlobalParams[4] = param5Filter.doBiQuad(current_GlobalParams[4], 0);*/
}

void RhythmSynthAudioProcessor::setAudioParamRanges()
{
	for (int i = 0; i < 6; i++)
	{
		audioParamsMin[i] = 100.0;
		audioParamsMax[i] = 400.0;
	}
}

void RhythmSynthAudioProcessor::obtainMappingValues()
{
	audioParams[0] = 0;
	audioParams[1] = 0;
	audioParams[2] = 0;
	audioParams[3] = 0;
	audioParams[4] = 0;

	bool atleastOneTrue = false;

	for (int i = 0; i < numAudioParamsMapped; i++)
	{

		for (int j = 0; j < numGaitParamsMapped; j++)
		{
			if (mappingMatrix[j][i] == true)
			{
				atleastOneTrue = true;
				audioParams[i] += paramMapping.getMappingValue(desired_GlobalParams[j], current_GlobalParams[j],
					slopeFactor_GlobalParams[j], audioParamsMin[i], audioParamsMax[i], mappingTypes[j],
					mappingDirections[j], numQuantizations[i]);
			}
		}
		//check Max/Min
		audioParams[i] = min(audioParams[i], audioParamsMax[i]);
		audioParams[i] = max(audioParams[i], audioParamsMin[i]);

		if (customLevelAudition)
		{
			audioParams[i] = 50 + numQuantizations[i] * 100;
		}

		if (!atleastOneTrue)
			//audioParams[i] = audioParamsMax[i];
			audioParams[i] = 420;
		atleastOneTrue = false;
	}
	
	//Expressive Roll Over
	audioParams[5] = paramMapping.getMappingValue(desired_GlobalParams[5], current_GlobalParams[5],
		slopeFactor_GlobalParams[5], audioParamsMin[5], audioParamsMax[5], mappingTypes[5],
		mappingDirections[5], numQuantizations[5]);

}

void RhythmSynthAudioProcessor::mapValuesToAudioParams()
{
	dspFaust.setParamValue("/PreTest/Audio_Param_1", audioParams[0]);
	dspFaust.setParamValue("/PreTest/Audio_Param_2", audioParams[1]);
	dspFaust.setParamValue("/PreTest/Audio_Param_3", audioParams[2]);
	dspFaust.setParamValue("/PreTest/Audio_Param_4", audioParams[3]);
	dspFaust.setParamValue("/PreTest/Audio_Param_5", audioParams[4]);
	dspFaust.setParamValue("/PreTest/Audio_Param_6", audioParams[5]);
}

void RhythmSynthAudioProcessor::getSineSimulatedInput()
{
	float fs = 200;
	float w = f / fs;

	heel_L_in = sineOsc(100, w, 0, 1);
	m1_L_in = sineOsc(100, w, 0.05, 1);
	toe_L_in = sineOsc(100, w, 0.06, 1);
	heel_R_in = sineOsc(100, w, 0, -1);;
	m1_R_in = sineOsc(100, w, 0.05, -1);
	toe_R_in = sineOsc(100, w, 0.06, -1);

	checkAllEnabled();
}

void RhythmSynthAudioProcessor::getOSCInput()
{
	heel_L_in = oscRecieve.oscDataArray[0];
	m1_L_in = oscRecieve.oscDataArray[1];
	toe_L_in = oscRecieve.oscDataArray[2];
	heel_R_in = oscRecieve.oscDataArray[3];
	m1_R_in = oscRecieve.oscDataArray[4];
	toe_R_in = oscRecieve.oscDataArray[5];

	heel_L_in = applyThreshold(heel_L_in,2000);
	heel_R_in = applyThreshold(heel_R_in, 2000);

	m1_L_in = applyThreshold(m1_L_in, 2000);
	m1_R_in = applyThreshold(m1_R_in, 2000);

	toe_L_in = applyThreshold(toe_L_in, 2000);
	toe_R_in = applyThreshold(toe_R_in, 2000);

	checkAllEnabled();
}

void RhythmSynthAudioProcessor::checkAllEnabled()
{
	heel_L_in = checkIfEnabled(heel_L_in, heelEnabled);
	heel_R_in = checkIfEnabled(heel_R_in, heelEnabled);

	m1_L_in = checkIfEnabled(m1_L_in, m1Enabled);
	m1_R_in = checkIfEnabled(m1_R_in, m1Enabled);

	toe_L_in = checkIfEnabled(toe_L_in, toeEnabled);
	toe_R_in = checkIfEnabled(toe_R_in, toeEnabled);
}

float RhythmSynthAudioProcessor::checkIfEnabled(float input, bool isEnabled)
{
	if (isEnabled)
		return input;
	else
		return 0;
}

float RhythmSynthAudioProcessor::applyThreshold(float input, float threshold)
{
	//return input;

	if (input >= threshold)
		return 1;
	else
		return 0;
}

int RhythmSynthAudioProcessor::streamData(int bufferSize)
{
	if (!hasStarted)
		return 0;

	time_elapsed += gaitAnalysis.samplePeriod;

	if (stepInputMode == true)
	{
		getOSCInput();
	}

	else
	{
		getSineSimulatedInput();
	}

	if (time_elapsed >= 10 && baselineSet == false && !usabilityTestMode)
	{
		setBaselines();
		baselineSet = true;
	}

	gaitAnalysis.processForceSamples(heel_L_in, heel_R_in, m1_L_in, m1_R_in, toe_L_in, toe_R_in,!stepInputMode);

	heel_L_in = gaitAnalysis.heel_L;
	m1_L_in = gaitAnalysis.m1_L;
	toe_L_in = gaitAnalysis.toe_L;

	heel_R_in = gaitAnalysis.heel_R;
	m1_R_in = gaitAnalysis.m1_R;
	toe_R_in = gaitAnalysis.toe_R;
	
	accSamplesElapsed++;

	if (accSamplesElapsed % 5 == 0)
	{
		updatePerformanceParams();
		collectCurrentGlobalParams();		
	}

	if (baselineSet)
	{
		obtainMappingValues();
		mapValuesToAudioParams();
	}

	if (stepCount % 2 == 0 && stepCount > 1)
	{
		if (!isTempoSet && isAutoTempoEnabled)
		{
			setTempoAuto();
		}
	}
	else isTempoSet = false;

	if (usabilityTestMode)
	{
		if (!baselineSet && !stepsCrossed_20 && stepCount == 20)
		{
			setBaselines();
			baselineSet = true;
			stepsCrossed_20 = true;
		}
		if (!stepsCrossed_100 && stepCount == 100)
		{
			stepsCrossed_100 = true;
			percentageChanges[0] = 10.0;
			calcDesiredParameter(percentageChanges[0], 0);
		}
		if (!stepsCrossed_200 && stepCount == 200)
		{
			stepsCrossed_200 = true;
			percentageChanges[0] = 20.0;
			calcDesiredParameter(percentageChanges[0], 0);
		}
		if (!stepsCrossed_300 && stepCount == 300)
		{
			stepsCrossed_300 = true;
			storeMeasurements();
		}
	}

	return 0;
}

void RhythmSynthAudioProcessor::setMusicalTime(bool state)
{
	if (state)
		musicalTimeSetting = 1;
	else
		musicalTimeSetting = 0;
}

float RhythmSynthAudioProcessor::sineOsc(float amplitude, float w, float startPhase, float flip)
{
	float sine_slow = 0.1 * sin(2 * 3.14*accSamplesElapsed*w / 10);
	float wave = amplitude * (0.5 + flip * 0.5*sin(2 * 3.14*(w*accSamplesElapsed - startPhase)));
	if (wave >= 0.5)
		return 4096;
	else
		return 0;
}

void RhythmSynthAudioProcessor::setTempoAuto()
{
	float tempo = 60.0 / stepDuration_Mean_Short;
	if (musicalTimeSetting == 0)
	{
		dspFaust.setParamValue("/PreTest/Set_Tempo", tempo);
	}

	if (musicalTimeSetting == 1)
	{
		dspFaust.setParamValue("/PreTest/Set_Tempo", 2*tempo);
	}

	isTempoSet = true;
}

void RhythmSynthAudioProcessor::setManualTempo(double tempo)
{
	if (!isAutoTempoEnabled)
	{
		dspFaust.setParamValue("/PreTest/Set_Tempo", tempo);
	}
}

void RhythmSynthAudioProcessor::setScale(double scale)
{
	scale = (int)scale;
	dspFaust.setParamValue("/PreTest/Active_Scale", scale);
}

void RhythmSynthAudioProcessor::storeMeasurements()
{
	for (int i = 0; i < stepCount; i++)
	{
		fprintf(walkingParamMeasurements, "%d,%c,%e,%e,%e,%e,%e,%e,%e,%e,%e,%e,%s\n", 
				i+1,gaitAnalysis.stepSequence[i],gaitAnalysis.stepDurationArrayLong[i],gaitAnalysis.stanceDurationArrayLong[i],
				gaitAnalysis.swingDurationArrayLong[i],gaitAnalysis.supportTime_s1_long[i],
				gaitAnalysis.supportTime_s2_long[i],gaitAnalysis.supportTime_s3_long[i],
				gaitAnalysis.supportTime_s12_long[i],gaitAnalysis.supportTime_s23_long[i],
				gaitAnalysis.supportTime_s13_long[i],gaitAnalysis.supportTime_s123_long[i],
				gaitAnalysis.rolloverPatternLong[i].c_str());
	}
}

void RhythmSynthAudioProcessor::updatePerformanceParams()
{
	stepCount = gaitAnalysis.stepCount;
	stepCount_L = gaitAnalysis.stepCount_L;
	stepCount_R = gaitAnalysis.stepCount_R;

	stepDuration_Mean_Long = gaitAnalysis.stepDuration_Mean_Long;
	stepDuration_Mean_Long_L = gaitAnalysis.stepDuration_Mean_Long_L;
	stepDuration_Mean_Long_R = gaitAnalysis.stepDuration_Mean_Long_R;

	//Short
	stepDuration_Mean_Short = gaitAnalysis.stepDuration_Mean_Short;
	stepDuration_Mean_Short_L = gaitAnalysis.stepDuration_Mean_Short_L;
	stepDuration_Mean_Short_R = gaitAnalysis.stepDuration_Mean_Short_R;
	currentCadence = 60 / stepDuration_Mean_Short;

	//Stance

	//Long
	stanceDuration_Mean_Long = gaitAnalysis.stanceDuration_Mean_Long;
	stanceDuration_Mean_Long_L = gaitAnalysis.stanceDuration_Mean_Long_L;
	stanceDuration_Mean_Long_R = gaitAnalysis.stanceDuration_Mean_Long_R;

	//Short
	stanceDuration_Mean_Short = gaitAnalysis.stanceDuration_Mean_Short;
	stanceDuration_Mean_Short_L = gaitAnalysis.stanceDuration_Mean_Short_L;
	stanceDuration_Mean_Short_R = gaitAnalysis.stanceDuration_Mean_Short_R;

	//Swing

	//Mean - Long
	swingDuration_Mean_Long = gaitAnalysis.swingDuration_Mean_Long;
	swingDuration_Mean_Long_L = gaitAnalysis.swingDuration_Mean_Long_L;
	swingDuration_Mean_Long_R = gaitAnalysis.swingDuration_Mean_Long_R;

	// Mean - Short
	swingDuration_Mean_Short = gaitAnalysis.swingDuration_Mean_Short;
	swingDuration_Mean_Short_L = gaitAnalysis.swingDuration_Mean_Short_L;
	swingDuration_Mean_Short_R = gaitAnalysis.swingDuration_Mean_Short_R;

	// =============================================== SUPPORT DURATION MEANS ================================

	// 1
	supportTime_s1_Mean_Long = gaitAnalysis.supportTime_s1_Mean_Long;
	supportTime_s1_CoV_Long = gaitAnalysis.supportTime_s1_CoV_Long;
	supportTime_s1_Mean_Short = gaitAnalysis.supportTime_s1_Mean_Short + offset_flatFootAsymm;
	supportTime_s1_CoV_Short = gaitAnalysis.supportTime_s1_CoV_Short;
	supportTime_s1_Mean_Long_L = gaitAnalysis.supportTime_s1_Mean_Long_L;
	supportTime_s1_Mean_Long_R = gaitAnalysis.supportTime_s1_Mean_Long_R;
	supportTime_s1_Mean_Short_L = gaitAnalysis.supportTime_s1_Mean_Short_L;
	supportTime_s1_Mean_Short_R = gaitAnalysis.supportTime_s1_Mean_Short_R;

	// 2
	supportTime_s2_Mean_Long = gaitAnalysis.supportTime_s2_Mean_Long;
	supportTime_s2_CoV_Long = gaitAnalysis.supportTime_s2_CoV_Long;
	supportTime_s2_Mean_Short = gaitAnalysis.supportTime_s2_Mean_Short;
	supportTime_s2_CoV_Short = gaitAnalysis.supportTime_s2_CoV_Short;
	supportTime_s2_Mean_Long_L = gaitAnalysis.supportTime_s2_Mean_Long_L;
	supportTime_s2_Mean_Long_R = gaitAnalysis.supportTime_s2_Mean_Long_R;
	supportTime_s2_Mean_Short_L = gaitAnalysis.supportTime_s2_Mean_Short_L;
	supportTime_s2_Mean_Short_R = gaitAnalysis.supportTime_s2_Mean_Short_R;

	// 3
	supportTime_s3_Mean_Long = gaitAnalysis.supportTime_s3_Mean_Long;
	supportTime_s3_CoV_Long = gaitAnalysis.supportTime_s3_CoV_Long;
	supportTime_s3_Mean_Short = gaitAnalysis.supportTime_s3_Mean_Short;
	supportTime_s3_CoV_Short = gaitAnalysis.supportTime_s3_CoV_Short;
	supportTime_s3_Mean_Long_L = gaitAnalysis.supportTime_s3_Mean_Long_L;
	supportTime_s3_Mean_Long_R = gaitAnalysis.supportTime_s3_Mean_Long_R;
	supportTime_s3_Mean_Short_L = gaitAnalysis.supportTime_s3_Mean_Short_L;
	supportTime_s3_Mean_Short_R = gaitAnalysis.supportTime_s3_Mean_Short_R;

	// 4
	supportTime_s12_Mean_Long = gaitAnalysis.supportTime_s12_Mean_Long;
	supportTime_s12_CoV_Long = gaitAnalysis.supportTime_s12_CoV_Long;
	supportTime_s12_Mean_Short = gaitAnalysis.supportTime_s12_Mean_Short;
	supportTime_s12_CoV_Short = gaitAnalysis.supportTime_s12_CoV_Short;
	supportTime_s12_Mean_Long_L = gaitAnalysis.supportTime_s12_Mean_Long_L;
	supportTime_s12_Mean_Long_R = gaitAnalysis.supportTime_s12_Mean_Long_R;
	supportTime_s12_Mean_Short_L = gaitAnalysis.supportTime_s12_Mean_Short_L;
	supportTime_s12_Mean_Short_R = gaitAnalysis.supportTime_s12_Mean_Short_R;

	// 5
	supportTime_s23_Mean_Long = gaitAnalysis.supportTime_s23_Mean_Long;
	supportTime_s23_CoV_Long = gaitAnalysis.supportTime_s23_CoV_Long;
	supportTime_s23_Mean_Short = gaitAnalysis.supportTime_s23_Mean_Short;
	supportTime_s23_CoV_Short = gaitAnalysis.supportTime_s23_CoV_Short;
	supportTime_s23_Mean_Long_L = gaitAnalysis.supportTime_s23_Mean_Long_L;
	supportTime_s23_Mean_Long_R = gaitAnalysis.supportTime_s23_Mean_Long_R;
	supportTime_s23_Mean_Short_L = gaitAnalysis.supportTime_s23_Mean_Short_L;
	supportTime_s23_Mean_Short_R = gaitAnalysis.supportTime_s23_Mean_Short_R;

	// 6
	supportTime_s13_Mean_Long = gaitAnalysis.supportTime_s13_Mean_Long;
	supportTime_s13_CoV_Long = gaitAnalysis.supportTime_s13_CoV_Long;
	supportTime_s13_Mean_Short = gaitAnalysis.supportTime_s13_Mean_Short;
	supportTime_s13_CoV_Short = gaitAnalysis.supportTime_s13_CoV_Short;
	supportTime_s13_Mean_Long_L = gaitAnalysis.supportTime_s13_Mean_Long_L;
	supportTime_s13_Mean_Long_R = gaitAnalysis.supportTime_s13_Mean_Long_R;
	supportTime_s13_Mean_Short_L = gaitAnalysis.supportTime_s13_Mean_Short_L;
	supportTime_s13_Mean_Short_R = gaitAnalysis.supportTime_s13_Mean_Short_R;

	// 7
	supportTime_s123_Mean_Long = gaitAnalysis.supportTime_s123_Mean_Long;
	supportTime_s123_CoV_Long = gaitAnalysis.supportTime_s123_CoV_Long;
	supportTime_s123_Mean_Short = gaitAnalysis.supportTime_s123_Mean_Short;
	supportTime_s123_CoV_Short = gaitAnalysis.supportTime_s123_CoV_Short;
	supportTime_s123_Mean_Long_L = gaitAnalysis.supportTime_s123_Mean_Long_L;
	supportTime_s123_Mean_Long_R = gaitAnalysis.supportTime_s123_Mean_Long_R;
	supportTime_s123_Mean_Short_L = gaitAnalysis.supportTime_s123_Mean_Short_L;
	supportTime_s123_Mean_Short_R = gaitAnalysis.supportTime_s123_Mean_Short_R;

	//CoV

	//Step

	stepDuration_CoV_Short = gaitAnalysis.stepDuration_CoV_Short + offset_stepTimeCoV;
	stepDuration_CoV_Long = gaitAnalysis.stepDuration_CoV_Long;

	//Stance

	stanceDuration_CoV_Short = gaitAnalysis.stanceDuration_CoV_Short;
	stanceDuration_CoV_Long = gaitAnalysis.stanceDuration_CoV_Long;

	//Swing

	swingDuration_CoV_Short = gaitAnalysis.swingDuration_CoV_Short;
	swingDuration_CoV_Long = gaitAnalysis.swingDuration_CoV_Long;

	// Asymmetry Coefficients

	//Step

	stepDuration_Asymmetry_Short = gaitAnalysis.stepDuration_Asymmetry_Short + offset_stepTimeAsymm;
	stepDuration_Asymmetry_Long = gaitAnalysis.stepDuration_Asymmetry_Long;

	//Stance

	stanceDuration_Asymmetry_Short = gaitAnalysis.stanceDuration_Asymmetry_Short;
	stanceDuration_Asymmetry_Long = gaitAnalysis.stanceDuration_Asymmetry_Long;

	//Swing

	swingDuration_Asymmetry_Short = gaitAnalysis.swingDuration_Asymmetry_Short + offset_swingAsymm;
	swingDuration_Asymmetry_Long = gaitAnalysis.swingDuration_Asymmetry_Long;

	//S1

	s1_Asymmetry_Short = gaitAnalysis.s1_Asymmetry_Short;
	s1_Asymmetry_Long = gaitAnalysis.s1_Asymmetry_Long;

	//S2

	s2_Asymmetry_Short = gaitAnalysis.s2_Asymmetry_Short;
	s2_Asymmetry_Long = gaitAnalysis.s2_Asymmetry_Long;

	//S3

	s3_Asymmetry_Short = gaitAnalysis.s3_Asymmetry_Short;
	s3_Asymmetry_Long = gaitAnalysis.s3_Asymmetry_Long;

	//S12

	s12_Asymmetry_Short = gaitAnalysis.s12_Asymmetry_Short;
	s12_Asymmetry_Long = gaitAnalysis.s12_Asymmetry_Long;

	//S23

	s23_Asymmetry_Short = gaitAnalysis.s23_Asymmetry_Short ;
	s23_Asymmetry_Long = gaitAnalysis.s23_Asymmetry_Long;

	//S13

	s13_Asymmetry_Short = gaitAnalysis.s13_Asymmetry_Short;
	s13_Asymmetry_Long = gaitAnalysis.s13_Asymmetry_Long;

	//S123

	s123_Asymmetry_Short = gaitAnalysis.s123_Asymmetry_Short + offset_flatFootAsymm;
	s123_Asymmetry_Long = gaitAnalysis.s123_Asymmetry_Long;

	/*lastRollover_L = gaitAnalysis.rolloverPatternShort_L[gaitAnalysis.stepCount_L-1 % gaitAnalysis.stepSizeWindow];
	lastRollover_R = gaitAnalysis.rolloverPatternShort_R[gaitAnalysis.stepCount_R-1 % gaitAnalysis.stepSizeWindow];*/

	lastRollover_L = gaitAnalysis.lastRollover_L;
	lastRollover_R = gaitAnalysis.lastRollover_R;

}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RhythmSynthAudioProcessor();
}
