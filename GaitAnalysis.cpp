#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include "BiQuad.h"
#include "GaitAnalysis.h"

#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */                
#define M_PI		          3.141592653589793238462643383279502884L /* pi */

GaitAnalysis::GaitAnalysis()
{}

GaitAnalysis::~GaitAnalysis()
{}

void GaitAnalysis::processForceSamples(float heel_L_in,float heel_R_in,float m1_L_in,
									   float m1_R_in, float toe_L_in, float toe_R_in,bool sim)
{
	normalizeInput(heel_L_in, heel_R_in, m1_L_in, m1_R_in, toe_L_in, toe_R_in);
	filterAllSensors(heel_L_in, heel_R_in, m1_L_in, m1_R_in, toe_L_in, toe_R_in,sim);
	checkContactState(sim); // --> Also checks contact state change
	checkContactStateChange();
	incrementDurationCounters();
	incrementRolloverSupportCounters();

}

void GaitAnalysis::normalizeInput(float heel_L_in, float heel_R_in, float m1_L_in,
									float m1_R_in, float toe_L_in, float toe_R_in)
{
	heel_L = heel_L_in / heel_NormFactor;
	heel_R = heel_R_in / heel_NormFactor;
	m1_L = m1_L_in / m1_NormFactor;
	m1_R = m1_R_in / m1_NormFactor;
	toe_L = toe_L_in / toe_NormFactor;
	toe_R = toe_R_in / toe_NormFactor;
}

void GaitAnalysis::filterAllSensors(float heel_L_in, float heel_R_in, float m1_L_in,
	float m1_R_in, float toe_L_in, float toe_R_in, bool sim)
{
	if (sim)
	{
		//Left
		heel_L = LPF_H_L.doBiQuad(heel_L_in, 0.1);
		m1_L = LPF_M1_L.doBiQuad(m1_L_in, 0.1);
		toe_L = LPF_T_L.doBiQuad(toe_L_in, 0.1);

		//Right
		heel_R = LPF_H_R.doBiQuad(heel_R_in, 0.1);
		m1_R = LPF_M1_R.doBiQuad(m1_R_in, 0.1);
		toe_R = LPF_T_R.doBiQuad(toe_R_in, 0.1);
	}

	else
	{
		heel_L = heel_L_in;
		heel_R = heel_R_in;
		m1_L = m1_L_in;
		m1_R = m1_R_in;
		toe_L = toe_L_in;
		toe_R = toe_R_in;
	}
}

void GaitAnalysis::checkContactState(bool sim)
{
	checkForMinima(sim);
	checkForMaxima(sim);
	isStance_L = (isHeelContact_L || isM1Contact_L || isToeContact_L);
	isStance_R = (isHeelContact_R || isM1Contact_R || isToeContact_R);
	isSwing_L = !isStance_L;
	isSwing_R = !isStance_R;	
	
	contactCondition_L = getContactCondition(isHeelContact_L, isM1Contact_L, isToeContact_L);
	contactCondition_R = getContactCondition(isHeelContact_R, isM1Contact_R, isToeContact_R);
}

int GaitAnalysis::getContactCondition(bool heelContact, bool m1Contact, bool toeContact)
{
	if (heelContact && !m1Contact && !toeContact)
		return 1;
	if (!heelContact && m1Contact && !toeContact)
		return 2;
	if (!heelContact && !m1Contact && toeContact)
		return 3;
	if (heelContact && m1Contact && !toeContact)
		return 4;
	if (!heelContact && m1Contact && toeContact)
		return 5;
	if (heelContact && !m1Contact && toeContact)
		return 6;
	if (heelContact && m1Contact && toeContact) //Foot Flat
		return 7;
	if (!heelContact && !m1Contact && !toeContact) // Swing
		return 8;
}

