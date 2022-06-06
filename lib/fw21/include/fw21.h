#pragma once
#include <string>
#include <ctime>
#include <vector>

const int iNODATA = -999;
const double dNODATA = -999.0;
//this is the raw record we read from FW21 file
class FW21Record
{
public:
	FW21Record();
	FW21Record(const FW21Record& rhs);
	~FW21Record();

	//accessors
	tm GetDateTime() { return m_dateTime; }
	int GetYear() { return m_dateTime.tm_year + 1900;}
	int GetMonth() { return m_dateTime.tm_mon + 1;}
	int GetDay() { return m_dateTime.tm_mday; }
	int GetHour() { return m_dateTime.tm_hour; }
	int GetMinutes() { return m_dateTime.tm_min; }
	int GetSeconds() { return m_dateTime.tm_sec; }
	double GetTemp() { return m_temp; }
	double GetRH() { return m_RH; }
	double GetPrecip() { return m_pcp; }
	double GetWindSpeed() { return m_windSpeed; }
	int GetWindAzimuth() { return m_windAzimuth;	}
	double GetSolarRadiation() { return( m_solarRadiation != dNODATA) ? m_solarRadiation : 0; }
	int GetSnowFlag() { return (m_snowFlag != dNODATA) ? m_snowFlag : 0; }
	double GetGustSpeed() { return m_gustSpeed; }
	int GetGustAzimuth() { return m_gustAzimuth; }

	//setters
	void SetDateTime(tm dateTime) { m_dateTime = dateTime; }
	void SetTemp(double temp) { m_temp = temp; }
	void SetRH(double rh) { m_RH = rh; }
	void SetPrecip(double pcp) { m_pcp = pcp; }
	void SetWindSpeed(double ws) { m_windSpeed = ws; }
	void SetWindAzimuth(int azi) { m_windAzimuth = azi; }
	void SetSolarRadiation(double solrad) { m_solarRadiation = solrad; }
	void SetSnowFlag(int snowDay) { m_snowFlag = snowDay; }
	void SetGustSpeed(double gs) { m_gustSpeed = gs; }
	void SetGustAzimuth(int azi) { m_gustAzimuth = azi; }
	 
private:
	tm m_dateTime;
	double m_temp;//always stored in degrees F
	double m_RH;
	double m_pcp; //always stored in inches
	double m_windSpeed; //always stored in MPH
	int m_windAzimuth;
	double m_solarRadiation;
	int m_snowFlag;
	double m_gustSpeed;//always stored in MPH
	int m_gustAzimuth;
};

//record needed to pass to NFDRS4::Update()
class NFDRSDailyRec : public FW21Record
{
public:
	NFDRSDailyRec();
	NFDRSDailyRec(const NFDRSDailyRec& rhs);
	NFDRSDailyRec(FW21Record rhs);
	~NFDRSDailyRec();

	//getters
	double GetMinTemp() { return m_minTemp; }
	double GetMaxTemp() { return m_maxTemp; }
	double GetMinRH() { return m_minRH; }
	double GetPcp24() { return m_pcp24; }
	//setters
	void SetMinTemp(double minT) { m_minTemp = minT; }
	void SetMaxTemp(double maxT) { m_maxTemp = maxT; }
	void SetMinRH(double minRH) { m_minRH = minRH; }
	void SetPcp24(double pcp24) { m_pcp24 = pcp24; }
private:
	double m_minTemp;
	double m_maxTemp;
	double m_minRH;
	double m_pcp24;
};

class CFW21Data
{
public:
	CFW21Data();
	CFW21Data(const CFW21Data& rhs);
	~CFW21Data();

	int LoadFile(const char *fw21FileName, int tzOffsetHours = 0);
	FW21Record GetRec(size_t recNum);//zero based! valid: 0->GetNumRecs() - 1
	NFDRSDailyRec GetNFDRSDailyRec(size_t recNum);//zero based! valid: 0->GetNumRecs() - 1
	size_t GetNumRecs() { return m_recs.size(); }
	bool TimeIsZulu() {return m_bTimeIsZulu; }
	tm ParseISO8061(const std::string input);
	int AddRecord(FW21Record rec);
	int WriteFile(const char* fw21FileName, int offsetHours);

private:
	std::string m_fileName;
	std::vector< FW21Record> m_recs;
	bool m_bTimeIsZulu;
	int m_timeZoneOffset;
	//this enum is used to match string in m_vFieldNames
	enum FW21FIELDS  { FW21_DATE, FW21_TEMPF, FW21_RH, FW21_PCPIN, FW21_WSMPH, FW21_WAZI, FW21_SOLRAD, 
		FW21_SNOWFLAG, FW21_GSMPH, FW21_GAZI, FW21_TEMPC, FW21_PCPMM, FW21_WSKPH, FW21_GSKPH};
	//ensure field names match enum values if any additions made
	std::vector<std::string> m_vFieldNames = { "DateTime","Temperature(F)","RelativeHumidity(%)","Precipitation(in)",
		"WindSpeed(mph)","WindAzimuth(degrees)","SolarRadiation(W/m2)","SnowFlag","GustSpeed(mph)","GustAzimuth(degrees)",
		"Temperature(C)","Precipitation(mm)","WindSpeed(kph)","GustSpeed(kph)"
	};

};

