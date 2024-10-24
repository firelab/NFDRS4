#include "nfdrs4.h"
#include "nfdrs4calcstate.h"


NFDRS4State::NFDRS4State()
{
}


NFDRS4State::~NFDRS4State()
{
}

NFDRS4State::NFDRS4State(const NFDRS4State& rhs)
{
	m_NFDRSVersion = rhs.m_NFDRSVersion;
	fm1State = rhs.fm1State;
	fm10State = rhs.fm10State;
	fm100State = rhs.fm100State;
	fm1000State = rhs.fm1000State;
	herbState = rhs.herbState;
	woodyState = rhs.woodyState;
	m_Lat = rhs.m_Lat;
	m_YesterdayJDay = rhs.m_YesterdayJDay;
	m_SlopeClass = rhs.m_SlopeClass;
	m_FuelModel = rhs.m_FuelModel;
	m_MC1 = rhs.m_MC1;
	m_MC10 = rhs.m_MC10;
	m_MC100 = rhs.m_MC100;
	m_MC1000 = rhs.m_MC1000;
	m_MCHERB = rhs.m_MCHERB;
	m_MCWOOD = rhs.m_MCWOOD;
	m_PrevYear = rhs.m_PrevYear;
	m_KBDI = rhs.m_KBDI;
	m_YKBDI = rhs.m_YKBDI;
	m_StartKBDI = rhs.m_StartKBDI;
	m_CummPrecip = rhs.m_CummPrecip;
	m_AvgPrecip = rhs.m_AvgPrecip;
	m_UseCuring = rhs.m_UseCuring;
	m_UseLoadTransfer = rhs.m_UseLoadTransfer;
	m_FuelTemperature = rhs.m_FuelTemperature;
	m_BI = rhs.m_BI;
	m_ERC = rhs.m_ERC;
	m_SC = rhs.m_SC;
	m_IC = rhs.m_IC;
	m_GSI = rhs.m_GSI;
	m_nConsectiveSnowDays = rhs.m_nConsectiveSnowDays;
	m_KBDIThreshold = rhs.m_KBDIThreshold;
	m_qPrecip = rhs.m_qPrecip;
	m_qHourlyPrecip = rhs.m_qHourlyPrecip;
	m_qHourlyTemp = rhs.m_qHourlyTemp;
	m_qHourlyRH = rhs.m_qHourlyRH;
	m_lastUtcUpdateTime = rhs.m_lastUtcUpdateTime;
	m_lastDailyUpdateTime = rhs.m_lastDailyUpdateTime;

}

NFDRS4State::NFDRS4State(NFDRS4 *pNFDRS)
{
	m_AvgPrecip = pNFDRS->AvgPrecip;
	m_BI = pNFDRS->BI;
	m_CummPrecip = pNFDRS->CummPrecip;
	m_ERC = pNFDRS->ERC;
	m_FuelModel = pNFDRS->FuelModel;
	m_FuelTemperature = pNFDRS->FuelTemperature;
	m_GSI = pNFDRS->m_GSI;
	m_IC = pNFDRS->IC;
	m_KBDI = pNFDRS->KBDI;
	m_KBDIThreshold = pNFDRS->KBDIThreshold;
	m_lastUtcUpdateTime = pNFDRS->lastUtcUpdateTime;
	m_lastDailyUpdateTime = pNFDRS->lastDailyUpdateTime;
	m_Lat = pNFDRS->Lat;
	m_MC1 = pNFDRS->MC1;
	m_MC10 = pNFDRS->MC10;
	m_MC100 = pNFDRS->MC100;
	m_MC1000 = pNFDRS->MC1000;
	m_MCHERB = pNFDRS->MCHERB;
	m_MCWOOD = pNFDRS->MCWOOD;
	m_nConsectiveSnowDays = pNFDRS->nConsectiveSnowDays;
	m_NFDRSVersion = pNFDRS->NFDRSVersion;
	m_PrevYear = pNFDRS->PrevYear;
	m_SC = pNFDRS->SC;
	m_SlopeClass = pNFDRS->SlopeClass;
	m_StartKBDI = pNFDRS->StartKBDI;
	m_UseCuring = pNFDRS->UseCuring;
	m_UseLoadTransfer = pNFDRS->UseLoadTransfer;
	m_YesterdayJDay = pNFDRS->YesterdayJDay;
	m_YKBDI = pNFDRS->YKBDI;
	float tVal;
	std::deque<double> qCopy = pNFDRS->qPrecip;
	while(qCopy.size() > 0)
	{
		tVal = (float)qCopy.front();
		m_qPrecip.push_back(tVal);
		qCopy.pop_front();
	}
	//hourly deques are always 24 entries
	std::deque<double> qTempCopy = pNFDRS->qHourlyTemp;
	std::deque<double> qRHcopy = pNFDRS->qHourlyRH;
	std::deque<double> qPcpCopy = pNFDRS->qHourlyPrecip;
	for (int h = 0; h < pNFDRS->nHoursPerDay; h++)
	{
		m_qHourlyTemp.push_back((float)qTempCopy.front());
		m_qHourlyRH.push_back((float)qRHcopy.front());
		m_qHourlyPrecip.push_back((float)qPcpCopy.front());
		qTempCopy.pop_front();
		qRHcopy.pop_front();
		qPcpCopy.pop_front();
	}
	m_KBDIThreshold = pNFDRS->KBDIThreshold;
	fm1State = pNFDRS->OneHourFM.GetState();
	fm10State = pNFDRS->TenHourFM.GetState();
	fm100State = pNFDRS->HundredHourFM.GetState();
	fm1000State = pNFDRS->ThousandHourFM.GetState();
	herbState = pNFDRS->HerbFM.GetState();
	woodyState = pNFDRS->WoodyFM.GetState();
}


