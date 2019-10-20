/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "DspFaust.h"
//#include <WinSock2.h>
//#include <WS2tcpip.h>
#include <Windows.h>
#include <string>
#include "BiQuad.h"
#include "OSC_Class.h"
#include "GaitAnalysis.h"
#include "ParamMapping.h"

class RhythmSynthAudioProcessor : public AudioProcessor, public HighResolutionTimer
{
public:
	//==============================================================================
	RhythmSynthAudioProcessor();
	~RhythmSynthAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;


	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void setState(bool state);
	bool isStepPeak;
	void stopFaust();
	void setOnOff(bool);
	void setInputMode(bool mode);

	float sineOsc(float amplitude, float freq, float startPhase, float flip);

	void getSineSimulatedInput();
	void getOSCInput();
	float applyThreshold(float input, float threshold);

	void updatePerformanceParams();

	void hiResTimerCallback() override;
	void setTempoAuto();
	void setStepFrequency(float freq);
	void setAutoCadenceDetect(bool state);
	void setDesiredCadence(float percent, bool isBaselineSet);
	void setStepSizeWindow(int window);
	void setBaselines();
	void calcDesiredParameter(float value,int gaitParamIndex);
	void calcSlopeFactor(int gaitParamIndex);
	void collectCurrentGlobalParams();
	void obtainMappingValues();
	void mapValuesToAudioParams();
	void storeMeasurements();

	FILE *walkingParamMeasurements;
	FILE *rolloverPatternMeasurements;
	FILE *performanceParamMeasurements;

	//==============================================================================

	//=============================================Mapping/Desired Parameters=============================

	float desired_GlobalParams[10] = { 0.0 };
	//float baseline_GlobalParams[10] = { 0.0 };
	float current_GlobalParams[10] = { 0.0 };
	float slopeFactor_GlobalParams[10] = { 0.0 };
	float desiredCadence = 0;

	int numGaitParamsMapped = 5;
	int numAudioParamsMapped = 5;

	bool mappingMatrix[5][5] = { false };
	void initializeMappingMatrix();
	void updateMappingType(int value, int gaitParamIndex);
	void updateQuantSteps(int value, int audioParamIndex);
	void setMappingMatrixValue(bool value, int rowIndex, int colIndex);
	void setRandomSeed(float value);

	int mappedParamIndex[10] = { 0 };
	float audioParams[10] = { 0.0 };
	float audioParamsMin[10] = { 0.0 };
	float audioParamsMax[10] = { 0.0 };
	int mappingTypes[10] = { 0 };
	int mappingDirections[10] = { 0 };
	int numQuantizations[10] = { 0 };
	float percentageChanges[5] = { 0 };

	//void setDesiredValue(double value, int paramIndex);
	void setAudioParamRanges();
	void initializeMappingConfiguration();
	void setMappingConfiguration(int paramIndex, int mapping);

	int musicalTimeSetting = 0;
	void setMusicalTime(bool state);

	void setManualTempo(double tempo);
	void setScale(double scale);
	
	//===============================================Gait Parameters======================================

	int accSamplesElapsed = 0;
	int streamData(int buffersize);
	float time_elapsed = 0;
	bool baselineSet = false;
	bool stepInputMode = false;

	bool heelEnabled = true;
	bool m1Enabled = true;
	bool toeEnabled = true;
	float checkIfEnabled(float input, bool isEnabled);
	void checkAllEnabled();

	OSCReceiverDemo oscRecieve;

	// Make L and R Separate

	int k;	//byte count
	
	float accsampleL[3];	//ACC sample left 
	float accsampleR[3];	//ACC sample right
	
	// Instantaneous Force Sample Values

	float heel_L_in;
	float heel_R_in;
	float m1_L_in;
	float m1_R_in;
	float toe_L_in;
	float toe_R_in;

	float idealStanceSwingRatio = 1.6315789;
	float minThresh = 0;
	float maxThresh = 14;

	// Step Count

	int stepCount = 0;
	int stepCount_L = 0;
	int stepCount_R = 0;

	// Short Term Step Duration
	
