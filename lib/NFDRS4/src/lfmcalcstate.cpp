#include "lfmcalcstate.h"



LFMCalcState::LFMCalcState()
{
	//m_Year = 0;
	//m_Month = 0;
	//m_Day = 0;
	//m_Hour = 0;
	m_UseVPDAvg = 0;
	m_IsHerb = 0;
	m_IsAnnual = 0;
	m_LFIdaysAvg = 0;
	m_Lat = 0.0;
	m_TminMin = 0.0;
	m_TminMax = 0.0;
	m_VPDMin = 0.0;
	m_VPDMax = 0.0;
	m_DaylenMin = 0.0;
	m_DaylenMax = 0.0;
	m_MaxGSI = 0.0;
	m_GreenupThreshold = 0.0;
	m_MaxLFMVal = 0.0;
	m_MinLFMVal = 0.0;
	m_Slope =0.0;
	m_Intercept = 0.0;
	m_hasGreenedUpThisYear = 0;
	m_hasExceeded120ThisYear = 0;
	m_canIncreaseHerb = 0;
	lastHerbFM = 0;
	m_lastUpdateTime = 0;
	m_nDaysPrecip = 0;
	m_useRTPrecip = 0;
	m_pcpMin = 0.5;
	m_pcpMax = 1.5;
}

LFMCalcState::LFMCalcState(const LFMCalcState& rhs)
{
	//m_Year = rhs.m_Year;
	//m_Month = rhs.m_Month;
	//m_Day = rhs.m_Day;
	//m_Hour = rhs.m_Hour;
	m_UseVPDAvg = rhs.m_UseVPDAvg;
	m_IsHerb = rhs.m_IsHerb;
	m_IsAnnual = rhs.m_IsAnnual;
	m_LFIdaysAvg = rhs.m_LFIdaysAvg;
	m_Lat = rhs.m_Lat;
	m_TminMin = rhs.m_TminMin;
	m_TminMax = rhs.m_TminMax;
	m_VPDMin = rhs.m_VPDMin;
	m_VPDMax = rhs.m_VPDMax;
	m_DaylenMin = rhs.m_DaylenMin;
	m_DaylenMax = rhs.m_DaylenMax;
	m_MaxGSI = rhs.m_MaxGSI;
	m_GreenupThreshold = rhs.m_GreenupThreshold;
	m_MaxLFMVal = rhs.m_MaxLFMVal;
	m_MinLFMVal = rhs.m_MinLFMVal;
	m_Slope = rhs.m_Slope;
	m_Intercept = rhs.m_Intercept;
	m_hasGreenedUpThisYear = rhs.m_hasGreenedUpThisYear;
	m_hasExceeded120ThisYear = rhs.m_hasExceeded120ThisYear;
	m_canIncreaseHerb = rhs.m_canIncreaseHerb;
	lastHerbFM = rhs.lastHerbFM;
	m_qGSI = rhs.m_qGSI;
	m_lastUpdateTime = rhs.m_lastUpdateTime;
	m_nDaysPrecip = rhs.m_nDaysPrecip;
	m_useRTPrecip = rhs.m_useRTPrecip;
	m_pcpMin = rhs.m_pcpMin;
	m_pcpMax = rhs.m_pcpMax;
}

LFMCalcState::~LFMCalcState()
{
}

bool LFMCalcState::ReadState(FILE *in)
{
	size_t nRead = fread(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_UseVPDAvg, sizeof(m_UseVPDAvg), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_IsHerb, sizeof(m_IsHerb), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_IsAnnual, sizeof(m_IsAnnual), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_LFIdaysAvg, sizeof(m_LFIdaysAvg), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Lat, sizeof(m_Lat), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_TminMin, sizeof(m_TminMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_TminMax, sizeof(m_TminMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_VPDMin, sizeof(m_VPDMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_VPDMax, sizeof(m_VPDMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_DaylenMin, sizeof(m_DaylenMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_DaylenMax, sizeof(m_DaylenMax), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MaxGSI, sizeof(m_MaxGSI), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_GreenupThreshold, sizeof(m_GreenupThreshold), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MaxLFMVal, sizeof(m_MaxLFMVal), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_MinLFMVal, sizeof(m_MinLFMVal), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Slope, sizeof(m_Slope), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_Intercept, sizeof(m_Intercept), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_canIncreaseHerb, sizeof(m_canIncreaseHerb), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&lastHerbFM, sizeof(lastHerbFM), 1, in);
	if (nRead != 1)
		return false;
	short qSize;
	nRead = fread(&qSize, sizeof(qSize), 1, in);
	if (nRead != 1)
		return false;
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
			return false;
		m_qGSI.push_back(tVal);
	}
	nRead = fread(&m_nDaysPrecip, sizeof(m_nDaysPrecip), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_useRTPrecip, sizeof(m_useRTPrecip), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_pcpMin, sizeof(m_pcpMin), 1, in);
	if (nRead != 1)
		return false;
	nRead = fread(&m_pcpMax, sizeof(m_pcpMax), 1, in);
	if (nRead != 1)
		return false;

	return true;
}

bool LFMCalcState::SaveState(FILE *out)
{
	size_t nWrite = fwrite(&m_lastUpdateTime, sizeof(m_lastUpdateTime), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_UseVPDAvg, sizeof(m_UseVPDAvg), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_IsHerb, sizeof(m_IsHerb), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_IsAnnual, sizeof(m_IsAnnual), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_LFIdaysAvg, sizeof(m_LFIdaysAvg), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Lat, sizeof(m_Lat), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_TminMin, sizeof(m_TminMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_TminMax, sizeof(m_TminMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_VPDMin, sizeof(m_VPDMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_VPDMax, sizeof(m_VPDMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_DaylenMin, sizeof(m_DaylenMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_DaylenMax, sizeof(m_DaylenMax), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MaxGSI, sizeof(m_MaxGSI), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_GreenupThreshold, sizeof(m_GreenupThreshold), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MaxLFMVal, sizeof(m_MaxLFMVal), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_MinLFMVal, sizeof(m_MinLFMVal), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Slope, sizeof(m_Slope), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_Intercept, sizeof(m_Intercept), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_hasGreenedUpThisYear, sizeof(m_hasGreenedUpThisYear), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_hasExceeded120ThisYear, sizeof(m_hasExceeded120ThisYear), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_canIncreaseHerb, sizeof(m_canIncreaseHerb), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&lastHerbFM, sizeof(lastHerbFM), 1, out);
	if (nWrite != 1)
		return false;
	short qSize = m_qGSI.size();
	nWrite = fwrite(&qSize, sizeof(qSize), 1, out);
	if (nWrite != 1)
		return false;
	for (int i = 0; i < qSize; i++)
	{
		FP_STORAGE_TYPE tVal = m_qGSI[i];
		nWrite = fwrite(&tVal, sizeof(tVal), 1, out);
		if (nWrite != 1)
			return false;
	}
	nWrite = fwrite(&m_nDaysPrecip, sizeof(m_nDaysPrecip), 1, out);
	if (nWrite != 1)
		return false;

	nWrite = fwrite(&m_useRTPrecip, sizeof(m_useRTPrecip), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_pcpMin, sizeof(m_pcpMin), 1, out);
	if (nWrite != 1)
		return false;
	nWrite = fwrite(&m_pcpMax, sizeof(m_pcpMax), 1, out);
	if (nWrite != 1)
		return false;

	return true;
}