bool NFDRS4State::LoadState(std::string fileName)
{
	FILE *in = fopen(fileName.c_str(), "rb");
	if (!in)
		return false;

	bool status;
	size_t nRead = fread(&m_NFDRSVersion, sizeof(m_NFDRSVersion), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}

	status = fm1State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm10State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm100State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = fm1000State.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = herbState.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}
	status = woodyState.ReadState(in);
	if (!status)
	{
		fclose(in);
		return false;
	}

	nRead = fread(&m_Lat, sizeof(m_Lat), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_YesterdayJDay, sizeof(m_YesterdayJDay), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_SlopeClass, sizeof(m_SlopeClass), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_FuelModel, sizeof(m_FuelModel), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC1, sizeof(m_MC1), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC10, sizeof(m_MC10), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC100, sizeof(m_MC100), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MC1000, sizeof(m_MC1000), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MCWOOD, sizeof(m_MCWOOD), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_MCHERB, sizeof(m_MCHERB), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_PrevYear, sizeof(m_PrevYear), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_KBDI, sizeof(m_KBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_YKBDI, sizeof(m_YKBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_StartKBDI, sizeof(m_StartKBDI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_KBDIThreshold, sizeof(m_KBDIThreshold), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_CummPrecip, sizeof(m_CummPrecip), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_AvgPrecip, sizeof(m_AvgPrecip), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_UseLoadTransfer, sizeof(m_UseLoadTransfer), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_UseCuring, sizeof(m_UseCuring), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_FuelTemperature, sizeof(m_FuelTemperature), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_BI, sizeof(m_BI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_ERC, sizeof(m_ERC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_SC, sizeof(m_SC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_IC, sizeof(m_IC), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_GSI, sizeof(m_GSI), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	int nPcp;
	nRead = fread(&nPcp, sizeof(nPcp), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	for (int i = 0; i < nPcp; i++)
	{
		float tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
		{
			fclose(in);
			return false;
		}
		m_qPrecip.push_back(tVal);
	}
	//added 2021/01/26 deques, (Temp, RH, Precip) and UTCTimes
	for (int h = 0; h < 24; h++)
	{
		float tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
		{
			fclose(in);
			return false;
		}
		m_qHourlyTemp.push_back(tVal);
	}
	for (int h = 0; h < 24; h++)
	{
		float tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
		{
			fclose(in);
			return false;
		}
		m_qHourlyRH.push_back(tVal);
	}
	for (int h = 0; h < 24; h++)
	{
		float tVal;
		nRead = fread(&tVal, sizeof(tVal), 1, in);
		if (nRead != 1)
		{
			fclose(in);
			return false;
		}
		m_qHourlyPrecip.push_back(tVal);
	}
	nRead = fread(&m_KBDIThreshold, sizeof(m_KBDIThreshold), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	int utcYear, utcMonth, utcDay, utcHour;
	nRead = fread(&utcYear, sizeof(utcYear), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcMonth, sizeof(utcMonth), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcDay, sizeof(utcDay), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcHour, sizeof(utcHour), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	m_lastUtcUpdateTime = utctime::UTCTime(utcYear + 1900, utcMonth + 1, utcDay, utcHour, 0, 0);
	nRead = fread(&utcYear, sizeof(utcYear), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcMonth, sizeof(utcMonth), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcDay, sizeof(utcDay), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	nRead = fread(&utcHour, sizeof(utcHour), 1, in);
	if (nRead != 1)
	{
		fclose(in);
		return false;
	}
	m_lastDailyUpdateTime = utctime::UTCTime(utcYear + 1900, utcMonth + 1, utcDay, utcHour, 0, 0);


	fclose(in);
	return true;
}

bool NFDRS4State::SaveState(std::string fileName)
{
	FILE *out = fopen(fileName.c_str(), "wb");
	if (!out)
		return false;

	bool status;
	//write version first
	size_t nWrite = fwrite(&m_NFDRSVersion, sizeof(m_NFDRSVersion), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	status = fm1State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm10State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm100State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = fm1000State.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = herbState.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	status = woodyState.SaveState(out);
	if (!status)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_Lat, sizeof(m_Lat), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_YesterdayJDay, sizeof(m_YesterdayJDay), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_SlopeClass, sizeof(m_SlopeClass), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_FuelModel, sizeof(m_FuelModel), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC1, sizeof(m_MC1), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC10, sizeof(m_MC10), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC100, sizeof(m_MC100), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MC1000, sizeof(m_MC1000), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MCWOOD, sizeof(m_MCWOOD), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_MCHERB, sizeof(m_MCHERB), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_PrevYear, sizeof(m_PrevYear), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_KBDI, sizeof(m_KBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_YKBDI, sizeof(m_YKBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_StartKBDI, sizeof(m_StartKBDI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_KBDIThreshold, sizeof(m_KBDIThreshold), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_CummPrecip, sizeof(m_CummPrecip), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_AvgPrecip, sizeof(m_AvgPrecip), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_UseLoadTransfer, sizeof(m_UseLoadTransfer), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_UseCuring, sizeof(m_UseCuring), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_FuelTemperature, sizeof(m_FuelTemperature), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_BI, sizeof(m_BI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_ERC, sizeof(m_ERC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_SC, sizeof(m_SC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_IC, sizeof(m_IC), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_GSI, sizeof(m_GSI), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&m_nConsectiveSnowDays, sizeof(m_nConsectiveSnowDays), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	int nPcp = m_qPrecip.size();
	nWrite = fwrite(&nPcp, sizeof(nPcp), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	for (int i = 0; i < nPcp; i++)
	{
		float tVal = m_qPrecip[i];
		nWrite = fwrite(&tVal, sizeof(float), 1, out);
		if (nWrite != 1)
		{
			fclose(out);
			return false;
		}
	}
	//added 2021/01/26 deques and UTCTimes
	for (int h = 0; h < 24; h++)
	{
		float tVal = m_qHourlyTemp[h];
		nWrite = fwrite(&tVal, sizeof(float), 1, out);
		if (nWrite != 1)
		{
			fclose(out);
			return false;
		}
	}
	for (int h = 0; h < 24; h++)
	{
		float tVal = m_qHourlyRH[h];
		nWrite = fwrite(&tVal, sizeof(float), 1, out);
		if (nWrite != 1)
		{
			fclose(out);
			return false;
		}
	}
	for (int h = 0; h < 24; h++)
	{
		float tVal = m_qHourlyPrecip[h];
		nWrite = fwrite(&tVal, sizeof(float), 1, out);
		if (nWrite != 1)
		{
			fclose(out);
			return false;
		}
	}
	nWrite = fwrite(&m_KBDIThreshold, sizeof(m_KBDIThreshold), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	int utcYear, utcMonth, utcDay, utcHour;
	utcYear = m_lastUtcUpdateTime.get_tm().tm_year;
	utcMonth = m_lastUtcUpdateTime.get_tm().tm_mon;
	utcDay = m_lastUtcUpdateTime.get_tm().tm_mday;
	utcHour = m_lastUtcUpdateTime.get_tm().tm_hour;
	nWrite = fwrite(&utcYear, sizeof(utcYear), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcMonth, sizeof(utcMonth), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcDay, sizeof(utcDay), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcHour, sizeof(utcHour), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	utcYear = m_lastDailyUpdateTime.get_tm().tm_year;
	utcMonth = m_lastDailyUpdateTime.get_tm().tm_mon;
	utcDay = m_lastDailyUpdateTime.get_tm().tm_mday;
	utcHour = m_lastDailyUpdateTime.get_tm().tm_hour;
	nWrite = fwrite(&utcYear, sizeof(utcYear), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcMonth, sizeof(utcMonth), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcDay, sizeof(utcDay), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	nWrite = fwrite(&utcHour, sizeof(utcHour), 1, out);
	if (nWrite != 1)
	{
		fclose(out);
		return false;
	}
	fclose(out);
	return true;
}


