#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include "BiQuad.h"

#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */                
#define M_PI		          3.141592653589793238462643383279502884L /* pi */

class GaitAnalysis
{
public:
	// Instantaneous Force Sample Values

	//L
	float heel_L;
	float m1_L;
	float toe_L;

	//R
	float heel_R;
	float m1_R;
	float toe_R;

	// Is Contact?

	//L
	bool isHeelContact_L = false;
	bool isM1Contact_L = false;
	bool isToeContact_L = false;

	//R
	bool isHeelContact_R = false;
	bool isM1Contact_R = false;
	bool isToeContact_R = false;

	// Previous State

	//L
	bool isHeelContact_L_z1 = false;
	bool isM1Contact_L_z1 = false;
	bool isToeContact_L_z1 = false;

	//R
	bool isHeelContact_R_z1 = false;
	bool isM1Contact_R_z1 = false;
	bool isToeContact_R_z1 = false;

	// State Change

	bool isHeelChange_L = false;
	bool isHeelChange_R = false;
	bool isM1Change_L = false;
	bool isM1Change_R = false;
	bool isToeChange_L = false;
	bool isToeChange_R = false;
	bool isPhaseChange_L = false;
	bool isPhaseChange_R = false;
	bool isContactConditionChange_L = false;
	bool isContactConditionChange_R = false;
		
	//========================================================================================================

	float longestStep = 4.0; //Longest Possible Step Before Considering Pause
	int stepSizeWindow = 4; //Short Term Measurements - Window Length - Parameterize?

	// Step Measurement Variables=============================================================================

	// Step Count

	int stepCount = 0;
	int stepCount_L = 0;
	int stepCount_R = 0;

	// Stance Time============================================================================================

	//Current
	float currentStanceDuration_L = 0.0;
	float currentStanceDuration_R = 0.0;

	//Arrays for Long Term Storage
	float stanceDurationArrayLong[10000] = { 0.0 };
	float stanceDurationArrayLong_L[10000] = { 0.0 };
	float stanceDurationArrayLong_R[10000] = { 0.0 };

	//Arrays for Short Term Storage
	float stanceDurationArrayShort[10] = { 0.0 };
	float stanceDurationArrayShort_L[5] = { 0.0 };
	float stanceDurationArrayShort_R[5] = { 0.0 };

	
	// Swing Time ============================================================================================

	//Current
	float currentSwingDuration_L = 0.0;
	float currentSwingDuration_R = 0.0;

	//Arrays for Long Term Storage
	float swingDurationArrayLong[10000] = { 0.0 };
	float swingDurationArrayLong_L[10000] = { 0.0 };
	float swingDurationArrayLong_R[10000] = { 0.0 };

	//Arrays for Short Term Storage
	float swingDurationArrayShort[10] = { 0.0 };
	float swingDurationArrayShort_L[5] = { 0.0 };
	float swingDurationArrayShort_R[5] = { 0.0 };

	//Step====================================================================================================

	//Arrays

	//Arrays for Long Term Storage
	float currentStepDuration = 0.0;
	float stepDurationArrayLong[10000] = { 0.0 };
	float stepDurationArrayLong_L[10000] = { 0.0 };
	float stepDurationArrayLong_R[10000] = { 0.0 };

	//Arrays for Short Term Storage
	float stepDurationArrayShort[10] = { 0.0 };
	float stepDurationArrayShort_L[5] = { 0.0 };
	float stepDurationArrayShort_R[5] = { 0.0 };

//===============================P E R F O R M A N C E  M E A S U R E S========================================

	// =========================================== MEANS = = = = = = = = =  = = = = == = = = = = = = = = = = = 
	
														//Step

	//Long
	float stepDuration_Mean_Long = 0.0;
	float stepDuration_Mean_Long_L = 0.0;
	float stepDuration_Mean_Long_R = 0.0;

	//Short
	float stepDuration_Mean_Short = 0.0;
	float stepDuration_Mean_Short_L = 0.0;
	float stepDuration_Mean_Short_R = 0.0;

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

	// ==================================================== CoV = = = = = = = = = = = = = = = = == = = = = = = = = =

	//Step
	float stepDuration_CoV_Short = 0.0;
	float stepDuration_CoV_Long = 0.0;

	//Stance

	float stanceDuration_CoV_Short = 0.0;
	float stanceDuration_CoV_Long = 0.0;

	//Swing

	float swingDuration_CoV_Short = 0.0;
	float swingDuration_CoV_Long = 0.0;
	
	//============================================== A S Y M M E T R Y =========================================
	
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

	
	//=============================================== Checking Flags =========================================

