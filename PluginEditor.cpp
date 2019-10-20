#include "PluginProcessor.h"
#include "PluginEditor.h"
#include<string>

RhythmSynthAudioProcessorEditor::RhythmSynthAudioProcessorEditor (RhythmSynthAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (1200, 900);
	
	//Tabs	
	tabs = std::make_unique<TabbedComponent>(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible(*tabs);	
	tabs->addTab("Mean Parameters",Colours::black,Component::getParentComponent(),1);
	tabs->addTab("Variability Parameters", Colours::black, Component::getParentComponent(), 1);
	tabs->addTab("Asymmetry Parameters", Colours::black, Component::getParentComponent(), 1);
	tabs->addTab("Synthesizer Controls", Colours::black, Component::getParentComponent(), 1);
	tabs->setBounds(0,0,getWidth(),getHeight());
	
	//On-Off Button
	addAndMakeVisible(onOffButton);
	onOffButton.setToggleState(false, dontSendNotification);
	onOffButton.onStateChange = [this]
	{
		processor.setOnOff(onOffButton.getToggleState());
	};
	addAndMakeVisible(onOffLabel);
	onOffLabel.setText("Start Music", dontSendNotification);
	onOffLabel.attachToComponent(&onOffButton, true);

	//Heel Enabled
	addAndMakeVisible(heelEnabled);
	heelEnabled.setToggleState(true, dontSendNotification);
	heelEnabled.onStateChange = [this]
	{
		processor.heelEnabled = heelEnabled.getToggleState();
	};

	//M1 Enabled
	addAndMakeVisible(m1Enabled);
	m1Enabled.setToggleState(true, dontSendNotification);
	m1Enabled.onStateChange = [this]
	{
		processor.m1Enabled = m1Enabled.getToggleState();
	};

	//Toe Enabled
	addAndMakeVisible(toeEnabled);
	toeEnabled.setToggleState(true, dontSendNotification);
	toeEnabled.onStateChange = [this]
	{
		processor.toeEnabled = toeEnabled.getToggleState();
	};

	//Usability Test Mode
	addAndMakeVisible(usabilityTestMode);
	usabilityTestMode.setToggleState(false, dontSendNotification);
	usabilityTestMode.onStateChange = [this]
	{
		processor.usabilityTestMode = usabilityTestMode.getToggleState();
	};
	addAndMakeVisible(usabilityTestModeLabel);
	usabilityTestModeLabel.setText("Usability Test", dontSendNotification);
	usabilityTestModeLabel.attachToComponent(&usabilityTestMode, true);

	//Measurement Storage Button
	addAndMakeVisible(storeMeasurements);
	storeMeasurements.setButtonText("Store Measurements");
	storeMeasurements.changeWidthToFitText();
	storeMeasurements.onClick = [this]
	{
		processor.storeMeasurements();
	};

	addAndMakeVisible(stepInputMode);
	stepInputMode.setToggleState(false, dontSendNotification);
	stepInputMode.onStateChange = [this]
	{
		processor.setInputMode(stepInputMode.getToggleState());
	};
	addAndMakeVisible(stepInputModeLabel);
	stepInputModeLabel.setText("Receive Wireless Sensor Data", dontSendNotification);
	stepInputModeLabel.attachToComponent(&stepInputMode, true);

	makeLabelsVisible();
	makeSynthControlsVisible();
	makeMappingMatrixVisible();
	initializeMappingMatrix();

	addAndMakeVisible(stepCountLabel);
	stepCountLabel.setText("Step Count: " + std::to_string(processor.stepCount), dontSendNotification);		
	startTimerHz(50);
}

RhythmSynthAudioProcessorEditor::~RhythmSynthAudioProcessorEditor()
{
	tabs->~TabbedComponent();
}

void RhythmSynthAudioProcessorEditor::makeSynthControlsVisible()
{
	//Step Oscillator Frequency
	addAndMakeVisible(stepOscFreq);
	stepOscFreq.setRange(0.1, 2);
	stepOscFreq.setValue(1);
	stepOscFreq.onValueChange = [this]
	{
		processor.setStepFrequency(stepOscFreq.getValue());
	};
	addAndMakeVisible(stepOscFreqLabel);
	stepOscFreqLabel.setText("Step Frequency (Simulated)", dontSendNotification);
	stepOscFreqLabel.attachToComponent(&stepOscFreq, true);

	//Desired Cadence Percent
	addAndMakeVisible(desiredCadencePercent);
	desiredCadencePercent.setRange(0.0, 3000.0);
	desiredCadencePercent.setValue(50.0);
	desiredCadencePercent.onValueChange = [this]
	{
		processor.setRandomSeed(desiredCadencePercent.getValue());
	};
	addAndMakeVisible(desiredCadencePercentLabel);
	desiredCadencePercentLabel.setText("Random Seed", dontSendNotification);
	desiredCadencePercentLabel.attachToComponent(&desiredCadencePercent, true);

	//Half Time
	addAndMakeVisible(halfTime);
	halfTime.setToggleState(false, dontSendNotification);
	halfTime.onStateChange = [this]
	{
		processor.setMusicalTime(halfTime.getToggleState());
	};
	addAndMakeVisible(halfTimeLabel);
	halfTimeLabel.setText("Half Time", dontSendNotification);
	halfTimeLabel.attachToComponent(&halfTime, true);

	//Auto Cadence Detect
	addAndMakeVisible(autoCadenceDetect);
	autoCadenceDetect.setToggleState(false, dontSendNotification);
	autoCadenceDetect.onStateChange = [this]
	{
		//processor.setAutoCadenceDetect(autoCadenceDetect.getToggleState());
		processor.customLevelAudition = autoCadenceDetect.getToggleState();
	};
	addAndMakeVisible(autoCadenceDetectLabel);
	autoCadenceDetectLabel.setText("Energy Audition", dontSendNotification);
	autoCadenceDetectLabel.attachToComponent(&autoCadenceDetect, true);

	//Step Size Window
	addAndMakeVisible(stepSizeWindow);
	stepSizeWindow.setRange(1, 5);
	stepSizeWindow.setValue(4);
	stepSizeWindow.onValueChange = [this]
	{
		processor.setStepSizeWindow(stepSizeWindow.getValue());
	};
	addAndMakeVisible(stepSizeWindowLabel);
	stepSizeWindowLabel.setText("Short Term Window (Steps)", dontSendNotification);
	stepSizeWindowLabel.attachToComponent(&stepSizeWindow, true);

}

void RhythmSynthAudioProcessorEditor::makeMappingMatrixVisible()
{
	addAndMakeVisible(GP1); addAndMakeVisible(GP2); addAndMakeVisible(GP3); 
	addAndMakeVisible(GP4); addAndMakeVisible(GP5);

	addAndMakeVisible(DesiredChange1); addAndMakeVisible(DesiredChange2);
	addAndMakeVisible(DesiredChange3); addAndMakeVisible(DesiredChange4);
	addAndMakeVisible(DesiredChange5); addAndMakeVisible(DesiredChange); //Along with label

	addAndMakeVisible(MappingType); addAndMakeVisible(NumQuantizations);
	addAndMakeVisible(AudioParameters);

	addAndMakeVisible(AP1); addAndMakeVisible(AP2); addAndMakeVisible(AP3);
	addAndMakeVisible(AP4); addAndMakeVisible(AP5);

	addAndMakeVisible(T11); addAndMakeVisible(T12); addAndMakeVisible(T13);
	addAndMakeVisible(T14); addAndMakeVisible(T15);
	addAndMakeVisible(T21); addAndMakeVisible(T22); addAndMakeVisible(T23);
	addAndMakeVisible(T24); addAndMakeVisible(T25);
	addAndMakeVisible(T31); addAndMakeVisible(T32); addAndMakeVisible(T33);
	addAndMakeVisible(T34); addAndMakeVisible(T35);
	addAndMakeVisible(T41); addAndMakeVisible(T42); addAndMakeVisible(T43);
	addAndMakeVisible(T44); addAndMakeVisible(T45);
	addAndMakeVisible(T51); addAndMakeVisible(T52); addAndMakeVisible(T53);
	addAndMakeVisible(T54); addAndMakeVisible(T55);

	addAndMakeVisible(MT1); addAndMakeVisible(MT2); addAndMakeVisible(MT3);
	addAndMakeVisible(MT4); addAndMakeVisible(MT5);

	addAndMakeVisible(NQ1); addAndMakeVisible(NQ2); addAndMakeVisible(NQ3);
	addAndMakeVisible(NQ4); addAndMakeVisible(NQ5);

	addAndMakeVisible(stepTimeAsymmOffset); addAndMakeVisible(stepTimeAsymmOffsetLabel);
	addAndMakeVisible(swingAsymmOffset); addAndMakeVisible(swingAsymmOffsetLabel);
	addAndMakeVisible(flatFootAsymmOffset); addAndMakeVisible(flatFootAsymmOffsetLabel);
	addAndMakeVisible(stepTimeCoVOffset); addAndMakeVisible(stepTimeCoVOffsetLabel);

}

void RhythmSynthAudioProcessorEditor::initializeMappingMatrix()
{
	initializeMappingSliders();
	initializeMappingCheckBoxes();
	initializeMappingComboBoxes();
	initializeMappingLabels();
}

void RhythmSynthAudioProcessorEditor::initializeMappingSliders()
{
	//	1
	DesiredChange1.setRange(-80.0, 80.0);
	DesiredChange1.setValue(0.0);
	DesiredChange1.onValueChange = [this]
	{
		processor.percentageChanges[0] = DesiredChange1.getValue();
		processor.calcDesiredParameter(processor.percentageChanges[0],0);
	};
	addAndMakeVisible(GP1);
	GP1.setText("Cadence", dontSendNotification);
	GP1.attachToComponent(&DesiredChange1, true);

	//	2
	DesiredChange2.setRange(-80.0, 80.0);
	DesiredChange2.setValue(0.0);
	DesiredChange2.onValueChange = [this]
	{
		processor.percentageChanges[1] = DesiredChange2.getValue();
		processor.calcDesiredParameter(processor.percentageChanges[1], 1);
	};
	addAndMakeVisible(GP2);
	GP2.setText("StepTime Var", dontSendNotification);
	GP2.attachToComponent(&DesiredChange2, true);

	//	3
	DesiredChange3.setRange(-80.0, 80.0);
	DesiredChange3.setValue(0.0);
	DesiredChange3.onValueChange = [this]
	{
		processor.percentageChanges[2] = DesiredChange3.getValue();
		processor.calcDesiredParameter(processor.percentageChanges[2], 2);
	};
	addAndMakeVisible(GP3);
	GP3.setText("Swing Asymmetry", dontSendNotification);
	GP3.attachToComponent(&DesiredChange3, true);

	//	4
	DesiredChange4.setRange(-80.0, 80.0);
	DesiredChange4.setValue(0.0);
	DesiredChange4.onValueChange = [this]
	{
		processor.percentageChanges[3] = DesiredChange4.getValue();
		processor.calcDesiredParameter(processor.percentageChanges[3], 3);
	};
	addAndMakeVisible(GP4);
	GP4.setText("Flat foot Asymmetry", dontSendNotification);
	GP4.attachToComponent(&DesiredChange4, true);

	//	5
	DesiredChange5.setRange(-80.0, 80.0);
	DesiredChange5.setValue(0.0);
	DesiredChange5.onValueChange = [this]
	{
		processor.percentageChanges[4] = DesiredChange5.getValue();
		processor.calcDesiredParameter(processor.percentageChanges[4], 4);
	};
	addAndMakeVisible(GP5);
	GP5.setText("Step Asymmetry", dontSendNotification);
	GP5.attachToComponent(&DesiredChange5, true);

	//Offset Slider Initialization

	stepTimeCoVOffset.setRange(40, 200);
	stepTimeCoVOffset.setValue(100);
	stepTimeCoVOffset.onValueChange = [this]
	{
		processor.setManualTempo(stepTimeCoVOffset.getValue());
	};
	stepTimeCoVOffsetLabel.setText("Manual Tempo Set", dontSendNotification);
	stepTimeCoVOffsetLabel.attachToComponent(&stepTimeCoVOffset, true);

	stepTimeAsymmOffset.setRange(0,6);
	stepTimeAsymmOffset.setValue(2);
	stepTimeAsymmOffset.onValueChange = [this]
	{
		processor.setScale(stepTimeAsymmOffset.getValue());
	};
	stepTimeAsymmOffsetLabel.setText("Musical Scale", dontSendNotification);
	stepTimeAsymmOffsetLabel.attachToComponent(&stepTimeAsymmOffset, true);

	swingAsymmOffset.setRange(0.0, 0.9);
	swingAsymmOffset.setValue(0.0);
	swingAsymmOffset.onValueChange = [this]
	{
		processor.offset_swingAsymm = swingAsymmOffset.getValue();
	};
	swingAsymmOffsetLabel.setText("Swing Asymm Offset", dontSendNotification);
	swingAsymmOffsetLabel.attachToComponent(&swingAsymmOffset, true);

	flatFootAsymmOffset.setRange(0.0, 0.9);
	flatFootAsymmOffset.setValue(0.0);
	flatFootAsymmOffset.onValueChange = [this]
	{
		processor.offset_flatFootAsymm = flatFootAsymmOffset.getValue();
	};
	flatFootAsymmOffsetLabel.setText("FlatFoot Asymm Offset", dontSendNotification);
	flatFootAsymmOffsetLabel.attachToComponent(&flatFootAsymmOffset, true);

}

void RhythmSynthAudioProcessorEditor::initializeMappingCheckBoxes()
{
	//	T11
	T11.setToggleState(true, dontSendNotification);
	T11.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T11.getToggleState(),1,1);
	};

	//	T12
	T12.setToggleState(true, dontSendNotification);
	T12.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T12.getToggleState(), 1, 2);
	};

	//	T13
	T13.setToggleState(true, dontSendNotification);
	T13.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T13.getToggleState(), 1, 3);
	};

	//	T14
	T14.setToggleState(true, dontSendNotification);
	T14.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T14.getToggleState(), 1, 4);
	};

	//	T15
	T15.setToggleState(true, dontSendNotification);
	T15.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T15.getToggleState(), 1, 5);
	};

	//	T21
	T21.setToggleState(false, dontSendNotification);
	T21.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T21.getToggleState(), 2, 1);
	};

	//	T22
	T22.setToggleState(false, dontSendNotification);
	T22.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T22.getToggleState(), 2, 2);
	};

	//	T23
	T23.setToggleState(false, dontSendNotification);
	T23.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T23.getToggleState(), 2, 3);
	};

	//	T24
	T24.setToggleState(false, dontSendNotification);
	T24.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T24.getToggleState(), 2, 4);
	};

	//	T25
	T25.setToggleState(false, dontSendNotification);
	T25.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T25.getToggleState(), 2, 5);
	};

	//	T31
	T31.setToggleState(false, dontSendNotification);
	T31.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T31.getToggleState(), 3, 1);
	};

	//	T32
	T32.setToggleState(false, dontSendNotification);
	T32.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T32.getToggleState(), 3, 2);
	};

	//	T33
	T33.setToggleState(false, dontSendNotification);
	T33.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T33.getToggleState(), 3, 3);
	};

	//	T34
	T34.setToggleState(false, dontSendNotification);
	T34.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T34.getToggleState(), 3, 4);
	};

	//	T35
	T35.setToggleState(false, dontSendNotification);
	T35.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T35.getToggleState(), 3, 5);
	};

	//	T41
	T41.setToggleState(false, dontSendNotification);
	T41.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T41.getToggleState(), 4, 1);
	};

	//	T42
	T42.setToggleState(false, dontSendNotification);
	T42.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T42.getToggleState(), 4, 2);
	};

	//	T43
	T43.setToggleState(false, dontSendNotification);
	T43.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T43.getToggleState(), 4, 3);
	};

	//	T44
	T44.setToggleState(false, dontSendNotification);
	T44.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T44.getToggleState(), 4, 4);
	};

	//	T45
	T45.setToggleState(false, dontSendNotification);
	T45.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T45.getToggleState(), 4, 5);
	};

	//	T51
	T51.setToggleState(false, dontSendNotification);
	T51.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T51.getToggleState(), 5, 1);
	};

	//	T52
	T52.setToggleState(false, dontSendNotification);
	T52.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T52.getToggleState(), 5, 2);
	};

	//	T53
	T53.setToggleState(false, dontSendNotification);
	T53.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T53.getToggleState(), 5, 3);
	};

	//	T54
	T54.setToggleState(false, dontSendNotification);
	T54.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T54.getToggleState(), 5, 4);
	};

	//	T55
	T55.setToggleState(false, dontSendNotification);
	T55.onStateChange = [this]
	{
		processor.setMappingMatrixValue(T55.getToggleState(), 5, 5);
	};
}