	bool hasStarted = false;
	bool firstSwingDone = false; //To ignore first stance
	bool swingStarted = false;
	bool dataStored = false;
	bool isTempoSet = false;
	bool isAutoTempoEnabled = false;

	// Sine Osc
	float f = 1;
	float rtFootLagRad = 0.05;

	// Mean & CoV

	//STEP

	//Long
	float stepDuration_Mean_Long = 0.0;
	float stepDuration_Mean_Long_L = 0.0;
	float stepDuration_Mean_Long_R = 0.0;

	//Short
	float stepDuration_Mean_Short = 0.0;
	float stepDuration_Mean_Short_L = 0.0;
	float stepDuration_Mean_Short_R = 0.0;
	float currentCadence = 0;

	//Stance

	//Long
	float stanceDuration_Mean_Long = 0.0;
	float stanceDuration_Mean_Long_L = 0.0;
	float stanceDuration_Mean_Long_R = 0.0;

	//Short
	float stanceDuration_Mean_Short = 0.0;
	float stanceDuration_Mean_Short_L = 0.0;
	float stanceDuration_Mean_Short_R = 0.0;

	//Swing

	//Mean - Long
	float swingDuration_Mean_Long = 0.0;
	float swingDuration_Mean_Long_L = 0.0;
	float swingDuration_Mean_Long_R = 0.0;

	// Mean - Short
	float swingDuration_Mean_Short = 0.0;
	float swingDuration_Mean_Short_L = 0.0;
	float swingDuration_Mean_Short_R = 0.0;

	// =============================================== SUPPORT DURATION MEANS ================================

	std::string lastRollover_L;
	std::string lastRollover_R;
	std::string rolloverPatternLong_L[10000] = { "" };
	std::string rolloverPatternLong_R[10000] = { "" };

	// 1
	float supportTime_s1_Mean_Long = 0;
	float supportTime_s1_CoV_Long = 0;
	float supportTime_s1_Mean_Short = 0;
	float supportTime_s1_CoV_Short = 0;
	float supportTime_s1_Mean_Long_L = 0;
	float supportTime_s1_Mean_Long_R = 0;
	float supportTime_s1_Mean_Short_L = 0;
	float supportTime_s1_Mean_Short_R = 0;

	// 2
	float supportTime_s2_Mean_Long = 0;
	float supportTime_s2_CoV_Long = 0;
	float supportTime_s2_Mean_Short = 0;
	float supportTime_s2_CoV_Short = 0;
	float supportTime_s2_Mean_Long_L = 0;
	float supportTime_s2_Mean_Long_R = 0;
	float supportTime_s2_Mean_Short_L = 0;
	float supportTime_s2_Mean_Short_R = 0;

	// 3
	float supportTime_s3_Mean_Long = 0;
	float supportTime_s3_CoV_Long = 0;
	float supportTime_s3_Mean_Short = 0;
	float supportTime_s3_CoV_Short = 0;
	float supportTime_s3_Mean_Long_L = 0;
	float supportTime_s3_Mean_Long_R = 0;
	float supportTime_s3_Mean_Short_L = 0;
	float supportTime_s3_Mean_Short_R = 0;

	// 4
	float supportTime_s12_Mean_Long = 0;
	float supportTime_s12_CoV_Long = 0;
	float supportTime_s12_Mean_Short = 0;
	float supportTime_s12_CoV_Short = 0;
	float supportTime_s12_Mean_Long_L = 0;
	float supportTime_s12_Mean_Long_R = 0;
	float supportTime_s12_Mean_Short_L = 0;
	float supportTime_s12_Mean_Short_R = 0;

	// 5
	float supportTime_s23_Mean_Long = 0;
	float supportTime_s23_CoV_Long = 0;
	float supportTime_s23_Mean_Short = 0;
	float supportTime_s23_CoV_Short = 0;
	float supportTime_s23_Mean_Long_L = 0;
	float supportTime_s23_Mean_Long_R = 0;
	float supportTime_s23_Mean_Short_L = 0;
	float supportTime_s23_Mean_Short_R = 0;