void GaitAnalysis::checkContactStateChange()
{
	isHeelChange_L = (isHeelContact_L != isHeelContact_L_z1);
	isHeelChange_R = (isHeelContact_R != isHeelContact_R_z1);
	
	isM1Change_L = (isM1Contact_L != isM1Contact_L_z1);
	isM1Change_R = (isM1Contact_R != isM1Contact_R_z1);
	
	isToeChange_L = (isToeContact_L != isToeContact_L_z1);
	isToeChange_R = (isToeContact_R != isToeContact_R_z1);
	
	isContactConditionChange_L = (contactCondition_L != contactCondition_L_z1);
	isContactConditionChange_R = (contactCondition_R != contactCondition_R_z1);
	
	isPhaseChange_L = (isStance_L != isStance_L_z1);
	isPhaseChange_R = (isStance_R != isStance_R_z1);


	//If Global Contact State Changes, Recalculate Asymmetry and CoV Coefficients

	if (isPhaseChange_L || isPhaseChange_R)
	{
		handleGlobalContactStateChange();
	}
	
	if (isContactConditionChange_L || isContactConditionChange_R)
	{
		handleLocalContactStateChange();
	}

	shuffleContactStateDelay();

	//if (isStance_L)
	//{
	//	if (isHeelContact_L && !isM1Contact_L && !isToeContact_L)
	//	{
	//		contactState_L = 1;
	//	}
	//	if (isHeelContact_L && isM1Contact_L && !isToeContact_L)
	//	{
	//		contactState_L = 2;
	//	}
	//	if (isHeelContact_L && isM1Contact_L && isToeContact_L)
	//	{
	//		contactState_L = 3;
	//	}
	//	if (!isHeelContact_L && isM1Contact_L && isToeContact_L)
	//	{
	//		contactState_L = 4;
	//	}
	//	if (!isHeelContact_L && !isM1Contact_L && isToeContact_L)
	//	{
	//		contactState_L = 5;
	//	}
	//}
	//if (isStance_R)
	//{
	//	if (isHeelContact_R && !isM1Contact_R && !isToeContact_R)
	//	{
	//		contactState_R = 1;
	//	}
	//	if (isHeelContact_R && isM1Contact_R && !isToeContact_R)
	//	{
	//		contactState_R = 2;
	//	}
	//	if (isHeelContact_R && isM1Contact_R && isToeContact_R)
	//	{
	//		contactState_R = 3;
	//	}
	//	if (!isHeelContact_R && isM1Contact_R && isToeContact_R)
	//	{
	//		contactState_R = 4;
	//	}
	//	if (!isHeelContact_R && !isM1Contact_R && isToeContact_R)
	//	{
	//		contactState_R = 5;
	//	}
	//}

}

void GaitAnalysis::checkForMinima(bool sim)
{
	if (sim)
	{
		if (LPF_H_L.isMinima)
			isHeelContact_L = true;
		if (LPF_H_R.isMinima)
			isHeelContact_R = true;
		if (LPF_M1_L.isMinima)
			isM1Contact_L = true;
		if (LPF_M1_R.isMinima)
			isM1Contact_R = true;
		if (LPF_T_L.isMinima)
			isToeContact_L = true;
		if (LPF_T_R.isMinima)
			isToeContact_R = true;
	}

	else
	{
		if (heel_L == 1)
			isHeelContact_L = true;
		if (heel_R == 1)
			isHeelContact_R = true;
		if (m1_L == 1)
			isM1Contact_L = true;
		if (m1_R == 1)
			isM1Contact_R = true;
		if (toe_L == 1)
			isToeContact_L = true;
		if (toe_R == 1)
			isToeContact_R = true;
	}
}

void GaitAnalysis::checkForMaxima(bool sim)
{
	if (sim)
	{
		if (LPF_H_L.isMaxima)
			isHeelContact_L = false;
		if (LPF_H_R.isMaxima)
			isHeelContact_R = false;
		if (LPF_M1_L.isMaxima)
			isM1Contact_L = false;
		if (LPF_M1_R.isMaxima)
			isM1Contact_R = false;
		if (LPF_T_L.isMaxima)
			isToeContact_L = false;
		if (LPF_T_R.isMaxima)
			isToeContact_R = false;
	}

	else
	{
		if (heel_L == 0)
			isHeelContact_L = false;
		if (heel_R == 0)
			isHeelContact_R = false;
		if (m1_L == 0)
			isM1Contact_L = false;
		if (m1_R == 0)
			isM1Contact_R = false;
		if (toe_L == 0)
			isToeContact_L = false;
		if (toe_R == 0)
			isToeContact_R = false;
	}
}