void RhythmSynthAudioProcessorEditor::comboBoxChanged(ComboBox* box) 
{
	processor.updateMappingType(MT1.getSelectedItemIndex(), 0);
	processor.updateMappingType(MT2.getSelectedItemIndex(), 1);
	processor.updateMappingType(MT3.getSelectedItemIndex(), 2);
	processor.updateMappingType(MT4.getSelectedItemIndex(), 3);
	processor.updateMappingType(MT5.getSelectedItemIndex(), 4);

	processor.updateQuantSteps(NQ1.getSelectedItemIndex(), 0);
	processor.updateQuantSteps(NQ2.getSelectedItemIndex(), 1);
	processor.updateQuantSteps(NQ3.getSelectedItemIndex(), 2);
	processor.updateQuantSteps(NQ4.getSelectedItemIndex(), 3);
	processor.updateQuantSteps(NQ5.getSelectedItemIndex(), 4);
}

void RhythmSynthAudioProcessorEditor::initializeMappingComboBoxes()
{
	StringArray mappingTypeArray = { "1","2","3" };
	StringArray quantizationStepsArray = {"1","2","3","4","5","6","7","8","9","10"};

	MT1.addItemList(mappingTypeArray, 1);
	MT1.setSelectedId(2,sendNotificationAsync);
	MT1.addListener(this);	
	MT2.addItemList(mappingTypeArray, 1);
	MT2.setSelectedId(1, sendNotificationAsync);
	MT2.addListener(this);
	MT3.addItemList(mappingTypeArray, 1);
	MT3.setSelectedId(1, sendNotificationAsync);
	MT3.addListener(this);
	MT4.addItemList(mappingTypeArray, 1);
	MT4.setSelectedId(1, sendNotificationAsync);
	MT4.addListener(this);
	MT5.addItemList(mappingTypeArray, 1);
	MT5.setSelectedId(1, sendNotificationAsync);
	MT5.addListener(this);
	NQ1.addItemList(quantizationStepsArray, 1);
	NQ1.setSelectedId(3, sendNotificationAsync);
	NQ1.addListener(this);
	NQ2.addItemList(quantizationStepsArray, 1);
	NQ2.setSelectedId(3, sendNotificationAsync);
	NQ2.addListener(this);
	NQ3.addItemList(quantizationStepsArray, 1);
	NQ3.setSelectedId(3, sendNotificationAsync);
	NQ3.addListener(this);
	NQ4.addItemList(quantizationStepsArray, 1);
	NQ4.setSelectedId(3, sendNotificationAsync);
	NQ4.addListener(this);
	NQ5.addItemList(quantizationStepsArray, 1);
	NQ5.setSelectedId(3, sendNotificationAsync);
	NQ5.addListener(this);
}

