#pragma once
#include "nfdrs4.h"
class CGSIParams
{
public:
	CGSIParams();
	CGSIParams(const CGSIParams&rhs);
	~CGSIParams();
	//getters
	double getGsiMax() { return m_gsiMax; }
	double getGsiHerbGreenup() { return m_gsiHerbGreenup; }
	double getGsiTminMin() { return m_gsiTminMin; }
	double getGsiTminMax() { return m_gsiTminMax; }
	double getGsiVpdMin() { return m_gsiVpdMin; }
	double getGsiVpdMax() { return m_gsiVpdMax; }
	double getGsiDaylenMin() { return m_gsiDaylenMin; }
	double getGsiDaylenMax() { return m_gsiDaylenMax; }
	int getGsiAveragingPeriod() { return m_gsiAveragingPeriod; }
	bool getUseVpdAverage() { return m_useVPDAverage; }
	int getNumPrecipDays() { return m_numPrecipDays; }
	double getRunningTotalPrecipMin() { return m_runningTotalPcpMin; }
	double getRunningTotalPrecipMax() { return m_runningTotalPcpMax; }
	double getLiveFuelMoistureMin() { return m_liveFuelMoistureMin; }
	double getLiveFuelMoistureMax() { return m_liveFuelMoistureMax; }
	bool getUseRTPrecip() { return m_useRTPrecip; }
	//setters
	void setGsiMax(double gsiMax) { m_gsiMax = gsiMax; }
	void setGsiHerbGreenup(double gsiHerbGreenup) { m_gsiHerbGreenup = gsiHerbGreenup; }
	void setGsiTminMin(double gsiTMinMin) { m_gsiTminMin = gsiTMinMin; }
	void setGsiTminMax(double gsiTMinMax) { m_gsiTminMax = gsiTMinMax; }
	void setGsiVpdMin(double gsiVpdMin) { m_gsiVpdMin = gsiVpdMin; }
	void setGsiVpdMax(double gsiVpdMax) { m_gsiVpdMax = gsiVpdMax; }
	void setGsiDaylenMin(double gsiDaylenMin) { m_gsiDaylenMin = gsiDaylenMin; }
	void setGsiDaylenMax(double gsiDaylenMax) { m_gsiDaylenMax = gsiDaylenMax; }
	void setGsiAveragingPeriod(int gsiAveragingPeriod) { m_gsiAveragingPeriod = gsiAveragingPeriod; }
	void setUseVpdAverage(bool useVPDAverage) { m_useVPDAverage = useVPDAverage; }
	void setNumPrecipDays(int numPrecipDays) { m_numPrecipDays = numPrecipDays; }
	void setRunningTotalPrecipMin(double rtPrecipMin) { m_runningTotalPcpMin = rtPrecipMin; }
	void setRunningTotalPrecipMax(double rtPrecipMax) { m_runningTotalPcpMax = rtPrecipMax; }
	void setLiveFuelMoistureMin(double liveFuelMoistureMin) { m_liveFuelMoistureMin = liveFuelMoistureMin; }
	void setLiveFuelMoistureMax(double liveFuelMoistureMax) { m_liveFuelMoistureMax = liveFuelMoistureMax; }
	void setUseRTPrecip(bool useRTPrecip) { m_useRTPrecip = useRTPrecip; }
private:
	double m_gsiMax;
	double m_gsiHerbGreenup;
	double m_gsiTminMin;
	double m_gsiTminMax;
	double m_gsiVpdMin;
	double m_gsiVpdMax;
	double m_gsiDaylenMin;
	double m_gsiDaylenMax;
	int m_gsiAveragingPeriod;
	bool m_useVPDAverage;
	int m_numPrecipDays;
	double m_runningTotalPcpMin;
	double m_runningTotalPcpMax;
	double m_liveFuelMoistureMin;
	double m_liveFuelMoistureMax;
	bool m_useRTPrecip;
};

class CDeadFuelMoistureParams
{
public:
	CDeadFuelMoistureParams();
	CDeadFuelMoistureParams(const CDeadFuelMoistureParams& rhs);
	~CDeadFuelMoistureParams();

	double getRadius() { return m_radius; }
	double getAdsorptionRate() { return m_adsorptionRate; }
	double getStickDensity() { return m_stickDensity; }
	double getMaxLocalMoisture() { return m_maxLocalMoisture; }
	double getDesorptionRate() { return m_desorptionRate; }
	void setRadius(double radius) { m_radius = radius; }
	void setAdsorptionRate(double adsortionRate) { m_adsorptionRate = adsortionRate; }
	void setStickDensity(double stickDensity) { m_stickDensity = stickDensity; }
	void setMaxLocalMoisture(double maxLocalMoisture) { m_maxLocalMoisture = maxLocalMoisture; }
	void setDesortionRate(double desorptionRate) { m_desorptionRate = desorptionRate; }
private:
	double m_radius;
	double m_adsorptionRate;
	double m_stickDensity;
	double m_maxLocalMoisture;
	double m_desorptionRate;
};