void GaitAnalysis::shuffleContactStateDelay()
{
	isHeelContact_L_z1 = isHeelContact_L;
	isM1Contact_L_z1 = isM1Contact_L;
	isToeContact_L_z1 = isToeContact_L;

	isHeelContact_R_z1 = isHeelContact_R;
	isM1Contact_R_z1 = isM1Contact_R;
	isToeContact_R_z1 = isToeContact_R;

	isStance_L_z1 = isStance_L;
	isStance_R_z1 = isStance_R;

	contactCondition_L_z1 = contactCondition_L;
	contactCondition_R_z1 = contactCondition_R;
}

void GaitAnalysis::incrementDurationCounters()
{
	if (currentStepDuration < longestStep)
	{
		currentStepDuration += samplePeriod;
	}

	if (isStance_L)
	{
		currentStanceDuration_L += samplePeriod;
	}

	if (isSwing_L)
	{
		currentSwingDuration_L += samplePeriod;
	}

	if (isStance_R)
	{
		currentStanceDuration_R += samplePeriod;
	}

	if (isSwing_R)
	{
		currentSwingDuration_R += samplePeriod;
	}

}

void GaitAnalysis::incrementRolloverSupportCounters()
{
	supportTime_currentState_L += samplePeriod;
	supportTime_currentState_R += samplePeriod;
}

