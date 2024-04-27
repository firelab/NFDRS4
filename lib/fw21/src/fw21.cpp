#include "fw21.h"
#include <fstream>
#include <vector>
#include "csv_readrow.h"
#include "utctime.h"
#include <iostream>
#include <iomanip>
#include <cstring>

using namespace std;
using namespace utctime;


TM CFW21Data::ParseISO8061(const string input)
{
	TM thisTime = { 0 };
	//first, need to know if extended or basic ISO 8061 format, and if Zulu time or time zone offset, also if milliseconds are included(but we'll ignore them...)
	bool isExtended = false;
	bool isZulu = false;
	bool hasMillisecs = false;
	size_t tLoc = input.find('T');
	size_t found = input.find('-');
	if (found != string::npos && found < tLoc)
		isExtended = true;
	found = input.find('Z');
	if (found != string::npos)
		isZulu = true;
	found = input.find('.');
	if (found != string::npos)
		hasMillisecs = true;
	//then do appropriate sscanf!
	int nRead;
	int y = -1, M = -1, d = -1, h = -1, m = 0, s = 0, tzh = 0, tzm = 0;
	float ms = 0.0;
	if (isExtended)//has dashes separating fields...
	{
		if (isZulu)
		{
			if(hasMillisecs)
				nRead = sscanf(input.c_str(), "%d-%d-%dT%d:%d:%fZ", &y, &M, &d, &h, &m, &ms);
			else
				nRead = sscanf(input.c_str(), "%d-%d-%dT%d:%d:%dZ", &y, &M, &d, &h, &m, &s);
		}
		else
		{
			if (hasMillisecs)
				nRead = sscanf(input.c_str(), "%d-%d-%dT%d:%d:%f%d%d", &y, &M, &d, &h, &m, &ms, &tzh, &tzm);
			else
				nRead = sscanf(input.c_str(), "%d-%d-%dT%d:%d:%d%3d:%d", &y, &M, &d, &h, &m, &s, &tzh, &tzm);
		}
	}
	else//basic
	{
		if (isZulu)
		{
			if (hasMillisecs)
				nRead = sscanf(input.c_str(), "%4d%2d%2dT%2d%2d%fZ", &y, &M, &d, &h, &m, &ms);
			else
				nRead = sscanf(input.c_str(), "%4d%2d%2dT%2d%2d%2dZ", &y, &M, &d, &h, &m, &s);
		}
		else
		{
			if (hasMillisecs)
				nRead = sscanf(input.c_str(), "%4d%2d%2dT%2d%2d%f%3d%2d", &y, &M, &d, &h, &m, &ms, &tzh, &tzm);
			else
				nRead = sscanf(input.c_str(), "%4d%2d%2dT%2d%2d%2d%3d%2d", &y, &M, &d, &h, &m, &s, &tzh, &tzm);
		}
	}
	if (y < 0 || M <= 0 || M > 12 || d <= 0 || d > 31 || h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59)
	{
		thisTime.tm_year = thisTime.tm_mon = thisTime.tm_mday = thisTime.tm_hour = thisTime.tm_min = thisTime.tm_sec = -1;
		return thisTime;
	}
	//now build the tm
	if (isZulu)//convert to local time
	{
		try
		{
			UTCTime zTime(y, M, d, h, m, s);
			thisTime = zTime.get_tm();
			tm_increment_hour(&thisTime, m_timeZoneOffset);
		}
		catch (invalid_date e)
		{
			//cout << e.what();
		}
		catch (bad_time_init e)
		{

		}
	}
	else
	{
		try
		{
			UTCTime lTime(y, M, d, h, m, s);
			thisTime = lTime.get_tm();
		}
		catch (invalid_date e)
		{
			//cout << e.what();
		}
		catch (bad_time_init e)
		{

		}
	}
	return thisTime;
}

