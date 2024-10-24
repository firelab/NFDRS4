#pragma once
#include "dfmcalcstate.h"
#include "lfmcalcstate.h"
#include <string>
#include <vector>
#include "nfdrs4statesizes.h"
#include "utctime.h"

class NFDRS4;

class NFDRS4State
{
public:
	NFDRS4State();
	NFDRS4State(NFDRS4 *pNFDRS);
	NFDRS4State(const NFDRS4State& rhs);
	~NFDRS4State();

	bool LoadState(std::string fileName);
	bool SaveState(std::string fileName);

	short m_NFDRSVersion;

	//dead fuel moisture states
	DFMCalcState fm1State;
	DFMCalcState fm10State;
	DFMCalcState fm100State;
	DFMCalcState fm1000State;

	//gsi and live fuel moisture states
	//LFMCalcState gsiState;
	LFMCalcState herbState;
	LFMCalcState woodyState;

	//local variables
	FP_STORAGE_TYPE m_Lat;
	short m_YesterdayJDay;
	char m_SlopeClass;
	char m_FuelModel;
	FP_STORAGE_TYPE m_MC1, m_MC10, m_MC100, m_MC1000, m_MCWOOD, m_MCHERB;
	short m_PrevYear;
	short m_KBDI, m_YKBDI, m_StartKBDI, m_KBDIThreshold;
	FP_STORAGE_TYPE m_CummPrecip, m_AvgPrecip;
	char m_UseLoadTransfer, m_UseCuring;
	FP_STORAGE_TYPE m_FuelTemperature;
	FP_STORAGE_TYPE m_BI, m_ERC, m_SC, m_IC, m_GSI;
	short m_nConsectiveSnowDays;
	//time_t m_lastUpdateTime;
	utctime::UTCTime m_lastUtcUpdateTime;
	utctime::UTCTime m_lastDailyUpdateTime;
	//int m_KBDIThreshold;
	//precip we can always store as float
	//as we only read to 3 decimal places
	//on import (FW13 format)
	std::vector<float> m_qPrecip;
	std::vector<float> m_qHourlyPrecip;
	std::vector<float> m_qHourlyTemp;
	std::vector<float> m_qHourlyRH;
};