void GaitAnalysis::handleGlobalContactStateChange()
{
	if (isPhaseChange_L && isStance_L && contactCondition_L_z1 == 8)
	{
		stepCount++;
		stepCount_L++;
		stepSequence += "L";
		
		//Step Duration Parameter Calculation
		storeDuration_calcMean_CoV(currentStepDuration, stepDurationArrayLong, stepDurationArrayLong_L,
			stepDurationArrayLong_R, stepDurationArrayShort, stepDurationArrayShort_L, stepDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &stepDuration_Mean_Long, &stepDuration_CoV_Long,
			&stepDuration_Mean_Short, &stepDuration_CoV_Short, &stepDuration_Mean_Long_L, &stepDuration_Mean_Long_R,
			&stepDuration_Mean_Short_L, &stepDuration_Mean_Short_R);
		calcAsymmetry(&stepDuration_Asymmetry_Long,&stepDuration_Asymmetry_Short,stepDuration_Mean_Long_L,
						stepDuration_Mean_Long_R,stepDuration_Mean_Short_L,stepDuration_Mean_Short_R);
		currentStepDuration = 0;

		//Swing Duration Parameter Calculation
		storeDuration_calcMean_CoV(currentSwingDuration_L, swingDurationArrayLong, swingDurationArrayLong_L,
			swingDurationArrayLong_R, swingDurationArrayShort, swingDurationArrayShort_L, swingDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &swingDuration_Mean_Long, &swingDuration_CoV_Long,
			&swingDuration_Mean_Short, &swingDuration_CoV_Short, &swingDuration_Mean_Long_L, &swingDuration_Mean_Long_R,
			&swingDuration_Mean_Short_L, &swingDuration_Mean_Short_R);
		calcAsymmetry(&swingDuration_Asymmetry_Long, &swingDuration_Asymmetry_Short, swingDuration_Mean_Long_L,
			swingDuration_Mean_Long_R, swingDuration_Mean_Short_L, swingDuration_Mean_Short_R);

		currentSwingDuration_L = 0;

	}

	if (isPhaseChange_R && isStance_R && contactCondition_R_z1 == 8)
	{
		stepCount++;
		stepCount_R++;
		stepSequence += "R";

		//Step Duration Parameter Calculation
		storeDuration_calcMean_CoV(currentStepDuration, stepDurationArrayLong, stepDurationArrayLong_L,
			stepDurationArrayLong_R, stepDurationArrayShort, stepDurationArrayShort_L, stepDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &stepDuration_Mean_Long, &stepDuration_CoV_Long,
			&stepDuration_Mean_Short, &stepDuration_CoV_Short, &stepDuration_Mean_Long_L, &stepDuration_Mean_Long_R,
			&stepDuration_Mean_Short_L, &stepDuration_Mean_Short_R);
		calcAsymmetry(&stepDuration_Asymmetry_Long, &stepDuration_Asymmetry_Short, stepDuration_Mean_Long_L,
			stepDuration_Mean_Long_R, stepDuration_Mean_Short_L, stepDuration_Mean_Short_R);

		currentStepDuration = 0;

		//Swing Duration Parameter Calculation
		storeDuration_calcMean_CoV(currentSwingDuration_R, swingDurationArrayLong, swingDurationArrayLong_L,
			swingDurationArrayLong_R, swingDurationArrayShort, swingDurationArrayShort_L, swingDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &swingDuration_Mean_Long, &swingDuration_CoV_Long,
			&swingDuration_Mean_Short, &swingDuration_CoV_Short, &swingDuration_Mean_Long_L, &swingDuration_Mean_Long_R,
			&swingDuration_Mean_Short_L, &swingDuration_Mean_Short_R);
		calcAsymmetry(&swingDuration_Asymmetry_Long, &swingDuration_Asymmetry_Short, swingDuration_Mean_Long_L,
			swingDuration_Mean_Long_R, swingDuration_Mean_Short_L, swingDuration_Mean_Short_R);

		currentSwingDuration_R = 0;
	}

	if (isPhaseChange_L && !isStance_L)
	{
		//Stance Time Parameter Calculation
		storeDuration_calcMean_CoV(currentStanceDuration_L, stanceDurationArrayLong, stanceDurationArrayLong_L,
			stanceDurationArrayLong_R, stanceDurationArrayShort, stanceDurationArrayShort_L, stanceDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &stanceDuration_Mean_Long, &stanceDuration_CoV_Long,
			&stanceDuration_Mean_Short, &stanceDuration_CoV_Short, &stanceDuration_Mean_Long_L, &stanceDuration_Mean_Long_R,
			&stanceDuration_Mean_Short_L, &stanceDuration_Mean_Short_R);
		calcAsymmetry(&stanceDuration_Asymmetry_Long, &stanceDuration_Asymmetry_Short, stanceDuration_Mean_Long_L,
			stanceDuration_Mean_Long_R, stanceDuration_Mean_Short_L, stanceDuration_Mean_Short_R);

		currentStanceDuration_L = 0;
	}

	if (isPhaseChange_R && !isStance_R)
	{
		storeDuration_calcMean_CoV(currentStanceDuration_R, stanceDurationArrayLong, stanceDurationArrayLong_L,
			stanceDurationArrayLong_R, stanceDurationArrayShort, stanceDurationArrayShort_L, stanceDurationArrayShort_R,
			stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &stanceDuration_Mean_Long, &stanceDuration_CoV_Long,
			&stanceDuration_Mean_Short, &stanceDuration_CoV_Short, &stanceDuration_Mean_Long_L, &stanceDuration_Mean_Long_R,
			&stanceDuration_Mean_Short_L, &stanceDuration_Mean_Short_R);
		calcAsymmetry(&stanceDuration_Asymmetry_Long, &stanceDuration_Asymmetry_Short, stanceDuration_Mean_Long_L,
			stanceDuration_Mean_Long_R, stanceDuration_Mean_Short_L, stanceDuration_Mean_Short_R);

		currentStanceDuration_R = 0;
	}
}

