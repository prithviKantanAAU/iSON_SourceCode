#include "ParamMapping.h"

#include <cmath>



ParamMapping::ParamMapping()
{}

ParamMapping::~ParamMapping()
{}

float ParamMapping::getMappingValue(float desiredValue, float currentValue, float slopeFactor, float paramMin, float paramMax
								    , int mappingType, int mappingDirection, int numQuantizationSteps)
{
	//Mapping Type -	1 = Linear	//	2 = Quadratic // 3 = Sigmoid
	//Mapping Direction -	1 = Maximize AP if GP is less	// 2 = Maximize AP if GP is a precise value
						//	3 = Maximize AP if GP is greater
	
	float delta = currentValue - desiredValue;
	float range = paramMax - paramMin;
	float audioParamValue = paramMin;
	float argument = 0;
	switch (mappingDirection)
	{
	case 1:									//Less is Better
		argument = (0 < delta) ? delta : 0;
		argument = (argument > slopeFactor) ? slopeFactor : argument;
		switch (mappingType)
		{			
		case 1:								//Linear - Lesser	
			argument = (-argument + slopeFactor) / slopeFactor;
			audioParamValue += range * argument;
			break;
		case 2:								//Quadratic - Lesser
			argument = (-argument + slopeFactor) / slopeFactor;
			argument = pow(argument, 2);
			audioParamValue += range * argument;
			break;
		case 3:								//Sigmoid - Lesser
			audioParamValue += range * (1 / (1 + exp(delta)));
			break;
		}
		break;
	case 2:									//Precise Value Desired
		argument = abs(delta);
		argument = (argument > slopeFactor) ? slopeFactor : argument;
		switch (mappingType)
		{
		case 1:								//Linear - Precise
			argument = (-argument + slopeFactor) / slopeFactor;
			audioParamValue += range * argument;
			break;
		case 2:								//Quadratic - Precise
			argument = (-argument + slopeFactor) / slopeFactor;
			argument = pow(argument, 2);
			audioParamValue += range * argument;
			break;
		case 3:								//Sigmoid - Precise	- Do Linear
			argument = (-argument + slopeFactor) / slopeFactor;
			audioParamValue += range * argument;
			return audioParamValue;
			break;
		}
		break;
	case 3:									//Greater is Better
		argument = (desiredValue > currentValue) ? -delta : 0;
		argument = (argument > slopeFactor) ? slopeFactor : argument;
		switch (mappingType)
		{			
		case 1:								//Linear - Greater			
			argument = (-argument + slopeFactor) / slopeFactor;
			audioParamValue += range * argument;
			break;
		case 2:								//Quadratic - Greater
			argument = (-argument + slopeFactor) / slopeFactor;
			argument = pow(argument, 2);
			audioParamValue += range * argument;
			break;
		case 3:								//Sigmoid - Greater
			audioParamValue += range * (1 / (1 + exp(-delta)));
			break;
		}
		break;
	}

	audioParamValue = quantizeParam(audioParamValue, paramMin, paramMax, numQuantizationSteps);
	return audioParamValue;
}

float ParamMapping::quantizeParam(float currentParamValue, float paramMin, float paramMax, int numQuantizationSteps)
{
	float quantizedParam = paramMin;

	if (numQuantizationSteps == 0)
		return currentParamValue;	

	else
	{
		float quantizationStepSize = (paramMax - paramMin) / (float)numQuantizationSteps;
		
		float diff = 10000;
		for (int i = 0; i < numQuantizationSteps + 1; i++)
		{
			float currentStepForTest = paramMin;
			currentStepForTest += i * quantizationStepSize;
			if (diff > abs(currentParamValue - currentStepForTest))
			{
				diff = currentParamValue - currentStepForTest;
			}
		}
		quantizedParam = currentParamValue - diff;
		return quantizedParam;
	}
}