	//++++++++++++++++++++++++++++++++++++++++++++++++ Minima ++++++++++++++++++++++++++++++++++++++++++++++++
	
	//Heel
	bool isHeelMinima_L = false;
	bool isHeelMinima_R = false;
	
	bool isHeelMinima_L_z1 = false;
	bool isHeelMinima_R_z1 = false;

	//M1
	bool isM1Minima_L = false;
	bool isM1Minima_R = false;

	bool isM1Minima_L_z1 = false;
	bool isM1Minima_R_z1 = false;

	//Toe
	bool isToeMinima_L = false;
	bool isToeMinima_R = false;

	bool isToeMinima_L_z1 = false;
	bool isToeMinima_R_z1 = false;

	//+++++++++++++++++++++++++++++++++++++++++++++++++ Maxima +++++++++++++++++++++++++++++++++++++++++++++++

	//Heel
	bool isHeelMaxima_L = false;
	bool isHeelMaxima_R = false;

	bool isHeelMaxima_L_z1 = false;
	bool isHeelMaxima_R_z1 = false;

	//M1
	bool isM1Maxima_L = false;
	bool isM1Maxima_R = false;

	bool isM1Maxima_L_z1 = false;
	bool isM1Maxima_R_z1 = false;

	//Toe
	bool isToeMaxima_L = false;
	bool isToeMaxima_R = false;

	bool isToeMaxima_L_z1 = false;
	bool isToeMaxima_R_z2 = false;

	// ============================================== Foot State =============================================
	
	bool lastFootstepDetected = false; // false = L, true = R

	bool isStance_L = false;
	bool isStance_L_z1 = false;
	bool isStance_R = false;
	bool isStance_R_z1 = false;
	bool isSwing_L = false;
	bool isSwing_R = false;

	// ============================================== Rollover Characteristics ===============================

	// ============================================ Pattern String Arrays ====================================

	std::string stepSequence = "";
	std::string currentRolloverPattern = "";
	std::string rolloverPatternShort_L[10] = { "" };
	std::string rolloverPatternShort_R[10] = { "" };
	std::string rolloverPatternLong[10000] = {""};
	std::string rolloverPatternLong_L[10000] = { "" };
	std::string rolloverPatternLong_R[10000] = { "" };

	std::string lastRollover_L = "";
	std::string lastRollover_R = "";
	// ============================================ Onset Latencies ==========================================

	// Current Step
	float lat_12_current_L = 0;
	float lat_23_current_L = 0;
	float lat_13_current_L = 0;

	float lat_12_current_R = 0;
	float lat_23_current_R = 0;
	float lat_13_current_R = 0;

	// Short Term

	float lat_12_L_Short[10] = { 0.0 };
	float lat_23_L_Short[10] = { 0.0 };
	float lat_13_L_Short[10] = { 0.0 };
	float lat_12_R_Short[10] = { 0.0 };
	float lat_23_R_Short[10] = { 0.0 };
	float lat_13_R_Short[10] = { 0.0 };

	// Long Term

	float lat_12_L_Long[10000] = { 0.0 };
	float lat_23_L_Long[10000] = { 0.0 };
	float lat_13_L_Long[10000] = { 0.0 };
	float lat_12_R_Long[10000] = { 0.0 };
	float lat_23_R_Long[10000] = { 0.0 };
	float lat_13_R_Long[10000] = { 0.0 };
	
	// ============================================ Support Durations ========================================

	// Current Step

	/*float supportTime_s1_current = 0;
	float supportTime_s2_current = 0;
	float supportTime_s3_current = 0;

	float supportTime_s12_current = 0;
	float supportTime_s23_current = 0;
	
	float supportTime_s123_current = 0;*/

	float supportTime_currentState_L = 0;
	float supportTime_currentState_R = 0;

	// Short Term

	float supportTime_s1_short[10] = { 0 };
	float supportTime_s2_short[10] = { 0 };
	float supportTime_s3_short[10] = { 0 };

	float supportTime_s12_short[10] = { 0 };
	float supportTime_s23_short[10] = { 0 };
	float supportTime_s13_short[10] = { 0 };

	float supportTime_s123_short[10] = { 0 };

	//L - short - support

	float supportTime_s1_short_L[10] = { 0 };
	float supportTime_s2_short_L[10] = { 0 };
	float supportTime_s3_short_L[10] = { 0 };

	float supportTime_s12_short_L[10] = { 0 };
	float supportTime_s23_short_L[10] = { 0 };
	float supportTime_s13_short_L[10] = { 0 };

	float supportTime_s123_short_L[10] = { 0 };

	//R - short - support