void GaitAnalysis::handleLocalContactStateChange()
{
	
	if (isContactConditionChange_L)
	{
		rolloverPatternLong[stepCount] += std::to_string(contactCondition_L);
		rolloverPatternLong_L[stepCount_L] += std::to_string(contactCondition_L);

		if (contactCondition_L == 8)
		{
			lastRollover_L = rolloverPatternLong_L[stepCount_L];
			rolloverPatternLong[stepCount] = lastRollover_L;
		}

		if (contactCondition_L_z1 == 8)
		{
			lastRollover_L = "";
			rolloverPatternShort_L[stepCount_L % stepSizeWindow] = "";
		}
		rolloverPatternShort_L[stepCount_L % stepSizeWindow] += std::to_string(contactCondition_L);
		switch (contactCondition_L_z1)
		{
		case 1:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s1_long, supportTime_s1_long_L,
				supportTime_s1_long_R, supportTime_s1_short, supportTime_s1_short_L, supportTime_s1_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s1_Mean_Long, &supportTime_s1_CoV_Long,
				&supportTime_s1_Mean_Short, &supportTime_s1_CoV_Short, &supportTime_s1_Mean_Long_L, &supportTime_s1_Mean_Long_R,
				&supportTime_s1_Mean_Short_L, &supportTime_s1_Mean_Short_R);
			calcAsymmetry(&s1_Asymmetry_Long, &s1_Asymmetry_Short, supportTime_s1_Mean_Long_L,
				supportTime_s1_Mean_Long_R, supportTime_s1_Mean_Short_L, supportTime_s1_Mean_Short_R);


			supportTime_currentState_L = 0;
			break;
		}
		case 2:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s2_long, supportTime_s2_long_L,
				supportTime_s2_long_R, supportTime_s2_short, supportTime_s2_short_L, supportTime_s2_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s2_Mean_Long, &supportTime_s2_CoV_Long,
				&supportTime_s2_Mean_Short, &supportTime_s2_CoV_Short, &supportTime_s2_Mean_Long_L, &supportTime_s2_Mean_Long_R,
				&supportTime_s2_Mean_Short_L, &supportTime_s2_Mean_Short_R);
			calcAsymmetry(&s2_Asymmetry_Long, &s2_Asymmetry_Short, supportTime_s2_Mean_Long_L,
				supportTime_s2_Mean_Long_R, supportTime_s2_Mean_Short_L, supportTime_s2_Mean_Short_R);

			supportTime_currentState_L = 0;
			break;
		}
		case 3:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s3_long, supportTime_s3_long_L,
				supportTime_s3_long_R, supportTime_s3_short, supportTime_s3_short_L, supportTime_s3_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s3_Mean_Long, &supportTime_s3_CoV_Long,
				&supportTime_s3_Mean_Short, &supportTime_s3_CoV_Short, &supportTime_s3_Mean_Long_L, &supportTime_s3_Mean_Long_R,
				&supportTime_s3_Mean_Short_L, &supportTime_s3_Mean_Short_R);
			calcAsymmetry(&s3_Asymmetry_Long, &s3_Asymmetry_Short, supportTime_s3_Mean_Long_L,
				supportTime_s3_Mean_Long_R, supportTime_s3_Mean_Short_L, supportTime_s3_Mean_Short_R);
			supportTime_currentState_L = 0;
			break;
		}
		case 4:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s12_long, supportTime_s12_long_L,
				supportTime_s12_long_L, supportTime_s12_short, supportTime_s12_short_L, supportTime_s12_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s12_Mean_Long, &supportTime_s12_CoV_Long,
				&supportTime_s12_Mean_Short, &supportTime_s12_CoV_Short, &supportTime_s12_Mean_Long_L, &supportTime_s12_Mean_Long_R,
				&supportTime_s12_Mean_Short_L, &supportTime_s12_Mean_Short_R);
			calcAsymmetry(&s12_Asymmetry_Long, &s12_Asymmetry_Short, supportTime_s12_Mean_Long_L,
				supportTime_s12_Mean_Long_R, supportTime_s12_Mean_Short_L, supportTime_s12_Mean_Short_R);

			supportTime_currentState_L = 0;
			break;
		}
		case 5:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s23_long, supportTime_s23_long_L,
				supportTime_s23_long_L, supportTime_s23_short, supportTime_s23_short_L, supportTime_s23_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s23_Mean_Long, &supportTime_s23_CoV_Long,
				&supportTime_s23_Mean_Short, &supportTime_s23_CoV_Short, &supportTime_s23_Mean_Long_L, &supportTime_s23_Mean_Long_R,
				&supportTime_s23_Mean_Short_L, &supportTime_s23_Mean_Short_R);
			calcAsymmetry(&s23_Asymmetry_Long, &s23_Asymmetry_Short, supportTime_s23_Mean_Long_L,
				supportTime_s23_Mean_Long_R, supportTime_s23_Mean_Short_L, supportTime_s23_Mean_Short_R);
			supportTime_currentState_L = 0;
			break;
		}
		case 6:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s13_long, supportTime_s13_long_L,
				supportTime_s13_long_L, supportTime_s13_short, supportTime_s13_short_L, supportTime_s13_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s13_Mean_Long, &supportTime_s13_CoV_Long,
				&supportTime_s13_Mean_Short, &supportTime_s13_CoV_Short, &supportTime_s13_Mean_Long_L, &supportTime_s13_Mean_Long_R,
				&supportTime_s13_Mean_Short_L, &supportTime_s13_Mean_Short_R);
			calcAsymmetry(&s13_Asymmetry_Long, &s13_Asymmetry_Short, supportTime_s13_Mean_Long_L,
				supportTime_s13_Mean_Long_R, supportTime_s13_Mean_Short_L, supportTime_s13_Mean_Short_R);

			supportTime_currentState_L = 0;
			break;
		}
		case 7:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_L, supportTime_s123_long, supportTime_s123_long_L,
				supportTime_s123_long_L, supportTime_s123_short, supportTime_s123_short_L, supportTime_s123_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, true, &supportTime_s123_Mean_Long, &supportTime_s123_CoV_Long,
				&supportTime_s123_Mean_Short, &supportTime_s123_CoV_Short, &supportTime_s123_Mean_Long_L, &supportTime_s123_Mean_Long_R,
				&supportTime_s123_Mean_Short_L, &supportTime_s123_Mean_Short_R);
			calcAsymmetry(&s123_Asymmetry_Long, &s123_Asymmetry_Short, supportTime_s123_Mean_Long_L,
				supportTime_s123_Mean_Long_R, supportTime_s123_Mean_Short_L, supportTime_s123_Mean_Short_R);

			supportTime_currentState_L = 0;
			break;
		}
		case 8:
		{
			supportTime_currentState_L = 0;
			break;
		}
		}
	}

	if (isContactConditionChange_R)
	{
		rolloverPatternLong_R[stepCount_R] += std::to_string(contactCondition_R);

		if (contactCondition_R == 8)
		{
			lastRollover_R = rolloverPatternLong_R[stepCount_R];
			rolloverPatternLong[stepCount] = lastRollover_R;
		}

		if (contactCondition_R_z1 == 8)
		{		
			lastRollover_R = "";
			rolloverPatternShort_R[stepCount_R % stepSizeWindow] = "";
		}
		rolloverPatternShort_R[stepCount_R % stepSizeWindow] += std::to_string(contactCondition_R);
		switch (contactCondition_R_z1)
		{
		case 1:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s1_long, supportTime_s1_long_L,
				supportTime_s1_long_R, supportTime_s1_short, supportTime_s1_short_L, supportTime_s1_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s1_Mean_Long, &supportTime_s1_CoV_Long,
				&supportTime_s1_Mean_Short, &supportTime_s1_CoV_Short, &supportTime_s1_Mean_Long_L, &supportTime_s1_Mean_Long_R,
				&supportTime_s1_Mean_Short_L, &supportTime_s1_Mean_Short_R);
			calcAsymmetry(&s1_Asymmetry_Long, &s1_Asymmetry_Short, supportTime_s1_Mean_Long_L,
				supportTime_s1_Mean_Long_R, supportTime_s1_Mean_Short_L, supportTime_s1_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 2:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s2_long, supportTime_s2_long_L,
				supportTime_s2_long_R, supportTime_s2_short, supportTime_s2_short_L, supportTime_s2_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s2_Mean_Long, &supportTime_s2_CoV_Long,
				&supportTime_s2_Mean_Short, &supportTime_s2_CoV_Short, &supportTime_s2_Mean_Long_L, &supportTime_s2_Mean_Long_R,
				&supportTime_s2_Mean_Short_L, &supportTime_s2_Mean_Short_R);
			calcAsymmetry(&s2_Asymmetry_Long, &s2_Asymmetry_Short, supportTime_s2_Mean_Long_L,
				supportTime_s2_Mean_Long_R, supportTime_s2_Mean_Short_L, supportTime_s2_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 3:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s3_long, supportTime_s3_long_L,
				supportTime_s3_long_R, supportTime_s3_short, supportTime_s3_short_L, supportTime_s3_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s3_Mean_Long, &supportTime_s3_CoV_Long,
				&supportTime_s3_Mean_Short, &supportTime_s3_CoV_Short, &supportTime_s3_Mean_Long_L, &supportTime_s3_Mean_Long_R,
				&supportTime_s3_Mean_Short_L, &supportTime_s3_Mean_Short_R);
			calcAsymmetry(&s3_Asymmetry_Long, &s3_Asymmetry_Short, supportTime_s3_Mean_Long_L,
				supportTime_s3_Mean_Long_R, supportTime_s3_Mean_Short_L, supportTime_s3_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 4:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s12_long, supportTime_s12_long_L,
				supportTime_s12_long_R, supportTime_s12_short, supportTime_s12_short_L, supportTime_s12_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s12_Mean_Long, &supportTime_s12_CoV_Long,
				&supportTime_s12_Mean_Short, &supportTime_s12_CoV_Short, &supportTime_s12_Mean_Long_L, &supportTime_s12_Mean_Long_R,
				&supportTime_s12_Mean_Short_L, &supportTime_s12_Mean_Short_R);
			calcAsymmetry(&s12_Asymmetry_Long, &s12_Asymmetry_Short, supportTime_s12_Mean_Long_L,
				supportTime_s12_Mean_Long_R, supportTime_s12_Mean_Short_L, supportTime_s12_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 5:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s23_long, supportTime_s23_long_L,
				supportTime_s23_long_R, supportTime_s23_short, supportTime_s23_short_L, supportTime_s23_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s23_Mean_Long, &supportTime_s23_CoV_Long,
				&supportTime_s23_Mean_Short, &supportTime_s23_CoV_Short, &supportTime_s23_Mean_Long_L, &supportTime_s23_Mean_Long_R,
				&supportTime_s23_Mean_Short_L, &supportTime_s23_Mean_Short_R);
			calcAsymmetry(&s23_Asymmetry_Long, &s23_Asymmetry_Short, supportTime_s23_Mean_Long_L,
				supportTime_s23_Mean_Long_R, supportTime_s23_Mean_Short_L, supportTime_s23_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 6:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s13_long, supportTime_s13_long_L,
				supportTime_s13_long_R, supportTime_s13_short, supportTime_s13_short_L, supportTime_s13_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s13_Mean_Long, &supportTime_s13_CoV_Long,
				&supportTime_s13_Mean_Short, &supportTime_s13_CoV_Short, &supportTime_s13_Mean_Long_L, &supportTime_s13_Mean_Long_R,
				&supportTime_s13_Mean_Short_L, &supportTime_s13_Mean_Short_R);
			calcAsymmetry(&s13_Asymmetry_Long, &s13_Asymmetry_Short, supportTime_s13_Mean_Long_L,
				supportTime_s13_Mean_Long_R, supportTime_s13_Mean_Short_L, supportTime_s13_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 7:
		{
			storeDuration_calcMean_CoV(supportTime_currentState_R, supportTime_s123_long, supportTime_s123_long_L,
				supportTime_s123_long_R, supportTime_s123_short, supportTime_s123_short_L, supportTime_s123_short_R,
				stepSizeWindow, stepCount, stepCount_L, stepCount_R, false, &supportTime_s123_Mean_Long, &supportTime_s123_CoV_Long,
				&supportTime_s123_Mean_Short, &supportTime_s123_CoV_Short, &supportTime_s123_Mean_Long_L, &supportTime_s123_Mean_Long_R,
				&supportTime_s123_Mean_Short_L, &supportTime_s123_Mean_Short_R);
			calcAsymmetry(&s123_Asymmetry_Long, &s123_Asymmetry_Short, supportTime_s123_Mean_Long_L,
				supportTime_s123_Mean_Long_R, supportTime_s123_Mean_Short_L, supportTime_s123_Mean_Short_R);

			supportTime_currentState_R = 0;
			break;
		}
		case 8:
		{
			supportTime_currentState_R = 0;
			break;
		}
		}
	}
}

