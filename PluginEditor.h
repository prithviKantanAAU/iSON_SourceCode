/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <string>

//==============================================================================
/**
*/
class RhythmSynthAudioProcessorEditor  : public AudioProcessorEditor, public Timer, public ComboBox::Listener
{
public:
    RhythmSynthAudioProcessorEditor (RhythmSynthAudioProcessor&);
    ~RhythmSynthAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
	int stepCountEditor = 0;
	void timerCallback() override;
	void redrawUI(int currentTabIndex);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	//TextButton tapAlong;
	//Label tapAlongLabel;
    RhythmSynthAudioProcessor& processor;

	std::unique_ptr<TabbedComponent> tabs;
	//void currentTabChanged(int newCurrentTabIndex, const String &newCurrentTabName);

	ToggleButton onOffButton;	
	Label onOffLabel;

	ToggleButton autoCadenceDetect;
	Label autoCadenceDetectLabel;

	ToggleButton halfTime;
	Label halfTimeLabel;

	ToggleButton stepInputMode;
	Label stepInputModeLabel;

	ToggleButton usabilityTestMode;
	Label usabilityTestModeLabel;

	Label stepCountLabel;

	Label timeElapsedLabel;

	ToggleButton heelEnabled;
	ToggleButton m1Enabled;
	ToggleButton toeEnabled;

	Label stepDuration_Mean_Long;
	Label stepDuration_Mean_Long_L;
	Label stepDuration_Mean_Long_R;
	Label stepDuration_Mean_Short;
	Label stepDuration_Mean_Short_L;
	Label stepDuration_Mean_Short_R;
	Label stepDuration_CoV_Long;
	Label stepDuration_CoV_Short;
	Label stepDuration_Asymmetry_Long;
	Label stepDuration_Asymmetry_Short;

	Label stanceDuration_Mean_Long;
	Label stanceDuration_Mean_Long_L;
	Label stanceDuration_Mean_Long_R;
	Label stanceDuration_Mean_Short;
	Label stanceDuration_Mean_Short_L;
	Label stanceDuration_Mean_Short_R;
	Label stanceDuration_CoV_Long;
	Label stanceDuration_CoV_Short;
	Label stanceDuration_Asymmetry_Long;
	Label stanceDuration_Asymmetry_Short;

	Label swingDuration_Mean_Long;
	Label swingDuration_Mean_Long_L;
	Label swingDuration_Mean_Long_R;
	Label swingDuration_Mean_Short;
	Label swingDuration_Mean_Short_L;
	Label swingDuration_Mean_Short_R;
	Label swingDuration_CoV_Long;
	Label swingDuration_CoV_Short;
	Label swingDuration_Asymmetry_Long;
	Label swingDuration_Asymmetry_Short;

	Label s1_Duration_Mean_Long;
	Label s1_Duration_Mean_Long_L;
	Label s1_Duration_Mean_Long_R;
	Label s1_Duration_Mean_Short;
	Label s1_Duration_Mean_Short_L;
	Label s1_Duration_Mean_Short_R;
	Label s1_Duration_CoV_Long;
	Label s1_Duration_CoV_Short;
	Label s1_Duration_Asymmetry_Long;
	Label s1_Duration_Asymmetry_Short;

	Label s2_Duration_Mean_Long;
	Label s2_Duration_Mean_Long_L;
	Label s2_Duration_Mean_Long_R;
	Label s2_Duration_Mean_Short;
	Label s2_Duration_Mean_Short_L;
	Label s2_Duration_Mean_Short_R;
	Label s2_Duration_CoV_Long;
	Label s2_Duration_CoV_Short;
	Label s2_Duration_Asymmetry_Long;
	Label s2_Duration_Asymmetry_Short;

	Label s3_Duration_Mean_Long;
	Label s3_Duration_Mean_Long_L;
	Label s3_Duration_Mean_Long_R;
	Label s3_Duration_Mean_Short;
	Label s3_Duration_Mean_Short_L;
	Label s3_Duration_Mean_Short_R;
	Label s3_Duration_CoV_Long;
	Label s3_Duration_CoV_Short;
	Label s3_Duration_Asymmetry_Long;
	Label s3_Duration_Asymmetry_Short;

	Label s12_Duration_Mean_Long;
	Label s12_Duration_Mean_Long_L;
	Label s12_Duration_Mean_Long_R;
	Label s12_Duration_Mean_Short;
	Label s12_Duration_Mean_Short_L;
	Label s12_Duration_Mean_Short_R;
	Label s12_Duration_CoV_Long;
	Label s12_Duration_CoV_Short;
	Label s12_Duration_Asymmetry_Long;
	Label s12_Duration_Asymmetry_Short;

	Label s23_Duration_Mean_Long;
	Label s23_Duration_Mean_Long_L;
	Label s23_Duration_Mean_Long_R;
	Label s23_Duration_Mean_Short;
	Label s23_Duration_Mean_Short_L;
	Label s23_Duration_Mean_Short_R;
	Label s23_Duration_CoV_Long;
	Label s23_Duration_CoV_Short;
	Label s23_Duration_Asymmetry_Long;
	Label s23_Duration_Asymmetry_Short;

