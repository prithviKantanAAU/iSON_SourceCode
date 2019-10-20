#pragma once

#include <cmath>

class ParamMapping
{
public:
	ParamMapping();
	~ParamMapping();
	float getMappingValue(float desiredValue, float currentValue, float slopeFactor, float paramMin,
				float paramMax, int mappingType, int mappingDirection, int numQuantizations);
	float quantizeParam(float paramUnquantized, float paramMin, float paramMax, int numQuantizations);

private:
	int maxQuantizationLevels = 20;
};