void RhythmSynthAudioProcessorEditor::initializeMappingLabels()
{
	DesiredChange.setText("Percentage Desired Change",dontSendNotification);
	NumQuantizations.setText("Quantization Steps", dontSendNotification);
	AudioParameters.setText("Audio Parameters", dontSendNotification);
	MappingType.setText("Mapping Types", dontSendNotification);

	AP1.setText("AP1", dontSendNotification);
	AP2.setText("AP2", dontSendNotification);
	AP3.setText("AP3", dontSendNotification);
	AP4.setText("AP4", dontSendNotification);
	AP5.setText("AP5", dontSendNotification);
}

void RhythmSynthAudioProcessorEditor::makeLabelsVisible()
{
	addAndMakeVisible(timeElapsedLabel);
	addAndMakeVisible(stepDuration_Mean_Long);
	addAndMakeVisible(stepDuration_Mean_Long_L);
	addAndMakeVisible(stepDuration_Mean_Long_R);
	addAndMakeVisible(stepDuration_Mean_Short);
	addAndMakeVisible(stepDuration_Mean_Short_L);
	addAndMakeVisible(stepDuration_Mean_Short_R);
	addAndMakeVisible(stepDuration_CoV_Long);
	addAndMakeVisible(stepDuration_CoV_Short);
	addAndMakeVisible(stepDuration_Asymmetry_Long);
	addAndMakeVisible(stepDuration_Asymmetry_Short);

	addAndMakeVisible(stanceDuration_Mean_Long);
	addAndMakeVisible(stanceDuration_Mean_Long_L);
	addAndMakeVisible(stanceDuration_Mean_Long_R);
	addAndMakeVisible(stanceDuration_Mean_Short);
	addAndMakeVisible(stanceDuration_Mean_Short_L);
	addAndMakeVisible(stanceDuration_Mean_Short_R);
	addAndMakeVisible(stanceDuration_CoV_Long);
	addAndMakeVisible(stanceDuration_CoV_Short);
	addAndMakeVisible(stanceDuration_Asymmetry_Long);
	addAndMakeVisible(stanceDuration_Asymmetry_Short);

	addAndMakeVisible(swingDuration_Mean_Long);
	addAndMakeVisible(swingDuration_Mean_Long_L);
	addAndMakeVisible(swingDuration_Mean_Long_R);
	addAndMakeVisible(swingDuration_Mean_Short);
	addAndMakeVisible(swingDuration_Mean_Short_L);
	addAndMakeVisible(swingDuration_Mean_Short_R);
	addAndMakeVisible(swingDuration_CoV_Long);
	addAndMakeVisible(swingDuration_CoV_Short);
	addAndMakeVisible(swingDuration_Asymmetry_Long);
	addAndMakeVisible(swingDuration_Asymmetry_Short);

	addAndMakeVisible(s1_Duration_Mean_Long);
	addAndMakeVisible(s1_Duration_Mean_Long_L);
	addAndMakeVisible(s1_Duration_Mean_Long_R);
	addAndMakeVisible(s1_Duration_Mean_Short);
	addAndMakeVisible(s1_Duration_Mean_Short_L);
	addAndMakeVisible(s1_Duration_Mean_Short_R);
	addAndMakeVisible(s1_Duration_CoV_Long);
	addAndMakeVisible(s1_Duration_CoV_Short);
	addAndMakeVisible(s1_Duration_Asymmetry_Long);
	addAndMakeVisible(s1_Duration_Asymmetry_Short);

	addAndMakeVisible(s2_Duration_Mean_Long);
	addAndMakeVisible(s2_Duration_Mean_Long_L);
	addAndMakeVisible(s2_Duration_Mean_Long_R);
	addAndMakeVisible(s2_Duration_Mean_Short);
	addAndMakeVisible(s2_Duration_Mean_Short_L);
	addAndMakeVisible(s2_Duration_Mean_Short_R);
	addAndMakeVisible(s2_Duration_CoV_Long);
	addAndMakeVisible(s2_Duration_CoV_Short);
	addAndMakeVisible(s2_Duration_Asymmetry_Long);
	addAndMakeVisible(s2_Duration_Asymmetry_Short);

	addAndMakeVisible(s3_Duration_Mean_Long);
	addAndMakeVisible(s3_Duration_Mean_Long_L);
	addAndMakeVisible(s3_Duration_Mean_Long_R);
	addAndMakeVisible(s3_Duration_Mean_Short);
	addAndMakeVisible(s3_Duration_Mean_Short_L);
	addAndMakeVisible(s3_Duration_Mean_Short_R);
	addAndMakeVisible(s3_Duration_CoV_Long);
	addAndMakeVisible(s3_Duration_CoV_Short);
	addAndMakeVisible(s3_Duration_Asymmetry_Long);
	addAndMakeVisible(s3_Duration_Asymmetry_Short);

	addAndMakeVisible(s12_Duration_Mean_Long);
	addAndMakeVisible(s12_Duration_Mean_Long_L);
	addAndMakeVisible(s12_Duration_Mean_Long_R);
	addAndMakeVisible(s12_Duration_Mean_Short);
	addAndMakeVisible(s12_Duration_Mean_Short_L);
	addAndMakeVisible(s12_Duration_Mean_Short_R);
	addAndMakeVisible(s12_Duration_CoV_Long);
	addAndMakeVisible(s12_Duration_CoV_Short);
	addAndMakeVisible(s12_Duration_Asymmetry_Long);
	addAndMakeVisible(s12_Duration_Asymmetry_Short);

	addAndMakeVisible(s23_Duration_Mean_Long);
	addAndMakeVisible(s23_Duration_Mean_Long_L);
	addAndMakeVisible(s23_Duration_Mean_Long_R);
	addAndMakeVisible(s23_Duration_Mean_Short);
	addAndMakeVisible(s23_Duration_Mean_Short_L);
	addAndMakeVisible(s23_Duration_Mean_Short_R);
	addAndMakeVisible(s23_Duration_CoV_Long);
	addAndMakeVisible(s23_Duration_CoV_Short);
	addAndMakeVisible(s23_Duration_Asymmetry_Long);
	addAndMakeVisible(s23_Duration_Asymmetry_Short);

	addAndMakeVisible(s13_Duration_Mean_Long);
	addAndMakeVisible(s13_Duration_Mean_Long_L);
	addAndMakeVisible(s13_Duration_Mean_Long_R);
	addAndMakeVisible(s13_Duration_Mean_Short);
	addAndMakeVisible(s13_Duration_Mean_Short_L);
	addAndMakeVisible(s13_Duration_Mean_Short_R);
	addAndMakeVisible(s13_Duration_CoV_Long);
	addAndMakeVisible(s13_Duration_CoV_Short);
	addAndMakeVisible(s13_Duration_Asymmetry_Long);
	addAndMakeVisible(s13_Duration_Asymmetry_Short);

	addAndMakeVisible(s123_Duration_Mean_Long);
	addAndMakeVisible(s123_Duration_Mean_Long_L);
	addAndMakeVisible(s123_Duration_Mean_Long_R);
	addAndMakeVisible(s123_Duration_Mean_Short);
	addAndMakeVisible(s123_Duration_Mean_Short_L);
	addAndMakeVisible(s123_Duration_Mean_Short_R);
	addAndMakeVisible(s123_Duration_CoV_Long);
	addAndMakeVisible(s123_Duration_CoV_Short);
	addAndMakeVisible(s123_Duration_Asymmetry_Long);
	addAndMakeVisible(s123_Duration_Asymmetry_Short);

	addAndMakeVisible(gaitRolloverPattern);
	addAndMakeVisible(gaitRolloverLast_L);
	addAndMakeVisible(gaitRolloverLast_R);

	addAndMakeVisible(leftHeelValue);
	addAndMakeVisible(rightHeelValue);
	addAndMakeVisible(leftM1Value);
	addAndMakeVisible(rightM1Value);
	addAndMakeVisible(leftToeValue);
	addAndMakeVisible(rightToeValue);

	addAndMakeVisible(Baseline1);
	addAndMakeVisible(Baseline2);
	addAndMakeVisible(Baseline3);
	addAndMakeVisible(Baseline4);
	addAndMakeVisible(Baseline5);

	addAndMakeVisible(Desired1);
	addAndMakeVisible(Desired2);
	addAndMakeVisible(Desired3);
	addAndMakeVisible(Desired4);
	addAndMakeVisible(Desired5);

	addAndMakeVisible(Current1);
	addAndMakeVisible(Current2);
	addAndMakeVisible(Current3);
	addAndMakeVisible(Current4);
	addAndMakeVisible(Current5);
}