	Label s13_Duration_Mean_Long;
	Label s13_Duration_Mean_Long_L;
	Label s13_Duration_Mean_Long_R;
	Label s13_Duration_Mean_Short;
	Label s13_Duration_Mean_Short_L;
	Label s13_Duration_Mean_Short_R;
	Label s13_Duration_CoV_Long;
	Label s13_Duration_CoV_Short;
	Label s13_Duration_Asymmetry_Long;
	Label s13_Duration_Asymmetry_Short;

	Label s123_Duration_Mean_Long;
	Label s123_Duration_Mean_Long_L;
	Label s123_Duration_Mean_Long_R;
	Label s123_Duration_Mean_Short;
	Label s123_Duration_Mean_Short_L;
	Label s123_Duration_Mean_Short_R;
	Label s123_Duration_CoV_Long;
	Label s123_Duration_CoV_Short;
	Label s123_Duration_Asymmetry_Long;
	Label s123_Duration_Asymmetry_Short;
	
	Label leftHeelValue;
	Label rightHeelValue;
	Label leftM1Value;
	Label rightM1Value;
	Label leftToeValue;
	Label rightToeValue;

	Label gaitRolloverPattern;
	Label gaitRolloverLast_L;
	Label gaitRolloverLast_R;

	void updateValues(int currentTab);

	void updateMeanValues();
	void updateCoVValues();
	void updateAsymmetryValues();
	void updateGlobalValues();

	Slider stepOscFreq;
	Label stepOscFreqLabel;

	Slider desiredCadencePercent;
	Label desiredCadencePercentLabel;

	Slider idealStanceSwing;
	Label idealStanceSwingLabel;

	Slider stepSizeWindow;
	Label stepSizeWindowLabel;

	TextButton storeMeasurements;

	Label GP1;
	Label GP2;
	Label GP3;
	Label GP4;
	Label GP5;

	Slider DesiredChange1;
	Slider DesiredChange2;
	Slider DesiredChange3;
	Slider DesiredChange4;
	Slider DesiredChange5;
	
	Label DesiredChange;
	Label NumQuantizations;
	Label AudioParameters;
	Label MappingType;

	Label AP1;
	Label AP2;
	Label AP3;
	Label AP4;
	Label AP5;

	ToggleButton T11; ToggleButton T12; ToggleButton T13; ToggleButton T14; ToggleButton T15;
	ToggleButton T21; ToggleButton T22; ToggleButton T23; ToggleButton T24; ToggleButton T25;
	ToggleButton T31; ToggleButton T32; ToggleButton T33; ToggleButton T34; ToggleButton T35;
	ToggleButton T41; ToggleButton T42; ToggleButton T43; ToggleButton T44; ToggleButton T45;
	ToggleButton T51; ToggleButton T52; ToggleButton T53; ToggleButton T54; ToggleButton T55;

	ComboBox MT1; ComboBox MT2; ComboBox MT3; ComboBox MT4; ComboBox MT5;
	ComboBox NQ1; ComboBox NQ2; ComboBox NQ3; ComboBox NQ4; ComboBox NQ5;

	Slider stepTimeCoVOffset;
	Slider swingAsymmOffset;
	Slider flatFootAsymmOffset;
	Slider stepTimeAsymmOffset;

	Label stepTimeCoVOffsetLabel;
	Label swingAsymmOffsetLabel;
	Label flatFootAsymmOffsetLabel;
	Label stepTimeAsymmOffsetLabel;

	Label Baseline1;
	Label Desired1;
	Label Baseline2;
	Label Desired2;
	Label Baseline3;
	Label Desired3;
	Label Baseline4;
	Label Desired4;
	Label Baseline5;
	Label Desired5;
	Label Current1;
	Label Current2;
	Label Current3;
	Label Current4;
	Label Current5;

	void displayRecentRollover();
	void makeLabelsVisible();
	void makeSynthControlsVisible();
	void makeMappingMatrixVisible();
	void initializeMappingMatrix();
	void initializeMappingSliders();
	void initializeMappingCheckBoxes();
	void initializeMappingComboBoxes();
	void initializeMappingLabels();
	//void updateMappingMatrixValues();
	void positionMeanLabels();
	void positionCoVLabels();
	void positionAsymmetryLabels();
	void positionSynthControlLabels();
	void positionGlobalLabels();
	bool checkWithBaseline(float currentVar, float baselineVar, bool shouldBeLess);
	void checkCoVPerformance();
	void checkAsymmetryPerformance();
	void comboBoxChanged(ComboBox *box) override;

	void colourCoVLabels();
	void colourAsymmetryLabels();

	void toggleMeanParamTab(bool selected);
	void toggleVariabilityParamTab(bool selected);
	void toggleAsymmetryParamTab(bool selected);
	void toggleSynthControlTab(bool selected);

	bool isMeanParamTab;
	bool isVariabilityParamTab;
	bool isAsymmetryParamTab;
	bool isSynthControlTab;

	//For Colour Setting

	bool isBetter_Step_Cov;
	bool isBetter_Step_Asymmetry;
	bool isBetter_Stance_Cov;
	bool isBetter_Stance_Asymmetry;
	bool isBetter_Swing_Cov;
	bool isBetter_Swing_Asymmetry;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RhythmSynthAudioProcessorEditor)
};