FW21Record::FW21Record()
{
	m_station = "";
	memset(&m_dateTime, 0, sizeof(m_dateTime));
	m_temp = dNODATA;
	m_RH = dNODATA;
	m_pcp = dNODATA;
	m_windSpeed = dNODATA;
	m_windAzimuth = iNODATA;
	m_solarRadiation = dNODATA;
	m_snowFlag = iNODATA;
	m_gustSpeed = dNODATA;
	m_gustAzimuth = iNODATA;
	m_mx1 = dNODATA;
	m_mx10 = dNODATA;
	m_mx100 = dNODATA;
	m_mx1000 = dNODATA;
	m_mxHerb = dNODATA;
	m_mxWood = dNODATA;
	m_fuelTempC = dNODATA;
	m_GSI = dNODATA;
	m_KBDI = iNODATA;
}

FW21Record::FW21Record(const FW21Record& rhs)
{
	m_station = rhs.m_station;
	m_dateTime = rhs.m_dateTime;
	m_temp = rhs.m_temp;
	m_RH = rhs.m_RH;
	m_pcp = rhs.m_pcp;
	m_windSpeed = rhs.m_windSpeed;
	m_windAzimuth = rhs.m_windAzimuth;
	m_solarRadiation = rhs.m_solarRadiation;
	m_snowFlag = rhs.m_snowFlag;
	m_gustSpeed = rhs.m_gustSpeed;
	m_gustAzimuth = rhs.m_gustAzimuth;
	m_mx1 = rhs.m_mx1;
	m_mx10 = rhs.m_mx10;
	m_mx100 = rhs.m_mx100;
	m_mx1000 = rhs.m_mx1000;
	m_mxHerb = rhs.m_mxHerb;
	m_mxWood = rhs.m_mxWood;
	m_fuelTempC = rhs.m_fuelTempC;
	m_GSI = rhs.m_GSI;
	m_KBDI = rhs.m_KBDI;
}
FW21Record::~FW21Record()
{

}

NFDRSDailyRec::NFDRSDailyRec() : FW21Record()
{
	m_minTemp = dNODATA;
	m_maxTemp = dNODATA;
	m_minRH = dNODATA;
	m_pcp24 = dNODATA;
}

NFDRSDailyRec::NFDRSDailyRec(const NFDRSDailyRec& rhs) : FW21Record(rhs)
{
	m_minTemp = rhs.m_minTemp;
	m_maxTemp = rhs.m_maxTemp;
	m_minRH = rhs.m_minRH;
	m_pcp24 = rhs.m_pcp24;
}

NFDRSDailyRec::NFDRSDailyRec(FW21Record rhs)
{
	SetDateTime(rhs.GetDateTime());
	SetTemp(rhs.GetTemp());
	SetRH(rhs.GetRH());
	SetPrecip(rhs.GetPrecip());
	SetWindSpeed(rhs.GetWindSpeed());
	SetSolarRadiation(rhs.GetSolarRadiation());
	SetSnowFlag(rhs.GetSnowFlag());
	m_minTemp = dNODATA;
	m_maxTemp = dNODATA;
	m_minRH = dNODATA;
	m_pcp24 = dNODATA;
}

NFDRSDailyRec::~NFDRSDailyRec()
{

}

vector<string> CFW21Data::m_vFieldNames = { "StationID","DateTime","Temperature(F)","RelativeHumidity(%)","Precipitation(in)",
		"WindSpeed(mph)","WindAzimuth(degrees)","SolarRadiation(W/m2)","SnowFlag","GustSpeed(mph)","GustAzimuth(degrees)",
		"1HourDFM(%)","10HourDFM(%)","100HourDFM(%)",
		"1000HourDFM(%)","HerbLFM(%)","WoodyLFM(%)","FuelTemp(C)",
		//"MinTemp(F)","MaxTemp(F)","MinRH(%)","Pcp24(in)",
		"BI","ERC","SC","IC","GSI","KBDI",
		"Temperature(C)","Precipitation(mm)","WindSpeed(kph)","GustSpeed(kph)"
};

CFW21Data::CFW21Data()
{
	m_fileName = "";
	m_bTimeIsZulu = false;
	m_timeZoneOffset = 0;
}

CFW21Data::CFW21Data(const CFW21Data& rhs)
{//not implemented!!!!

}

CFW21Data::~CFW21Data()
{

}

std::string CFW21Data::GetFieldName(FW21FIELDS fieldNum)
{
	if (fieldNum >= FW21_STATION && fieldNum < FW21_END)
		return CFW21Data::m_vFieldNames[fieldNum];
	return "";
}