void RhythmSynthAudioProcessorEditor::redrawUI(int currentTabIndex)
{
	switch (currentTabIndex)
	{
	case 0:
		isMeanParamTab = true;
		isVariabilityParamTab = false;
		isAsymmetryParamTab = false;
		isSynthControlTab = false;

		toggleMeanParamTab(isMeanParamTab);
		toggleVariabilityParamTab(isVariabilityParamTab);
		toggleAsymmetryParamTab(isAsymmetryParamTab);	
		toggleSynthControlTab(isSynthControlTab);
		break;
	case 1:
		isMeanParamTab = false;
		isVariabilityParamTab = true;
		isAsymmetryParamTab = false;
		isSynthControlTab = false;

		toggleMeanParamTab(isMeanParamTab);
		toggleVariabilityParamTab(isVariabilityParamTab);
		toggleAsymmetryParamTab(isAsymmetryParamTab);
		toggleSynthControlTab(isSynthControlTab);
		break;
	case 2:
		isMeanParamTab = false;
		isVariabilityParamTab = false;
		isAsymmetryParamTab = true;
		isSynthControlTab = false;

		toggleMeanParamTab(isMeanParamTab);
		toggleVariabilityParamTab(isVariabilityParamTab);
		toggleAsymmetryParamTab(isAsymmetryParamTab);
		toggleSynthControlTab(isSynthControlTab);
		break;

	case 3:
		isMeanParamTab = false;
		isVariabilityParamTab = false;
		isAsymmetryParamTab = false;
		isSynthControlTab = true;

		toggleMeanParamTab(isMeanParamTab);
		toggleVariabilityParamTab(isVariabilityParamTab);
		toggleAsymmetryParamTab(isAsymmetryParamTab);
		toggleSynthControlTab(isSynthControlTab);
		break;
	}
}

void RhythmSynthAudioProcessorEditor::toggleMeanParamTab(bool on)
{
	stepDuration_Mean_Long.setVisible(on);
	stepDuration_Mean_Long_L.setVisible(on);
	stepDuration_Mean_Long_R.setVisible(on);
	stepDuration_Mean_Short.setVisible(on);
	stepDuration_Mean_Short_L.setVisible(on);
	stepDuration_Mean_Short_R.setVisible(on);
	stanceDuration_Mean_Long.setVisible(on);
	stanceDuration_Mean_Long_L.setVisible(on);
	stanceDuration_Mean_Long_R.setVisible(on);
	stanceDuration_Mean_Short.setVisible(on);
	stanceDuration_Mean_Short_L.setVisible(on);
	stanceDuration_Mean_Short_R.setVisible(on);
	swingDuration_Mean_Long.setVisible(on);
	swingDuration_Mean_Long_L.setVisible(on);
	swingDuration_Mean_Long_R.setVisible(on);
	swingDuration_Mean_Short.setVisible(on);
	swingDuration_Mean_Short_L.setVisible(on);
	swingDuration_Mean_Short_R.setVisible(on);

	s1_Duration_Mean_Long.setVisible(on);
	s1_Duration_Mean_Short.setVisible(on);

	s2_Duration_Mean_Long.setVisible(on);
	s2_Duration_Mean_Short.setVisible(on);

	s3_Duration_Mean_Long.setVisible(on);
	s3_Duration_Mean_Short.setVisible(on);

	s12_Duration_Mean_Long.setVisible(on);
	s12_Duration_Mean_Short.setVisible(on);

	s23_Duration_Mean_Long.setVisible(on);
	s23_Duration_Mean_Short.setVisible(on);

	s13_Duration_Mean_Long.setVisible(on);
	s13_Duration_Mean_Short.setVisible(on);

	s123_Duration_Mean_Long.setVisible(on);
	s123_Duration_Mean_Short.setVisible(on);
}

void RhythmSynthAudioProcessorEditor::toggleVariabilityParamTab(bool on)
{
	stepDuration_CoV_Long.setVisible(on);	
	stepDuration_CoV_Short.setVisible(on);
	stanceDuration_CoV_Long.setVisible(on);
	stanceDuration_CoV_Short.setVisible(on);
	swingDuration_CoV_Long.setVisible(on);
	swingDuration_CoV_Short.setVisible(on);	

	s1_Duration_CoV_Long.setVisible(on);
	s1_Duration_CoV_Short.setVisible(on);

	s2_Duration_CoV_Long.setVisible(on);
	s2_Duration_CoV_Short.setVisible(on);

	s3_Duration_CoV_Long.setVisible(on);
	s3_Duration_CoV_Short.setVisible(on);

	s12_Duration_CoV_Long.setVisible(on);
	s12_Duration_CoV_Short.setVisible(on);

	s23_Duration_CoV_Long.setVisible(on);
	s23_Duration_CoV_Short.setVisible(on);

	s13_Duration_CoV_Long.setVisible(on);
	s13_Duration_CoV_Short.setVisible(on);

	s123_Duration_CoV_Long.setVisible(on);
	s123_Duration_CoV_Short.setVisible(on);

	gaitRolloverPattern.setVisible(on);
	gaitRolloverLast_L.setVisible(on);
	gaitRolloverLast_R.setVisible(on);

	bool a = gaitRolloverPattern.isShowing();
	a = true;
}

void RhythmSynthAudioProcessorEditor::toggleAsymmetryParamTab(bool on)
{
	stepDuration_Asymmetry_Long.setVisible(on);
	stepDuration_Asymmetry_Short.setVisible(on);
	stanceDuration_Asymmetry_Long.setVisible(on);
	stanceDuration_Asymmetry_Short.setVisible(on);
	swingDuration_Asymmetry_Long.setVisible(on);
	swingDuration_Asymmetry_Short.setVisible(on);

	s1_Duration_Asymmetry_Long.setVisible(on);
	s1_Duration_Asymmetry_Short.setVisible(on);

	s2_Duration_Asymmetry_Long.setVisible(on);
	s2_Duration_Asymmetry_Short.setVisible(on);

	s3_Duration_Asymmetry_Long.setVisible(on);
	s3_Duration_Asymmetry_Short.setVisible(on);

	s12_Duration_Asymmetry_Long.setVisible(on);
	s12_Duration_Asymmetry_Short.setVisible(on);

	s23_Duration_Asymmetry_Long.setVisible(on);
	s23_Duration_Asymmetry_Short.setVisible(on);

	s13_Duration_Asymmetry_Long.setVisible(on);
	s13_Duration_Asymmetry_Short.setVisible(on);

	s123_Duration_Asymmetry_Long.setVisible(on);
	s123_Duration_Asymmetry_Short.setVisible(on);
}