	float supportTime_s1_short_R[10] = { 0 };
	float supportTime_s2_short_R[10] = { 0 };
	float supportTime_s3_short_R[10] = { 0 };

	float supportTime_s12_short_R[10] = { 0 };
	float supportTime_s23_short_R[10] = { 0 };
	float supportTime_s13_short_R[10] = { 0 };

	float supportTime_s123_short_R[10] = { 0 };

	// Long Term

	float supportTime_s1_long[10000] = { 0 };
	float supportTime_s2_long[10000] = { 0 };
	float supportTime_s3_long[10000] = { 0 };

	float supportTime_s12_long[10000] = { 0 };
	float supportTime_s23_long[10000] = { 0 };
	float supportTime_s13_long[10000] = { 0 };

	float supportTime_s123_long[10000] = { 0 };

	//Long - Support - L

	float supportTime_s1_long_L[10000] = { 0 };
	float supportTime_s2_long_L[10000] = { 0 };
	float supportTime_s3_long_L[10000] = { 0 };

	float supportTime_s12_long_L[10000] = { 0 };
	float supportTime_s23_long_L[10000] = { 0 };
	float supportTime_s13_long_L[10000] = { 0 };

	float supportTime_s123_long_L[10000] = { 0 };

	//Long - Support - R

	float supportTime_s1_long_R[10000] = { 0 };
	float supportTime_s2_long_R[10000] = { 0 };
	float supportTime_s3_long_R[10000] = { 0 };

	float supportTime_s12_long_R[10000] = { 0 };
	float supportTime_s23_long_R[10000] = { 0 };
	float supportTime_s13_long_R[10000] = { 0 };

	float supportTime_s123_long_R[10000] = { 0 };

	// =============================================== SUPPORT DURATION MEANS ================================

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

	// =============================================== MISC ==================================================
	
	float samplePeriod = 0.005;
	bool firstSwingDone = false; //To ignore first stance	
	bool dataStored = false;	
	float shortTermStanceSwingRatio = 0.0;
	int contactState_L = 0;
	int contactState_R = 0;
	int contactState_L_z1 = 0;
	int contactState_R_z1 = 0;
	int contactCondition_L = 0;
	int contactCondition_R = 0;
	int contactCondition_L_z1 = 0;
	int contactCondition_R_z1 = 0;

	// ================================================ B I Q U A D S ========================================

	BiQuad LPF_H_L;
	BiQuad LPF_M1_L;
	BiQuad LPF_T_L;

	BiQuad LPF_H_R;
	BiQuad LPF_M1_R;
	BiQuad LPF_T_R;

	// =============================================== FSR Input Normalization ===============================

	float heel_NormFactor = 1;
	float m1_NormFactor = 1;
	float toe_NormFactor = 1;

	// ================================================MEASUREMENT STORAGE====================================

	FILE *walkingParamMeasurements;

	// ==================================================FUNCTIONS============================================

	GaitAnalysis();
	~GaitAnalysis();
	void processForceSamples(float heel_L_in, float heel_R_in, float m1_L_in, 
							 float m1_R_in, float toe_L_in, float toe_R_in, bool sim);
	void normalizeInput(float heel_L_in, float heel_R_in, float m1_L_in,
		float m1_R_in, float toe_L_in, float toe_R_in);
	void filterAllSensors(float heel_L_in, float heel_R_in, float m1_L_in,
		float m1_R_in, float toe_L_in, float toe_R_in, bool sim);
	void checkContactState(bool sim);
	void checkForMinima(bool sim);
	void checkForMaxima(bool sim);
	void shuffleContactStateDelay();
	void checkContactStateChange();
	void handleGlobalContactStateChange();
	void handleLocalContactStateChange();
	void incrementDurationCounters();
	void storeDuration_calcMean_CoV(float duration, float *longDurationArray, float *longDurationArray_L,
		float *longDurationArray_R, float *shortDurationArray, float *shortDurationArray_L,
		float *shortDurationArray_R, int windowLength,
		int stepCount, int stepCount_L, int stepCount_R, bool LRswitch,
		float *meanVarLong, float *covVarLong, float *meanVarShort, float *covVarShort,
		float *meanVarL, float *meanVarR, float *meanVarL_Short, float *meanVarR_Short);
	void calcAsymmetry(float *asymmVarLong, float*asymmVarShort, float long_mean_L,
					   float long_mean_R, float short_mean_L, float short_mean_R);
	float calcArrayMean(float *arr, int count);
	float calcArraySTD(float *arr, float *mean, int count);
	int getContactCondition(bool heelContact, bool m1Contact, bool toeContact);
	void incrementRolloverSupportCounters();
	

};
