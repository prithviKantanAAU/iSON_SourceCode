#pragma once
// includes for the project
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define FLT_MIN_PLUS          1.175494351e-38         /* min positive value */
#define FLT_MIN_MINUS        -1.175494351e-38         /* min negative value */                
#define M_PI		          3.141592653589793238462643383279502884L /* pi */

class BiQuad
{
public:
	BiQuad(void)
	{
		calculateLPFCoeffs(0.5, 1);
		flushDelays();
	}
	virtual ~BiQuad(void)
	{}

protected:
	float m_f_Xz_1; // x z-1 delay element
	float m_f_Xz_2; // x z-2 delay element
	float m_f_Yz_1; // y z-1 delay element
	float m_f_Yz_2; // y z-2 delay element

	// allow other objects to set these directly since we have no cooking
	// function or intelligence

public:
	double m_f_a0;
	double m_f_a1;
	double m_f_a2;
	double m_f_b1;
	double m_f_b2;

	bool isMaxima = false;
	bool isMinima = false;

	// flush Delays
	void flushDelays()
	{
		m_f_Xz_1 = 0;
		m_f_Xz_2 = 0;
		m_f_Yz_1 = 0;
		m_f_Yz_2 = 0;
	}

	void calculateLPFCoeffs(float fCutoffFreq, float fQ)
	{
		// use same terms as in book:
		float theta_c = 2.0*M_PI*fCutoffFreq / 200;
		float d = 1.0 / fQ;

		// intermediate values
		float fBetaNumerator = 1.0 - ((d / 2.0)*(sin(theta_c)));
		float fBetaDenominator = 1.0 + ((d / 2.0)*(sin(theta_c)));

		// beta
		float fBeta = 0.5*(fBetaNumerator / fBetaDenominator);

		// gamma
		float fGamma = (0.5 + fBeta)*(cos(theta_c));

		// alpha
		float fAlpha = (0.5 + fBeta - fGamma) / 2.0;

		// left channel
		m_f_a0 = (0.5 + fBeta - fGamma) / 2.0;
		m_f_a1 = 0.5 + fBeta - fGamma;
		m_f_a2 = (0.5 + fBeta - fGamma) / 2.0;
		m_f_b1 = -2 * fGamma;
		m_f_b2 = 2 * fBeta;
	}

		// Do the filter: given input xn, calculate output yn and return it
	float doBiQuad(float f_xn,float thresh)
	{
		// just do the difference equation: y(n) = a0x(n) + a1x(n-1) + a2x(n-2) - b1y(n-1) - b2y(n-2)
		float yn = m_f_a0 * f_xn + m_f_a1 * m_f_Xz_1 + m_f_a2 * m_f_Xz_2 
				   - m_f_b1 * m_f_Yz_1 - m_f_b2 * m_f_Yz_2;

		// underflow check
		if (yn > 0.0 && yn < FLT_MIN_PLUS) yn = 0;
		if (yn < 0.0 && yn > FLT_MIN_MINUS) yn = 0;

		// X delay Shuffle
		m_f_Xz_2 = m_f_Xz_1;
		m_f_Xz_1 = f_xn;

		//Check Maxima
		if (m_f_Yz_1 > m_f_Yz_2 && m_f_Yz_1 > yn && m_f_Yz_1 > thresh)
		{
			isMaxima = true;
		}

		else isMaxima = false;
		

		//Check Minima
		if (m_f_Yz_1 < m_f_Yz_2 && m_f_Yz_1 < yn && m_f_Yz_1 > 0)
		{
			isMinima = true;
		}

		else isMinima = false;
				
		m_f_Yz_2 = m_f_Yz_1;
		m_f_Yz_1 = yn;

		return yn;
	}	
};