void RhythmSynthAudioProcessorEditor::toggleSynthControlTab(bool on)
{
	autoCadenceDetect.setVisible(on);
	halfTime.setVisible(on);
	desiredCadencePercent.setVisible(on);
	stepOscFreq.setVisible(on);
	stepSizeWindow.setVisible(on);
	stepTimeCoVOffset.setVisible(on);
	stepTimeAsymmOffset.setVisible(on);
	swingAsymmOffset.setVisible(on);
	flatFootAsymmOffset.setVisible(on);

	GP1.setVisible(on);
	GP2.setVisible(on);
	GP3.setVisible(on);
	GP4.setVisible(on);
	GP5.setVisible(on);

	DesiredChange1.setVisible(on);
	DesiredChange2.setVisible(on);
	DesiredChange3.setVisible(on);
	DesiredChange4.setVisible(on);
	DesiredChange5.setVisible(on);

	DesiredChange.setVisible(on);
	NumQuantizations.setVisible(on);
	AudioParameters.setVisible(on);
	MappingType.setVisible(on);

	AP1.setVisible(on);
	AP2.setVisible(on);
	AP3.setVisible(on);
	AP4.setVisible(on);
	AP5.setVisible(on);

	T11.setVisible(on); T12.setVisible(on); T13.setVisible(on); T14.setVisible(on); T15.setVisible(on);
	T21.setVisible(on); T22.setVisible(on); T23.setVisible(on); T24.setVisible(on); T25.setVisible(on);
	T31.setVisible(on); T32.setVisible(on); T33.setVisible(on); T34.setVisible(on); T35.setVisible(on);
	T41.setVisible(on); T42.setVisible(on); T43.setVisible(on); T44.setVisible(on); T45.setVisible(on);
	T51.setVisible(on); T52.setVisible(on); T53.setVisible(on); T54.setVisible(on); T55.setVisible(on);
	
	MT1.setVisible(on); MT2.setVisible(on); MT3.setVisible(on); MT4.setVisible(on); MT5.setVisible(on);
	NQ1.setVisible(on); NQ2.setVisible(on); NQ3.setVisible(on); NQ4.setVisible(on); NQ5.setVisible(on);

	Baseline1.setVisible(on);
	Baseline2.setVisible(on);
	Baseline3.setVisible(on);
	Baseline4.setVisible(on);
	Baseline5.setVisible(on);

	Desired1.setVisible(on);
	Desired2.setVisible(on);
	Desired3.setVisible(on);
	Desired4.setVisible(on);
	Desired5.setVisible(on);

	Current1.setVisible(on);
	Current2.setVisible(on);
	Current3.setVisible(on);
	Current4.setVisible(on);
	Current5.setVisible(on);

}

void RhythmSynthAudioProcessorEditor::updateGlobalValues()
{
	timeElapsedLabel.setText("Time Elapsed: " + std::to_string(processor.time_elapsed) + " sec",dontSendNotification);
	stepCountLabel.setText("Step Count: " + std::to_string(processor.stepCount), dontSendNotification);
	leftHeelValue.setText("Left Heel: " + std::to_string(processor.heel_L_in), dontSendNotification);
	leftM1Value.setText("Left M1: " + std::to_string(processor.m1_L_in), dontSendNotification);
	leftToeValue.setText("Left Toe: " + std::to_string(processor.toe_L_in), dontSendNotification);
	rightHeelValue.setText("Right Heel: " + std::to_string(processor.heel_R_in), dontSendNotification);
	rightM1Value.setText("Right M1: " + std::to_string(processor.m1_R_in), dontSendNotification);
	rightToeValue.setText("Right Toe: " + std::to_string(processor.toe_R_in), dontSendNotification);

	Baseline1.setText("B1: " + std::to_string(processor.baseline_globalParams[0]),dontSendNotification);
	Baseline2.setText("B2: " + std::to_string(processor.baseline_globalParams[1]), dontSendNotification);
	Baseline3.setText("B3: " + std::to_string(processor.baseline_globalParams[2]), dontSendNotification);
	Baseline4.setText("B4: " + std::to_string(processor.baseline_globalParams[3]), dontSendNotification);
	Baseline5.setText("B5: " + std::to_string(processor.baseline_globalParams[4]), dontSendNotification);

	Desired1.setText("D1: " + std::to_string(processor.desired_GlobalParams[0]), dontSendNotification);
	Desired2.setText("D2: " + std::to_string(processor.desired_GlobalParams[1]), dontSendNotification);
	Desired3.setText("D3: " + std::to_string(processor.desired_GlobalParams[2]), dontSendNotification);
	Desired4.setText("D4: " + std::to_string(processor.desired_GlobalParams[3]), dontSendNotification);
	Desired5.setText("D5: " + std::to_string(processor.desired_GlobalParams[4]), dontSendNotification);

	Current1.setText("C1: " + std::to_string(processor.current_GlobalParams[0]), dontSendNotification);
	Current2.setText("C2: " + std::to_string(processor.current_GlobalParams[1]), dontSendNotification);
	Current3.setText("C3: " + std::to_string(processor.current_GlobalParams[2]), dontSendNotification);
	Current4.setText("C4: " + std::to_string(processor.current_GlobalParams[3]), dontSendNotification);
	Current5.setText("C5: " + std::to_string(processor.current_GlobalParams[4]), dontSendNotification);

}

void RhythmSynthAudioProcessorEditor::updateMeanValues()
{
	stepDuration_Mean_Long.setText("Mean Step Duration: " + std::to_string(processor.stepDuration_Mean_Long) + " sec", dontSendNotification);
	stepDuration_Mean_Long_L.setText("Left Mean Step Duration: " + std::to_string(processor.stepDuration_Mean_Long_L) + " sec", dontSendNotification);
	stepDuration_Mean_Long_R.setText("Right Mean Step Duration: " + std::to_string(processor.stepDuration_Mean_Long_R) + " sec", dontSendNotification);
	stepDuration_Mean_Short.setText("Short Term Mean Step Duration: " + std::to_string(processor.stepDuration_Mean_Short) + " sec", dontSendNotification);

	stanceDuration_Mean_Long.setText("Mean Stance Time: " + std::to_string(processor.stanceDuration_Mean_Long) + " sec", dontSendNotification);
	swingDuration_Mean_Long.setText("Mean Swing Time: " + std::to_string(processor.swingDuration_Mean_Long) + " sec", dontSendNotification);
	stanceDuration_Mean_Long_L.setText("Mean Left Stance Time: " + std::to_string(processor.stanceDuration_Mean_Long_L) + " sec", dontSendNotification);
	stanceDuration_Mean_Long_R.setText("Mean Right Stance Time: " + std::to_string(processor.stanceDuration_Mean_Long_R) + " sec", dontSendNotification);
	swingDuration_Mean_Long_L.setText("Mean Left Swing Time: " + std::to_string(processor.swingDuration_Mean_Long_L) + " sec", dontSendNotification);
	swingDuration_Mean_Long_R.setText("Mean Right Swing Time: " + std::to_string(processor.swingDuration_Mean_Long_R) + " sec", dontSendNotification);
	stanceDuration_Mean_Short_L.setText("Mean Short-term Left Stance Time: " + std::to_string(processor.stanceDuration_Mean_Short_L) + " sec", dontSendNotification);
	stanceDuration_Mean_Short_R.setText("Mean Short-term Right Stance Time: " + std::to_string(processor.stanceDuration_Mean_Short_R) + " sec", dontSendNotification);
	swingDuration_Mean_Short_L.setText("Mean Short-term Left Swing Time: " + std::to_string(processor.swingDuration_Mean_Short_L) + " sec", dontSendNotification);
	swingDuration_Mean_Short_R.setText("Mean Short-term Right Swing Time: " + std::to_string(processor.swingDuration_Mean_Short_R) + " sec", dontSendNotification);

	s1_Duration_Mean_Long.setText("Long Term Mean H Support Time: " + std::to_string(processor.supportTime_s1_Mean_Long), dontSendNotification);
	s1_Duration_Mean_Short.setText("Short Term Mean H Support Time: " + std::to_string(processor.supportTime_s1_Mean_Short), dontSendNotification);

	s2_Duration_Mean_Long.setText("Long Term Mean M1 Support Time: " + std::to_string(processor.supportTime_s2_Mean_Long), dontSendNotification);
	s2_Duration_Mean_Short.setText("Short Term Mean M1 Support Time: " + std::to_string(processor.supportTime_s2_Mean_Short), dontSendNotification);

	s3_Duration_Mean_Long.setText("Long Term Mean T Support Time: " + std::to_string(processor.supportTime_s3_Mean_Long), dontSendNotification);
	s3_Duration_Mean_Short.setText("Short Term Mean T Support Time: " + std::to_string(processor.supportTime_s3_Mean_Short), dontSendNotification);

	s12_Duration_Mean_Long.setText("Long Term Mean H + M1 Support Time: " + std::to_string(processor.supportTime_s12_Mean_Long), dontSendNotification);
	s12_Duration_Mean_Short.setText("Short Term Mean H + M1 Support Time: " + std::to_string(processor.supportTime_s12_Mean_Short), dontSendNotification);

	s23_Duration_Mean_Long.setText("Long Term Mean M1 + T Support Time: " + std::to_string(processor.supportTime_s23_Mean_Long), dontSendNotification);
	s23_Duration_Mean_Short.setText("Short Term Mean M1 + T Support Time: " + std::to_string(processor.supportTime_s23_Mean_Short), dontSendNotification);

	s13_Duration_Mean_Long.setText("Long Term Mean H + T Support Time: " + std::to_string(processor.supportTime_s13_Mean_Long), dontSendNotification);
	s13_Duration_Mean_Short.setText("Short Term Mean H + T Support Time: " + std::to_string(processor.supportTime_s13_Mean_Short), dontSendNotification);

	s123_Duration_Mean_Long.setText("Long Term Mean Flat Foot Time: " + std::to_string(processor.supportTime_s123_Mean_Long), dontSendNotification);
	s123_Duration_Mean_Short.setText("Short Term Mean Flat Foot Time: " + std::to_string(processor.supportTime_s123_Mean_Short), dontSendNotification);
}