class NFDRS4;

class CNFDRSParams
{
public:

	CNFDRSParams();
	CNFDRSParams(const CNFDRSParams& rhs);
	~CNFDRSParams();

	//to initialize an NFDRS4 object
	void InitNFDRS(NFDRS4* pNFDRS);
	//getters
	//const char* getStationID() { return m_stationID; }
	char getFuelModel() { return m_fuelModel; }
	int getMaxSC() { return m_maxSC; }
	double getLatitude() { return m_latitude; }
	//double getLongitude() { return m_longitude; }
	int getSlopeClass() { return m_slopeClass; }
	double getAvgAnnualPrecip() { return m_avgAnnualPrecip; }
	bool getUseLoadTransfer() { return m_loadTransfer; }
	bool getUseCure() { return m_cure; }
	bool getIsAnnual() { return m_isAnnual; }
	int getKbdiThreshold() { return m_kbdiThreshold; }
	int getStartKbdi() { return m_startKBDI; }
	int getMXD() { return m_MXD; }
	int getObsHour() { return m_obsHour; }
	int getTimeZoneOffsetHours() { return m_timeZoneOffsetHours; }
	CGSIParams getGsiParams() { return m_gsiParams; }
	CGSIParams getHerbParams() { return m_herbParams; }
	CGSIParams getWoodyParams() { return m_woodyParams; }
	CDeadFuelMoistureParams get1HourParams() { return m_1HourParams; }
	CDeadFuelMoistureParams get10HourParams() { return m_10HourParams; }
	CDeadFuelMoistureParams get100HourParams() { return m_100HourParams; }
	CDeadFuelMoistureParams get1000HourParams() { return m_1000HourParams; }
	CFuelModelParams getFuelModelParams() { return m_fuelParams; }
	//setters
	//void setStationID(const char* stationID) { m_stationID = stationID; }
	void setFuelModel(char fuelModel) { m_fuelModel = fuelModel; }
	void setMaxSC(int maxSC) { m_maxSC = maxSC; }
	void setLatitude(double latitude) { m_latitude = latitude; }
	//void setLongitude(double longitude) { m_longitude = longitude; }
	void setSlopeClass(int slopeClass) { m_slopeClass = slopeClass; }
	void setAvgAnnualPrecip(double avgAnnualPrecip) { m_avgAnnualPrecip = avgAnnualPrecip; }
	void setUseLoadTransfer(bool useLoadTransfer) { m_loadTransfer = useLoadTransfer; }
	void setUseCure(bool useCure) { m_cure = useCure; }
	void setIsAnnual(bool isAnnual) { m_isAnnual = isAnnual; }
	void setKbdiThreshold(int kbdiThreshold) { m_kbdiThreshold = kbdiThreshold; }
	void setStartKbdi(int startKbdi) { m_startKBDI = startKbdi; }
	//void setIsHumid(bool isHumid) { m_isHumid = isHumid; }
	void setMXD(int mxd) { m_MXD = mxd; }
	void setObsHour(int obsHour) { m_obsHour = obsHour; }
	void setTimeZoneOffsetHours(int offset){m_timeZoneOffsetHours = offset;}
		void setGsiParams(CGSIParams gsiParams) { m_gsiParams = gsiParams; }
	void setHerbParams(CGSIParams herbParams) { m_herbParams = herbParams; }
	void setWoodyParams(CGSIParams woodyParams) { m_woodyParams = woodyParams; }
	void set1HourParams(CDeadFuelMoistureParams oneHourParams) { m_1HourParams = oneHourParams; }
	void set10HourParams(CDeadFuelMoistureParams tenHourParams) { m_10HourParams = tenHourParams; }
	void set100HourParams(CDeadFuelMoistureParams hundredHourParams) { m_100HourParams = hundredHourParams; }
	void set1000HourParams(CDeadFuelMoistureParams thousandHourParams) { m_1000HourParams = thousandHourParams; }
	void setFuelModelParams(CFuelModelParams fuelModelParams) { m_fuelParams = fuelModelParams; }
private:
	//init params
	//const char* m_stationID;
	char m_fuelModel;
	int m_maxSC;
	double m_latitude;
	//double m_longitude;
	int m_slopeClass;
	double m_avgAnnualPrecip;
	bool m_loadTransfer;
	bool m_cure;
	bool m_isAnnual;
	int m_kbdiThreshold;
	int m_startKBDI;
	//bool m_isHumid;
	int m_MXD;
	//necessary for Update()
	int m_obsHour;
	int m_timeZoneOffsetHours;
	//gsi params
	CGSIParams m_gsiParams;
	CGSIParams m_herbParams;
	CGSIParams m_woodyParams;
	CDeadFuelMoistureParams m_1HourParams;
	CDeadFuelMoistureParams m_10HourParams;
	CDeadFuelMoistureParams m_100HourParams;
	CDeadFuelMoistureParams m_1000HourParams;
	CFuelModelParams  m_fuelParams;

};