int CFW21Data::LoadFile(const char *fw21FileName, std::string station, int tzOffsetHours/* = 0*/, bool needMxFields/* = false*/)
{
	m_timeZoneOffset = tzOffsetHours;
	m_fileName = fw21FileName;
	ifstream stream;
	stream.open(m_fileName);
	if (!stream.is_open())
	{
		printf("Error opening %s as input\n", m_fileName.c_str());
		return -1;
	}
	char buf[1024];
	//get the header line which contains FW12 fields
	int bufSize = 1024;
	stream.getline(buf, bufSize);
	string line = buf;
	vector<string> vFields = csv_read_row(line, ',');
	int nExpectedFields = vFields.size();
	//get field Indexes
	int staIdx, dtIdx, tmpIdx, rhIdx, pcpIdx, wsIdx, wdirIdx, srIdx, snowIdx, gsIdx, gdirIdx, 
		tmpCIdx, pcpmmIdx, wsKphIdx, gsKphIdx, fm1Idx, fm10Idx, fm100Idx, fm1000Idx, fmHerbIdx, fmWoodIdx,
		fuelTempIdx, 
		//minTempFIdx, maxTempFIdx, minRhIdx, pcp24Idx, 
		biIdx, ercIdx, scIdx, icIdx,
		gsiIdx, kbdiIdx;
	staIdx = getColIndex(m_vFieldNames[FW21_STATION], vFields);
	dtIdx = getColIndex(m_vFieldNames[FW21_DATE], vFields);
	tmpIdx = getColIndex(m_vFieldNames[FW21_TEMPF], vFields);
	rhIdx = getColIndex(m_vFieldNames[FW21_RH], vFields);
	pcpIdx = getColIndex(m_vFieldNames[FW21_PCPIN], vFields);
	wsIdx = getColIndex(m_vFieldNames[FW21_WSMPH], vFields);
	wdirIdx = getColIndex(m_vFieldNames[FW21_WAZI], vFields);
	srIdx = getColIndex(m_vFieldNames[FW21_SOLRAD], vFields);
	snowIdx = getColIndex(m_vFieldNames[FW21_SNOWFLAG], vFields);
	gsIdx = getColIndex(m_vFieldNames[FW21_GSMPH], vFields);
	gdirIdx = getColIndex(m_vFieldNames[FW21_GAZI], vFields);
	tmpCIdx = getColIndex(m_vFieldNames[FW21_TEMPC], vFields);
	pcpmmIdx = getColIndex(m_vFieldNames[FW21_PCPMM], vFields);
	wsKphIdx = getColIndex(m_vFieldNames[FW21_WSKPH], vFields);
	gsKphIdx = getColIndex(m_vFieldNames[FW21_GSKPH], vFields);
	fm1Idx = getColIndex(m_vFieldNames[FW21_DFM1], vFields);
	fm10Idx = getColIndex(m_vFieldNames[FW21_DFM10], vFields);
	fm100Idx = getColIndex(m_vFieldNames[FW21_DFM100], vFields);
	fm1000Idx = getColIndex(m_vFieldNames[FW21_DFM1000], vFields);
	fmHerbIdx = getColIndex(m_vFieldNames[FW21_LFMHERB], vFields);
	fmWoodIdx = getColIndex(m_vFieldNames[FW21_LFMWOOD], vFields);
	fuelTempIdx = getColIndex(m_vFieldNames[FW21_FUELTEMPC], vFields);
	biIdx = getColIndex(m_vFieldNames[FW21_BI], vFields);
	ercIdx = getColIndex(m_vFieldNames[FW21_ERC], vFields);
	scIdx = getColIndex(m_vFieldNames[FW21_SC], vFields);
	icIdx = getColIndex(m_vFieldNames[FW21_IC], vFields);
	gsiIdx = getColIndex(m_vFieldNames[FW21_GSI], vFields);
	kbdiIdx = getColIndex(m_vFieldNames[FW21_KBDI], vFields);

	string strStation, strDate, strTemp, strRH, strPcp, strWindSpeed, strWDir, strSolRad, strSnow, strGustSpeed, strGustDir;
	//basic check for required fields
	if (staIdx < 0 || dtIdx < 0 || (tmpIdx < 0 && tmpCIdx < 0) || rhIdx < 0 || (pcpIdx < 0 && pcpmmIdx < 0) || (wsIdx < 0 && wsKphIdx < 0) 
		|| wdirIdx < 0 || srIdx < 0 || snowIdx < 0)
	{
		if(staIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_STATION].c_str());
		if (dtIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_DATE].c_str());
		if (tmpIdx < 0 && tmpCIdx < 0)
			printf("Error, field %s or %s not found in header\n", m_vFieldNames[FW21_TEMPF].c_str(), m_vFieldNames[FW21_TEMPC].c_str());
		if (rhIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_RH].c_str());
		if (pcpIdx < 0 && pcpmmIdx < 0)
			printf("Error, field %s or %s not found in header\n", m_vFieldNames[FW21_PCPIN].c_str(), m_vFieldNames[FW21_PCPMM].c_str());
		if (wsIdx < 0 && wsKphIdx < 0)
			printf("Error, field %s or %s not found in header\n", m_vFieldNames[FW21_WSMPH].c_str(), m_vFieldNames[FW21_WSKPH].c_str());
		if (wdirIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_WAZI].c_str());
		if (srIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_SOLRAD].c_str());
		if (snowIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_SNOWFLAG].c_str());
		printf("Header line is:\n%s\n", buf);
		stream.close();
		return -2;
	}
	if (needMxFields && (fm1Idx < 0 || fm10Idx < 0 || fm100Idx < 0 || fm1000Idx < 0 || fmHerbIdx < 0 
		|| fmWoodIdx < 0 || fuelTempIdx < 0 || gsiIdx < 0))
	{
		if (fm1Idx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_DFM1].c_str());
		if (fm10Idx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_DFM10].c_str());
		if (fm100Idx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_DFM100].c_str());
		if (fm1000Idx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_DFM1000].c_str());
		if (fmHerbIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_LFMHERB].c_str());
		if (fmWoodIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_LFMWOOD].c_str());
		if (fuelTempIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_FUELTEMPC].c_str());
		if(gsiIdx < 0)
			printf("Error, field %s not found in header\n", m_vFieldNames[FW21_GSI].c_str());
		printf("Header line is:\n%s\n", buf);
		stream.close();
		return -3;
	}
	//ok, ready to parse the data...
	bool firstRec = false;
	int lineNo = 2;
	while (stream.good())
	{
		stream.getline(buf, bufSize);
		lineNo++;
		line = buf;
		vFields = csv_read_row(line, ',');
		if (vFields.size() < nExpectedFields)
		{
			printf("Warning, line %d has less than %d fields, skipping record\n", lineNo, nExpectedFields);
			continue;
		}
		//added 4/26/2024 StationID is now required, and we only process one station at a time
		strStation = vFields[staIdx];
		if (station.compare(strStation) != 0)
			continue;

		FW21Record thisRec;
		thisRec.SetStation(strStation);
		strDate = vFields[dtIdx];
		trim(strDate);
		if (strDate.empty())
		{
			printf("Error: DateTime is blank, line %d\n", lineNo);
			continue;
		}
		if (firstRec)
		{
			//need to check for Zulu time
			if (strDate.find('Z') != string::npos)
				m_bTimeIsZulu = true;
			firstRec = false;
		}
		TM recTime = ParseISO8061(strDate);
		if (recTime.tm_mon < 0 || recTime.tm_mday <= 0 || recTime.tm_hour < 0 || recTime.tm_min < 0 || recTime.tm_sec < 0)
		{
			printf("Error, line %d date (%s) is invalid, skipping record\n", lineNo, strDate.c_str());
			continue;
		}
		thisRec.SetDateTime(recTime);
		if (tmpIdx >= 0)//use Fahrenheit if present
		{
			strTemp = vFields[tmpIdx];
			trim(strTemp);
			if (!strTemp.empty())
				thisRec.SetTemp(atof(strTemp.c_str()));
		}
		else if (tmpCIdx >= 0)
		{
			strTemp = vFields[tmpCIdx];
			trim(strTemp);
			if (!strTemp.empty())
				thisRec.SetTemp(atof(strTemp.c_str()) * 1.8 + 32.0);
		}
		else
			strTemp.clear();
		strRH = vFields[rhIdx];
		trim(strRH);
		if (!strRH.empty())
			thisRec.SetRH(max(atof(strRH.c_str()), 1.0));
		if (pcpIdx >= 0)
		{
			strPcp = vFields[pcpIdx];
			trim(strPcp);
			if (!strPcp.empty())
				thisRec.SetPrecip(atof(strPcp.c_str()));
		}
		else if (pcpmmIdx >= 0)
		{
			strPcp = vFields[pcpmmIdx];
			trim(strPcp);
			if (!strPcp.empty())
				thisRec.SetPrecip(atof(strPcp.c_str()) / 25.4);
		}
		else
			strPcp.clear();
		if (wsIdx >= 0)
		{
			strWindSpeed = vFields[wsIdx];
			trim(strWindSpeed);
			if (!strWindSpeed.empty())
				thisRec.SetWindSpeed(atof(strWindSpeed.c_str()));
		}
		else if (wsKphIdx >= 0)
		{
			strWindSpeed = vFields[wsKphIdx];
			trim(strWindSpeed);
			if (!strWindSpeed.empty())
				thisRec.SetWindSpeed((atof(strWindSpeed.c_str()) / 1.15) * 0.6213711922);
		}
		strWDir = vFields[wdirIdx];
		trim(strWDir);
		if(!strWDir.empty())
			thisRec.SetWindAzimuth(atoi(strWDir.c_str()));
		strSolRad = vFields[srIdx];
		trim(strSolRad);
		if (!strSolRad.empty())
			thisRec.SetSolarRadiation(atof(strSolRad.c_str()));
		strSnow = vFields[snowIdx];
		trim(strSnow);
		if (!strSnow.empty())
			thisRec.SetSnowFlag(atoi(strSnow.c_str()));
		else // assume not snow covered
			thisRec.SetSnowFlag(0);
		if (gsIdx >= 0)
		{
			strGustSpeed = vFields[gsIdx];
			trim(strGustSpeed);
			if (!strGustSpeed.empty())
				thisRec.SetGustSpeed(atof(strGustSpeed.c_str()));
		}
		else if (gsKphIdx >= 0)
		{
			strGustSpeed = vFields[gsKphIdx];
			trim(strGustSpeed);
			if (!strGustSpeed.empty())
				thisRec.SetGustSpeed((atof(strGustSpeed.c_str()) / 1.15) * 0.6213711922);
		}
		if (gdirIdx >= 0)
		{
			strGustDir = vFields[gdirIdx];
			trim(strGustDir);
			if (!strGustDir.empty())
				thisRec.SetGustAzimuth(atoi(strGustDir.c_str()));
		}
		bool goodRec = true;
		//first, check for blanks on key fields
		if (strDate.length() <= 0)
		{
			printf("Error: DateTime is blank, line %d\n", lineNo);
			continue;
		}
		if (strTemp.length() <= 0)
		{
			printf("Error: Temperature(F) is blank, line %d, DateTime: %s\n", lineNo, strDate.c_str());
			continue;
		}
		if (strRH.length() <= 0)
		{
			printf("Error: RelativeHumidity(%%) is blank, line %d, DateTime: %s\n", lineNo, strDate.c_str());
			continue;
		}
		if (strPcp.length() <= 0)
		{
			printf("Error: Precipitation(in) is blank, line %d, DateTime: %s\n", lineNo, strDate.c_str());
			continue;
		}
		if (strSolRad.length() <= 0)
		{
			printf("Error: SolarRadiation(W/m2) is blank, line %d, DateTime: %s\n", lineNo, strDate.c_str());
			continue;
		}
		//now some range checks
		if (thisRec.GetTemp() < -76.0 || thisRec.GetTemp() > 140.0)
		{
			printf("Error: Bad Temperature(F) line %d, %.1f, DateTime: %s\n", lineNo, thisRec.GetTemp(), strDate.c_str());
			continue;
		}
		if (thisRec.GetRH() <= 0.0 || thisRec.GetRH() > 100.0)
		{
			printf("Error: Bad RelativeHumidity(%%) line %d, %.1f, DateTime: %s\n", lineNo, thisRec.GetRH(), strDate.c_str());
			continue;
		}
		if (thisRec.GetPrecip() < 0.0 || thisRec.GetPrecip() > 20.0)
		{
			printf("Error: Bad Precipitation(in) line %d, %.1f, DateTime: %s\n", lineNo, thisRec.GetPrecip(), strDate.c_str());
			continue;
		}
		if (thisRec.GetSolarRadiation() < 0.0 || thisRec.GetSolarRadiation() > 2000.0)
		{
			printf("Error: Bad SolarRadiation(W/m2) line %d, %.1f, DateTime: %s\n", lineNo, thisRec.GetSolarRadiation(), strDate.c_str());
			continue;
		}
		//non-fatal warnings
		if (thisRec.GetWindSpeed() < 0.0 || thisRec.GetWindSpeed() > 99.0)
		{
			printf("Warning: Bad WindSpeed(mph) line %d, %.1f, DateTime: %s\n", lineNo, thisRec.GetWindSpeed(), strDate.c_str());
		}
		if (thisRec.GetWindAzimuth() < 0 || thisRec.GetWindAzimuth() > 360)
		{
			printf("Warning: Bad WindAzimuth(degrees) line %d, %d, DateTime: %s\n", lineNo, thisRec.GetWindAzimuth(), strDate.c_str());
		}
		if (needMxFields)
		{
			string fm1, fm10, fm100, fm1000, fmHerb, fmWood, gsi, fuelTemp, kbdi;
			fm1 = vFields[fm1Idx];
			trim(fm1);
			if (!fm1.empty())
				thisRec.SetMx1(atof(fm1.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_DFM1].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fm10 = vFields[fm10Idx];
			trim(fm10);
			if (!fm10.empty())
				thisRec.SetMx10(atof(fm10.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_DFM10].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fm100 = vFields[fm100Idx];
			trim(fm100);
			if (!fm100.empty())
				thisRec.SetMx100(atof(fm100.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_DFM100].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fm1000 = vFields[fm1000Idx];
			trim(fm1000);
			if (!fm1000.empty())
				thisRec.SetMx1000(atof(fm1000.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_DFM1000].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fmHerb = vFields[fmHerbIdx];
			trim(fmHerb);
			if (!fmHerb.empty())
				thisRec.SetMxHerb(atof(fmHerb.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_LFMHERB].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fmWood = vFields[fmWoodIdx];
			trim(fmWood);
			if (!fmWood.empty())
				thisRec.SetMxWood(atof(fmWood.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_LFMWOOD].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			fuelTemp = vFields[fuelTempIdx];
			trim(fuelTemp);
			if (!fuelTemp.empty())
				thisRec.SetFuelTempC(atof(fuelTemp.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_FUELTEMPC].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			gsi = vFields[gsiIdx];
			trim(gsi);
			if (!gsi.empty())
				thisRec.SetGSI(atof(gsi.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_GSI].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
			kbdi = vFields[kbdiIdx];
			trim(kbdi);
			if (!kbdi.empty())
				thisRec.SetKBDI(atoi(kbdi.c_str()));
			else
			{
				printf("Error: %s is blank, line %d, DateTime:: %s\n",
					m_vFieldNames[FW21_KBDI].c_str(),
					lineNo,
					strDate.c_str());
				continue;
			}
		}
		//if we got here record is acceptable
		m_recs.push_back(thisRec);
	}
	stream.close();
	return 0;
}

FW21Record CFW21Data::GetRec(size_t recNum)//zero based! valid: 0->GetNumRecs() - 1
{
	FW21Record ret;
	if (recNum >= 0 && recNum < m_recs.size())
		ret = m_recs[recNum];
	return ret;
}

const double SECS_PER_DAY = 86400.0;

NFDRSDailyRec CFW21Data::GetNFDRSDailyRec(size_t recNum)//zero based! valid: 0->GetNumRecs() - 1
{
	FW21Record rec, rec2;
	if (recNum < 0 || recNum >= m_recs.size())
	{
		NFDRSDailyRec ret;
		return ret;
	}
	rec = GetRec(recNum);
	NFDRSDailyRec goodRec(rec);
	TM trgTime = rec.GetDateTime();
	Time64_T trgTimet = mktime64(&trgTime), thisTimet;
	double tMin = rec.GetTemp(), tMax = rec.GetTemp(), rhMin = rec.GetRH(), pcp = rec.GetPrecip();
	size_t checkRec = recNum - 1;
	while (checkRec >= 0)
	{
		rec2 = GetRec(checkRec);
		TM thisTime = rec2.GetDateTime();
		thisTimet = mktime64(&thisTime);
		if (difftime(trgTimet, thisTimet) >= SECS_PER_DAY)
			break;
		double thisTemp = rec2.GetTemp(), thisRH = rec2.GetRH(), thisPcp = rec2.GetPrecip();
		if (thisTemp != dNODATA)
		{
			if (thisTemp < tMin || tMin == dNODATA)
				tMin = thisTemp;
			if (thisTemp > tMax || tMax == dNODATA)
				tMax = thisTemp;
		}
		if (thisRH != dNODATA)
		{
			if (thisRH < rhMin || rhMin == dNODATA)
				rhMin = thisRH;
		}
		if (thisPcp != dNODATA || pcp == dNODATA)
			pcp += thisPcp;
		checkRec--;
	}
	goodRec.SetMinTemp(tMin);
	goodRec.SetMaxTemp(tMax);
	goodRec.SetMinRH(rhMin);
	goodRec.SetPcp24(pcp);
	return goodRec;
}

int CFW21Data::AddRecord(FW21Record rec)
{
	if (m_recs.size() > 0)//ensure rec is after last record
	{
		FW21Record lastRec = m_recs[m_recs.size() - 1];
		UTCTime lastUtc(lastRec.GetYear(), lastRec.GetMonth(), lastRec.GetDay(), lastRec.GetHour(), lastRec.GetMinutes(), 0);
		UTCTime recUtc(rec.GetYear(), rec.GetMonth(), rec.GetDay(), rec.GetHour(), rec.GetMinutes(), 0);
		if (recUtc <= lastUtc)
		{
			cout << "Error, rectime is <= last record time\n";
			return -1;
		}
	}
	m_recs.push_back(rec);
	return 1;
}

string FormatTM(TM in, int offsetHours)
{
	char buf[64];
	string ret = "";
	if(offsetHours < 0)
		sprintf(buf, "%4ld-%02d-%02dT%02d:%02d:00%03d:00",
			in.tm_year + 1900, in.tm_mon + 1, in.tm_mday,
			in.tm_hour, in.tm_min, offsetHours);
	else
		sprintf(buf, "%4ld-%02d-%02dT%02d:%02d:00%02d:00",
			in.tm_year + 1900, in.tm_mon + 1, in.tm_mday,
			in.tm_hour, in.tm_min, offsetHours);

	ret = buf;
	return ret;
}

int CFW21Data::WriteFile(const char* fw21FileName, int offsetHours)
{
	ofstream out;
	out.open(fw21FileName, ofstream::out | ofstream::trunc);
	//we only output English units, so disregard metric fields
	for (int f = FW21_STATION; f <= FW21_GAZI; f++)
	{
		if(f > FW21_STATION)
			out << ",";
		out << m_vFieldNames[f];
	}
	out << "\n";


	for (vector<FW21Record>::iterator it = m_recs.begin(); it != m_recs.end(); ++it)
	{
		FW21Record rec = *it;
		out << rec.GetStation() << ",";
		string dateStr = FormatTM(rec.GetDateTime(), offsetHours);
		out << dateStr << ",";
		
		out << std::fixed << std::setw(1) << std::setprecision(0) << rec.GetTemp() << ",";
		out << std::fixed << std::setw(1) << std::setprecision(0) << rec.GetRH() << ",";
		out << std::fixed << std::setw(5) << std::setprecision(3) << rec.GetPrecip() << ",";
		out << std::fixed << std::setw(1) << std::setprecision(0) << rec.GetWindSpeed() << ",";
		out << rec.GetWindAzimuth() << ",";
		out << std::fixed << std::setw(1) << std::setprecision(0) << rec.GetSolarRadiation() << ",";
		out << rec.GetSnowFlag() << ",";
		out << std::fixed << std::setw(1) << std::setprecision(0) << rec.GetGustSpeed() << ",";
		out << rec.GetGustAzimuth() << endl;
	}

	out.close();
	return 1;
}