void RhythmSynthAudioProcessorEditor::updateCoVValues()
{
	stepDuration_CoV_Long.setText("Coefficient of Variation: " + std::to_string(processor.stepDuration_CoV_Long), dontSendNotification);
	
	stepDuration_CoV_Short.setText("Short Term Coefficient of Variation: " + std::to_string(processor.stepDuration_CoV_Short), dontSendNotification);	

	swingDuration_CoV_Long.setText("Overall Swing Time Variability: " + std::to_string(processor.swingDuration_CoV_Long), dontSendNotification);
	stanceDuration_CoV_Long.setText("Overall Stance Time Variability: " + std::to_string(processor.stanceDuration_CoV_Long), dontSendNotification);
	swingDuration_CoV_Short.setText("Short Term Swing Time Variability: " + std::to_string(processor.swingDuration_CoV_Short), dontSendNotification);
	stanceDuration_CoV_Short.setText("Short Term Stance Time Variability: " + std::to_string(processor.stanceDuration_CoV_Short), dontSendNotification);

	s1_Duration_CoV_Long.setText("Long Term CoV H Support Time: " + std::to_string(processor.supportTime_s1_CoV_Long), dontSendNotification);
	s1_Duration_CoV_Short.setText("Short Term CoV H Support Time: " + std::to_string(processor.supportTime_s1_CoV_Short), dontSendNotification);

	s2_Duration_CoV_Long.setText("Long Term CoV M1 Support Time: " + std::to_string(processor.supportTime_s2_CoV_Long), dontSendNotification);
	s2_Duration_CoV_Short.setText("Short Term CoV M1 Support Time: " + std::to_string(processor.supportTime_s2_CoV_Short), dontSendNotification);

	s3_Duration_CoV_Long.setText("Long Term CoV T Support Time: " + std::to_string(processor.supportTime_s3_CoV_Long), dontSendNotification);
	s3_Duration_CoV_Short.setText("Short Term CoV T Support Time: " + std::to_string(processor.supportTime_s3_CoV_Short), dontSendNotification);

	s12_Duration_CoV_Long.setText("Long Term CoV H + M1 Support Time: " + std::to_string(processor.supportTime_s12_CoV_Long), dontSendNotification);
	s12_Duration_CoV_Short.setText("Short Term CoV H + M1 Support Time: " + std::to_string(processor.supportTime_s12_CoV_Short), dontSendNotification);

	s23_Duration_CoV_Long.setText("Long Term CoV M1 + T Support Time: " + std::to_string(processor.supportTime_s23_CoV_Long), dontSendNotification);
	s23_Duration_CoV_Short.setText("Short Term CoV M1 + T Support Time: " + std::to_string(processor.supportTime_s23_CoV_Short), dontSendNotification);

	s13_Duration_CoV_Long.setText("Long Term CoV H + T Support Time: " + std::to_string(processor.supportTime_s13_CoV_Long), dontSendNotification);
	s13_Duration_CoV_Short.setText("Short Term CoV H + T Support Time: " + std::to_string(processor.supportTime_s13_CoV_Short), dontSendNotification);

	s123_Duration_CoV_Long.setText("Long Term CoV Flat Foot Time: " + std::to_string(processor.supportTime_s123_CoV_Long), dontSendNotification);
	s123_Duration_CoV_Short.setText("Short Term CoV Flat Foot Time: " + std::to_string(processor.supportTime_s123_CoV_Short), dontSendNotification);

	gaitRolloverPattern.setText("Latest Rollover Pattern", dontSendNotification);
	displayRecentRollover();
}

void RhythmSynthAudioProcessorEditor::updateAsymmetryValues()
{
	stepDuration_Asymmetry_Long.setText("Overall Step Time Asymmetry: " + std::to_string(processor.stepDuration_Asymmetry_Long), dontSendNotification);
	swingDuration_Asymmetry_Long.setText("Overall Swing Time Asymmetry: " + std::to_string(processor.swingDuration_Asymmetry_Long), dontSendNotification);
	stanceDuration_Asymmetry_Long.setText("Overall Stance Time Asymmetry: " + std::to_string(processor.stanceDuration_Asymmetry_Long), dontSendNotification);
	stepDuration_Asymmetry_Short.setText("Short Term Step Time Asymmetry: " + std::to_string(processor.stepDuration_Asymmetry_Short), dontSendNotification);
	swingDuration_Asymmetry_Short.setText("Short Term Swing Time Asymmetry: " + std::to_string(processor.swingDuration_Asymmetry_Short), dontSendNotification);
	stanceDuration_Asymmetry_Short.setText("Short Term Stance Time Asymmetry: " + std::to_string(processor.stanceDuration_Asymmetry_Short), dontSendNotification);
	s1_Duration_Asymmetry_Long.setText("Long Term Asymmetry H Support Time: " + std::to_string(processor.s1_Asymmetry_Long), dontSendNotification);
	s1_Duration_Asymmetry_Short.setText("Short Term Asymmetry H Support Time: " + std::to_string(processor.s1_Asymmetry_Short), dontSendNotification);

	s2_Duration_Asymmetry_Long.setText("Long Term Asymmetry M1 Support Time: " + std::to_string(processor.s2_Asymmetry_Long), dontSendNotification);
	s2_Duration_Asymmetry_Short.setText("Short Term Asymmetry M1 Support Time: " + std::to_string(processor.s2_Asymmetry_Short), dontSendNotification);

	s3_Duration_Asymmetry_Long.setText("Long Term Asymmetry T Support Time: " + std::to_string(processor.s3_Asymmetry_Long), dontSendNotification);
	s3_Duration_Asymmetry_Short.setText("Short Term Asymmetry T Support Time: " + std::to_string(processor.s3_Asymmetry_Short), dontSendNotification);

	s12_Duration_Asymmetry_Long.setText("Long Term Asymmetry H + M1 Support Time: " + std::to_string(processor.s12_Asymmetry_Long), dontSendNotification);
	s12_Duration_Asymmetry_Short.setText("Short Term Asymmetry H + M1 Support Time: " + std::to_string(processor.s12_Asymmetry_Short), dontSendNotification);

	s23_Duration_Asymmetry_Long.setText("Long Term Asymmetry M1 + T Support Time: " + std::to_string(processor.s23_Asymmetry_Long), dontSendNotification);
	s23_Duration_Asymmetry_Short.setText("Short Term Asymmetry M1 + T Support Time: " + std::to_string(processor.s23_Asymmetry_Short), dontSendNotification);

	s13_Duration_Asymmetry_Long.setText("Long Term Asymmetry H + T Support Time: " + std::to_string(processor.s13_Asymmetry_Long), dontSendNotification);
	s13_Duration_Asymmetry_Short.setText("Short Term Asymmetry H + T Support Time: " + std::to_string(processor.s13_Asymmetry_Short), dontSendNotification);

	s123_Duration_Asymmetry_Long.setText("Long Term Asymmetry Flat Foot Time: " + std::to_string(processor.s123_Asymmetry_Long), dontSendNotification);
	s123_Duration_Asymmetry_Short.setText("Short Term Asymmetry Flat Foot Time: " + std::to_string(processor.s123_Asymmetry_Short), dontSendNotification);
}

