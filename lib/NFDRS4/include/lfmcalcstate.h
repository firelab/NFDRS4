#pragma once
#include <vector>
#include "nfdrs4statesizes.h"
#include <stdio.h>
#include <time.h>

class LFMCalcState
{
public:
	LFMCalcState();
	LFMCalcState(const LFMCalcState& rhs);
	~LFMCalcState();

	bool  ReadState(FILE *in);
	bool SaveState(FILE *out);

	time_t m_lastUpdateTime;
	char m_UseVPDAvg;
	char m_IsHerb;
	char m_IsAnnual;
	char m_LFIdaysAvg;
	char m_nDaysPrecip;
	FP_STORAGE_TYPE m_Lat;
	FP_STORAGE_TYPE m_TminMin;
	FP_STORAGE_TYPE m_TminMax;
	FP_STORAGE_TYPE m_VPDMin;
	FP_STORAGE_TYPE m_VPDMax;
	FP_STORAGE_TYPE m_DaylenMin;
	FP_STORAGE_TYPE m_DaylenMax;
	FP_STORAGE_TYPE m_MaxGSI;
	FP_STORAGE_TYPE m_GreenupThreshold;
	FP_STORAGE_TYPE m_MaxLFMVal;
	FP_STORAGE_TYPE m_MinLFMVal;
	FP_STORAGE_TYPE m_Slope;
	FP_STORAGE_TYPE m_Intercept;
	char m_hasGreenedUpThisYear;
	char m_hasExceeded120ThisYear;
	char m_canIncreaseHerb;
	FP_STORAGE_TYPE lastHerbFM;
	char m_useRTPrecip;
	FP_STORAGE_TYPE m_pcpMin;
	FP_STORAGE_TYPE m_pcpMax;

	std::vector<FP_STORAGE_TYPE> m_qGSI;
};