void GaitAnalysis::storeDuration_calcMean_CoV(float duration, float *longDurationArray, float *longDurationArray_L,
									   float *longDurationArray_R, float *shortDurationArray,float *shortDurationArray_L,
										float *shortDurationArray_R, int windowLength,
										int stepCount, int stepCount_L, int stepCount_R, bool LRswitch, 
										float *meanVarLong, float *covVarLong, float *meanVarShort, float *covVarShort,
										float *meanVarL_Long, float *meanVarR_Long, float *meanVarL_Short, float *meanVarR_Short)
{
	float stdLong = 0;
	if (stepCount > 5)
	{
		//Enter new value into respective arrays
		int trueStepCount = stepCount - 5;
		longDurationArray[trueStepCount - 1] = duration;
		shortDurationArray[(trueStepCount - 1)%windowLength] = duration;

		//Long
		*meanVarLong = calcArrayMean(longDurationArray, trueStepCount); // Set Mean
		stdLong = calcArraySTD(longDurationArray, meanVarLong, trueStepCount);
		*covVarLong = stdLong / *meanVarLong; // Set STD

		//Short
		*meanVarShort = calcArrayMean(shortDurationArray, windowLength); // Set Mean
		stdLong = calcArraySTD(shortDurationArray, meanVarShort, windowLength);
		*covVarShort = stdLong / *meanVarShort; // Set STD

		if (LRswitch)
		{
			longDurationArray_L[stepCount_L - 1] = duration;
			*meanVarL_Long = calcArrayMean(longDurationArray_L, stepCount_L);
			shortDurationArray_L[(stepCount_L - 1) % windowLength] = duration;
			*meanVarL_Short = calcArrayMean(shortDurationArray_L,windowLength);
		}
		else
		{
			longDurationArray_R[stepCount_R - 1] = duration;
			*meanVarR_Long = calcArrayMean(longDurationArray_R, stepCount_R);
			shortDurationArray_R[(stepCount_R - 1) % windowLength] = duration;
			*meanVarR_Short = calcArrayMean(shortDurationArray_R, windowLength);
		}
	}
}

void GaitAnalysis::calcAsymmetry(float *asymmVarLong, float 
	*asymmVarShort, float long_mean_L,
	float long_mean_R, float short_mean_L, float short_mean_R)
{
	*asymmVarLong = (long_mean_L - long_mean_R) * 2.0 / (long_mean_L + long_mean_R);
	*asymmVarShort = (short_mean_L - short_mean_R) * 2.0 / (short_mean_L + short_mean_R);
}

float GaitAnalysis::calcArrayMean(float *arr, int count)
{
	float mean = 0;
	for (int i = 0; i < count; i++)
	{
		mean += arr[i];
	}
	mean /= count;
	return mean;
}

float GaitAnalysis::calcArraySTD(float *arr, float *mean, int count)
{
	float standDev = 0.0; float variance = 0.0;
	for (int i = 0; i < count; i++)
	{
		variance += pow(arr[i] - *mean, 2);
	}
	variance = variance / float(count - 1);

	standDev = sqrt(variance);

	return standDev;
}