void RhythmSynthAudioProcessorEditor::checkCoVPerformance()
{
	isBetter_Step_Cov = checkWithBaseline(processor.stepDuration_CoV_Short,processor.baseline_stepDuration_CoV,true);
	isBetter_Stance_Cov = checkWithBaseline(processor.stanceDuration_CoV_Short,processor.baseline_stanceDuration_CoV,true);
	isBetter_Swing_Cov = checkWithBaseline(processor.swingDuration_CoV_Short, processor.baseline_swingDuration_CoV, true);

}

void RhythmSynthAudioProcessorEditor::checkAsymmetryPerformance()
{
	isBetter_Step_Asymmetry = checkWithBaseline(processor.stepDuration_Asymmetry_Short, processor.baseline_stepDuration_Asymmetry, true);
	isBetter_Stance_Asymmetry = checkWithBaseline(processor.stanceDuration_Asymmetry_Short, processor.baseline_stanceDuration_Asymmetry, true);
	isBetter_Swing_Asymmetry = checkWithBaseline(processor.swingDuration_Asymmetry_Short, processor.baseline_swingDuration_Asymmetry, true);
}

void RhythmSynthAudioProcessorEditor::colourCoVLabels()
{
	if (isBetter_Step_Cov)
		stepDuration_CoV_Short.setColour(stepDuration_CoV_Short.textColourId, Colours::lightgreen);
	else
		stepDuration_CoV_Short.setColour(stepDuration_CoV_Short.textColourId, Colours::red);

	if (isBetter_Stance_Cov)
		stanceDuration_CoV_Short.setColour(stanceDuration_CoV_Short.textColourId, Colours::lightgreen);
	else
		stanceDuration_CoV_Short.setColour(stanceDuration_CoV_Short.textColourId, Colours::red);

	if (isBetter_Swing_Cov)
		swingDuration_CoV_Short.setColour(swingDuration_CoV_Short.textColourId, Colours::lightgreen);
	else
		swingDuration_CoV_Short.setColour(swingDuration_CoV_Short.textColourId, Colours::red);
}

void RhythmSynthAudioProcessorEditor::colourAsymmetryLabels()
{
	if (isBetter_Step_Asymmetry)
		stepDuration_Asymmetry_Short.setColour(stepDuration_Asymmetry_Short.textColourId, Colours::lightgreen);
	else
		stepDuration_Asymmetry_Short.setColour(stepDuration_Asymmetry_Short.textColourId, Colours::red);

	if (isBetter_Stance_Asymmetry)
		stanceDuration_Asymmetry_Short.setColour(stanceDuration_Asymmetry_Short.textColourId, Colours::lightgreen);
	else
		stanceDuration_Asymmetry_Short.setColour(stanceDuration_Asymmetry_Short.textColourId, Colours::red);

	if (isBetter_Swing_Asymmetry)
		swingDuration_Asymmetry_Short.setColour(swingDuration_Asymmetry_Short.textColourId, Colours::lightgreen);
	else
		swingDuration_Asymmetry_Short.setColour(swingDuration_Asymmetry_Short.textColourId, Colours::red);
}

void RhythmSynthAudioProcessorEditor::updateValues(int currentTab)
{
	

	updateGlobalValues();

	switch (currentTab)
	{
	case 0:
		updateMeanValues();
		break;
	case 1:
		updateCoVValues();
		checkCoVPerformance();
		colourCoVLabels();
		break;
	case 2:
		updateAsymmetryValues();
		checkAsymmetryPerformance();
		colourAsymmetryLabels();
		break;
	}
}

void RhythmSynthAudioProcessorEditor::displayRecentRollover()
{
	gaitRolloverLast_L.setText("L: " + processor.lastRollover_L, dontSendNotification);

	if (processor.lastRollover_L == "147538")
	{
		gaitRolloverLast_L.setColour(gaitRolloverLast_L.textColourId,Colours::lightgreen);
	}
	else
	{
		gaitRolloverLast_L.setColour(gaitRolloverLast_L.textColourId, Colours::red);
	}

	gaitRolloverLast_R.setText("R: " + processor.lastRollover_R, dontSendNotification);

	if (processor.lastRollover_R == "147538")
	{
		gaitRolloverLast_R.setColour(gaitRolloverLast_R.textColourId, Colours::lightgreen);
	}
	else
	{
		gaitRolloverLast_R.setColour(gaitRolloverLast_R.textColourId, Colours::red);
	}
}

void RhythmSynthAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("", getLocalBounds(), Justification::centred, 1);
}

void RhythmSynthAudioProcessorEditor::positionMeanLabels()
{
	stepDuration_Mean_Long.setBounds(80, 300, 500, 160);
	stepDuration_Mean_Long_L.setBounds(80, 320, 500, 160);
	stepDuration_Mean_Long_R.setBounds(80, 340, 500, 160);
	stepDuration_Mean_Short.setBounds(80, 360, 500, 160);
	stepDuration_Mean_Short_L.setBounds(80, 380, 500, 160);
	stepDuration_Mean_Short_R.setBounds(80, 400, 500, 160);
	stanceDuration_Mean_Long.setBounds(80, 420, 500, 160);
	stanceDuration_Mean_Long_L.setBounds(80, 440, 500, 160);
	stanceDuration_Mean_Long_R.setBounds(80, 460, 500, 160);
	stanceDuration_Mean_Short.setBounds(80, 480, 500, 160);
	stanceDuration_Mean_Short_L.setBounds(80, 500, 500, 160);
	stanceDuration_Mean_Short_R.setBounds(80, 520, 500, 160);
	swingDuration_Mean_Long.setBounds(80, 540, 500, 160);
	swingDuration_Mean_Long_L.setBounds(80, 560, 500, 160);
	swingDuration_Mean_Long_R.setBounds(80, 580, 500, 160);
	swingDuration_Mean_Short.setBounds(80, 600, 500, 160);
	swingDuration_Mean_Short_L.setBounds(80, 620, 500, 160);
	swingDuration_Mean_Short_R.setBounds(80, 640, 500, 160);	

	s1_Duration_Mean_Long.setBounds(580, 300, 500, 160);	
	s1_Duration_Mean_Short.setBounds(580, 320, 500, 160);

	s2_Duration_Mean_Long.setBounds(580, 360, 500, 160);
	s2_Duration_Mean_Short.setBounds(580, 380, 500, 160);

	s3_Duration_Mean_Long.setBounds(580, 420, 500, 160);
	s3_Duration_Mean_Short.setBounds(580, 440, 500, 160);

	s12_Duration_Mean_Long.setBounds(580, 460, 500, 160);
	s12_Duration_Mean_Short.setBounds(580, 480, 500, 160);

	s23_Duration_Mean_Long.setBounds(580, 520, 500, 160);
	s23_Duration_Mean_Short.setBounds(580, 540, 500, 160);

	s13_Duration_Mean_Long.setBounds(580, 580, 500, 160);
	s13_Duration_Mean_Short.setBounds(580, 600, 500, 160);

	s123_Duration_Mean_Long.setBounds(580, 640, 500, 160);
	s123_Duration_Mean_Short.setBounds(580, 660, 500, 160);
	
}

void RhythmSynthAudioProcessorEditor::positionCoVLabels()
{
	stepDuration_CoV_Long.setBounds(80, 300, 500, 160);
	stepDuration_CoV_Short.setBounds(80, 320, 500, 160);
	stanceDuration_CoV_Long.setBounds(80, 340, 500, 160);
	stanceDuration_CoV_Short.setBounds(80, 360, 500, 160);
	swingDuration_CoV_Long.setBounds(80, 380, 500, 160);
	swingDuration_CoV_Short.setBounds(80, 400, 500, 160);

	gaitRolloverPattern.setBounds(80,420,500,160);
	gaitRolloverLast_L.setBounds(80, 440, 500, 160);
	gaitRolloverLast_R.setBounds(80, 460, 500, 160);

	s1_Duration_CoV_Long.setBounds(580, 300, 500, 160);
	s1_Duration_CoV_Short.setBounds(580, 320, 500, 160);

	s2_Duration_CoV_Long.setBounds(580, 360, 500, 160);
	s2_Duration_CoV_Short.setBounds(580, 380, 500, 160);

	s3_Duration_CoV_Long.setBounds(580, 420, 500, 160);
	s3_Duration_CoV_Short.setBounds(580, 440, 500, 160);

	s12_Duration_CoV_Long.setBounds(580, 460, 500, 160);
	s12_Duration_CoV_Short.setBounds(580, 480, 500, 160);

	s23_Duration_CoV_Long.setBounds(580, 520, 500, 160);
	s23_Duration_CoV_Short.setBounds(580, 540, 500, 160);

	s13_Duration_CoV_Long.setBounds(580, 580, 500, 160);
	s13_Duration_CoV_Short.setBounds(580, 600, 500, 160);

	s123_Duration_CoV_Long.setBounds(580, 640, 500, 160);
	s123_Duration_CoV_Short.setBounds(580, 660, 500, 160);
}