	// 6
	float supportTime_s13_Mean_Long = 0;
	float supportTime_s13_CoV_Long = 0;
	float supportTime_s13_Mean_Short = 0;
	float supportTime_s13_CoV_Short = 0;
	float supportTime_s13_Mean_Long_L = 0;
	float supportTime_s13_Mean_Long_R = 0;
	float supportTime_s13_Mean_Short_L = 0;
	float supportTime_s13_Mean_Short_R = 0;

	// 7
	float supportTime_s123_Mean_Long = 0;
	float supportTime_s123_CoV_Long = 0;
	float supportTime_s123_Mean_Short = 0;
	float supportTime_s123_CoV_Short = 0;
	float supportTime_s123_Mean_Long_L = 0;
	float supportTime_s123_Mean_Long_R = 0;
	float supportTime_s123_Mean_Short_L = 0;
	float supportTime_s123_Mean_Short_R = 0;

	//CoV

	//Step

	float stepDuration_CoV_Short = 0.0;
	float stepDuration_CoV_Long = 0.0;

	//Stance

	float stanceDuration_CoV_Short = 0.0;
	float stanceDuration_CoV_Long = 0.0;

	//Swing

	float swingDuration_CoV_Short = 0.0;
	float swingDuration_CoV_Long = 0.0;

	// Asymmetry Coefficients

	//Step

	float stepDuration_Asymmetry_Short = 0.0;
	float stepDuration_Asymmetry_Long = 0.0;

	//Stance

	float stanceDuration_Asymmetry_Short = 0.0;
	float stanceDuration_Asymmetry_Long = 0.0;

	//Swing

	float swingDuration_Asymmetry_Short = 0.0;
	float swingDuration_Asymmetry_Long = 0.0;

	//S1

	float s1_Asymmetry_Short = 0.0;
	float s1_Asymmetry_Long = 0.0;

	//S2

	float s2_Asymmetry_Short = 0.0;
	float s2_Asymmetry_Long = 0.0;

	//S3

	float s3_Asymmetry_Short = 0.0;
	float s3_Asymmetry_Long = 0.0;

	//S12

	float s12_Asymmetry_Short = 0.0;
	float s12_Asymmetry_Long = 0.0;

	//S23

	float s23_Asymmetry_Short = 0.0;
	float s23_Asymmetry_Long = 0.0;

	//S13

	float s13_Asymmetry_Short = 0.0;
	float s13_Asymmetry_Long = 0.0;

	//S123

	float s123_Asymmetry_Short = 0.0;
	float s123_Asymmetry_Long = 0.0;

	float rightFootLagRad;

	// Baseline Parameters

	float baseline_stepDuration_Mean;
	float baseline_cadence;
	float baseline_stanceDuration_Mean;
	float baseline_swingDuration_Mean;
	float baseline_stepDuration_CoV;
	float baseline_stanceDuration_CoV;
	float baseline_swingDuration_CoV;	
	float baseline_stepDuration_Asymmetry;
	float baseline_stanceDuration_Asymmetry;
	float baseline_swingDuration_Asymmetry;

	float baseline_globalParams[10] = { 0.0 };

	float baseline_s1_CoV;
	float baseline_s12_CoV;
	float baseline_s123_CoV;
	float baseline_s23_CoV;
	float baseline_s3_CoV;

	float baseline_s1_Asymmetry;
	float baseline_s12_Asymmetry;
	float baseline_s123_Asymmetry;
	float baseline_s23_Asymmetry;
	float baseline_s3_Asymmetry;

	//Test Gait Param Offsets

	float offset_stepTimeCoV = 0.0;
	float offset_swingAsymm = 0.0;
	float offset_flatFootAsymm = 0.0;
	float offset_stepTimeAsymm = 0.0;

	bool usabilityTestMode = false;
	bool customLevelAudition = false;
	bool stepsCrossed_20 = false;
	bool stepsCrossed_100 = false;
	bool stepsCrossed_200 = false;
	bool stepsCrossed_300 = false;

	

private:
    //==============================================================================
	DspFaust dspFaust;
	GaitAnalysis gaitAnalysis;
	ParamMapping paramMapping;
	
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmSynthAudioProcessor)
};