void RhythmSynthAudioProcessorEditor::positionAsymmetryLabels()
{
	stepDuration_Asymmetry_Long.setBounds(80, 300, 500, 160);
	stepDuration_Asymmetry_Short.setBounds(80, 320, 500, 160);
	stanceDuration_Asymmetry_Long.setBounds(80, 340, 500, 160);
	stanceDuration_Asymmetry_Short.setBounds(80, 360, 500, 160);
	swingDuration_Asymmetry_Long.setBounds(80, 380, 500, 160);
	swingDuration_Asymmetry_Short.setBounds(80, 400, 500, 160);

	s1_Duration_Asymmetry_Long.setBounds(580, 300, 500, 160);
	s1_Duration_Asymmetry_Short.setBounds(580, 320, 500, 160);

	s2_Duration_Asymmetry_Long.setBounds(580, 360, 500, 160);
	s2_Duration_Asymmetry_Short.setBounds(580, 380, 500, 160);

	s3_Duration_Asymmetry_Long.setBounds(580, 420, 500, 160);
	s3_Duration_Asymmetry_Short.setBounds(580, 440, 500, 160);

	s12_Duration_Asymmetry_Long.setBounds(580, 460, 500, 160);
	s12_Duration_Asymmetry_Short.setBounds(580, 480, 500, 160);

	s23_Duration_Asymmetry_Long.setBounds(580, 520, 500, 160);
	s23_Duration_Asymmetry_Short.setBounds(580, 540, 500, 160);

	s13_Duration_Asymmetry_Long.setBounds(580, 580, 500, 160);
	s13_Duration_Asymmetry_Short.setBounds(580, 600, 500, 160);

	s123_Duration_Asymmetry_Long.setBounds(580, 640, 500, 160);
	s123_Duration_Asymmetry_Short.setBounds(580, 660, 500, 160);
}

void RhythmSynthAudioProcessorEditor::positionGlobalLabels()
{
	timeElapsedLabel.setBounds(80, 30, 500, 20);
	onOffButton.setBounds(80, 100, 30, 90);
	storeMeasurements.setBounds(880,125,100,30);
	stepInputMode.setBounds(580, 100, 30, 40);
	usabilityTestMode.setBounds(580, 140, 30, 40);
	stepCountLabel.setBounds(80, 280, 100, 160);
	leftHeelValue.setBounds(80, 160, 500, 160);
	leftM1Value.setBounds(80, 180, 500, 160);
	leftToeValue.setBounds(80, 200, 500, 160);

	heelEnabled.setBounds(50, 230, 30, 20);
	m1Enabled.setBounds(50, 250, 30, 20);
	toeEnabled.setBounds(50, 270, 30, 20);

	rightHeelValue.setBounds(580, 160, 500, 160);
	rightM1Value.setBounds(580, 180, 500, 160);
	rightToeValue.setBounds(580, 200, 500, 160);
}

void RhythmSynthAudioProcessorEditor::positionSynthControlLabels()
{
	autoCadenceDetect.setBounds(80, 480, 150, 30);
	halfTime.setBounds(230, 480, 150, 30);
	desiredCadencePercent.setBounds(80, 520, 400, 30);
	stepOscFreq.setBounds(80, 400, 400, 30);
	stepSizeWindow.setBounds(80, 430, 400, 30);

	DesiredChange1.setBounds(80, 590, 500, 30);
	DesiredChange2.setBounds(80, 620, 500, 30);
	DesiredChange3.setBounds(80, 650, 500, 30);
	DesiredChange4.setBounds(80, 680, 500, 30);
	DesiredChange5.setBounds(80, 710, 500, 30);

	AudioParameters.setBounds(640,530,200,30);
	MappingType.setBounds(840,560,200,30);
	NumQuantizations.setBounds(400, 760, 150, 30);

	AP1.setBounds(600, 560, 50, 30);
	T11.setBounds(600, 590, 30, 30);
	T21.setBounds(600, 620, 30, 30);
	T31.setBounds(600, 650, 30, 30);
	T41.setBounds(600, 680, 30, 30);
	T51.setBounds(600, 710, 30, 30);
	NQ1.setBounds(600, 760, 30, 30);

	AP2.setBounds(640, 560, 50, 30);
	T12.setBounds(640, 590, 30, 30);
	T22.setBounds(640, 620, 30, 30);
	T32.setBounds(640, 650, 30, 30);
	T42.setBounds(640, 680, 30, 30);
	T52.setBounds(640, 710, 30, 30);
	NQ2.setBounds(640, 760, 30, 30);

	AP3.setBounds(680, 560, 50, 30);
	T13.setBounds(680, 590, 30, 30);
	T23.setBounds(680, 620, 30, 30);
	T33.setBounds(680, 650, 30, 30);
	T43.setBounds(680, 680, 30, 30);
	T53.setBounds(680, 710, 30, 30);
	NQ3.setBounds(680, 760, 30, 30);

	AP4.setBounds(720, 560, 50, 30);
	T14.setBounds(720, 590, 30, 30);
	T24.setBounds(720, 620, 30, 30);
	T34.setBounds(720, 650, 30, 30);
	T44.setBounds(720, 680, 30, 30);
	T54.setBounds(720, 710, 30, 30);
	NQ4.setBounds(720, 760, 30, 30);

	AP5.setBounds(760, 560, 50, 30);
	T15.setBounds(760, 590, 30, 30);
	T25.setBounds(760, 620, 30, 30);
	T35.setBounds(760, 650, 30, 30);
	T45.setBounds(760, 680, 30, 30);
	T55.setBounds(760, 710, 30, 30);
	NQ5.setBounds(760, 760, 30, 30);

	MT1.setBounds(800, 590, 30, 30);
	MT2.setBounds(800, 620, 30, 30);
	MT3.setBounds(800, 650, 30, 30);
	MT4.setBounds(800, 680, 30, 30);
	MT5.setBounds(800, 710, 30, 30);

	Baseline1.setBounds(850, 590, 100, 30);
	Baseline2.setBounds(850, 620, 100, 30);
	Baseline3.setBounds(850, 650, 100, 30);
	Baseline4.setBounds(850, 680, 100, 30);
	Baseline5.setBounds(850, 710, 100, 30);

	Desired1.setBounds(950, 590, 100, 30);
	Desired2.setBounds(950, 620, 100, 30);
	Desired3.setBounds(950, 650, 100, 30);
	Desired4.setBounds(950, 680, 100, 30);
	Desired5.setBounds(950, 710, 100, 30);

	Current1.setBounds(1050, 590, 100, 30);
	Current2.setBounds(1050, 620, 100, 30);
	Current3.setBounds(1050, 650, 100, 30);
	Current4.setBounds(1050, 680, 100, 30);
	Current5.setBounds(1050, 710, 100, 30);

	stepTimeCoVOffset.setBounds(680, 330, 300,30);
	swingAsymmOffset.setBounds(680, 360, 300, 30); 
	flatFootAsymmOffset.setBounds(680, 390, 300, 30); 
	stepTimeAsymmOffset.setBounds(680, 420, 300, 30);
}

bool RhythmSynthAudioProcessorEditor::checkWithBaseline(float currentVar, float baselineVar, bool shouldBeLess)
{
	bool isLess = (abs(currentVar) <= abs(baselineVar));
	if (!(isLess ^ shouldBeLess))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void RhythmSynthAudioProcessorEditor::resized()
{
	positionGlobalLabels();
	positionMeanLabels();
	positionCoVLabels();
	positionAsymmetryLabels();	
	positionSynthControlLabels();
}

void RhythmSynthAudioProcessorEditor::timerCallback()
{
	int currentTab = tabs->getCurrentTabIndex();
	redrawUI(currentTab);
	updateValues(currentTab);
}
