#include "nfdrs4.h"
#include "CNFDRSParams.h"

CGSIParams::CGSIParams()
{
	m_gsiMax = 1.0;
	m_gsiHerbGreenup = 0.5;
	m_gsiTminMin = -2.0;
	m_gsiTminMax = 5.0;
	m_gsiVpdMin = 900.0;
	m_gsiVpdMax = 4100.0;
	m_gsiDaylenMin = 36000.0;
	m_gsiDaylenMax = 39600.0;
	m_gsiAveragingPeriod = 21;
	m_useVPDAverage = false;
	m_numPrecipDays = 30;
	m_runningTotalPcpMin = 0.5;
	m_runningTotalPcpMax = 1.5;
	m_liveFuelMoistureMin = 30.0;
	m_liveFuelMoistureMax = 250.0;
	m_useRTPrecip = false;
}

CGSIParams::CGSIParams(const CGSIParams&rhs)
{
	m_gsiMax = rhs.m_gsiMax;
	m_gsiHerbGreenup = rhs.m_gsiHerbGreenup;
	m_gsiTminMin = rhs.m_gsiTminMin;
	m_gsiTminMax = rhs.m_gsiTminMax;
	m_gsiVpdMin = rhs.m_gsiVpdMin;
	m_gsiVpdMax = rhs.m_gsiVpdMax;
	m_gsiDaylenMin = rhs.m_gsiDaylenMin;
	m_gsiDaylenMax = rhs.m_gsiDaylenMax;
	m_gsiAveragingPeriod = rhs.m_gsiAveragingPeriod;
	m_useVPDAverage = rhs.m_useVPDAverage;
	m_numPrecipDays = rhs.m_numPrecipDays;
	m_runningTotalPcpMin = rhs.m_runningTotalPcpMin;
	m_runningTotalPcpMax = rhs.m_runningTotalPcpMax;
	m_liveFuelMoistureMin = rhs.m_liveFuelMoistureMin;
	m_liveFuelMoistureMax = rhs.m_liveFuelMoistureMax;
	m_useRTPrecip = rhs.m_useRTPrecip;
}

CGSIParams::~CGSIParams()
{
}

CDeadFuelMoistureParams::CDeadFuelMoistureParams()
{
	m_radius = 0.64;
	m_adsorptionRate = 0.079548303;
	m_stickDensity = 0.4;
	m_maxLocalMoisture = 0.35;
	m_desorptionRate = 0.06;
}

CDeadFuelMoistureParams::CDeadFuelMoistureParams(const CDeadFuelMoistureParams& rhs)
{
	m_radius = rhs.m_radius;
	m_adsorptionRate = rhs.m_adsorptionRate;
	m_stickDensity = rhs.m_stickDensity;
	m_maxLocalMoisture = rhs.m_maxLocalMoisture;
	m_desorptionRate = rhs.m_desorptionRate;
}

CDeadFuelMoistureParams::~CDeadFuelMoistureParams()
{

}

CNFDRSParams::CNFDRSParams()
{
	m_fuelModel = 'Y';
	m_maxSC = 5;
	m_latitude = 47.0;
	m_slopeClass = 1;
	m_avgAnnualPrecip = 13.2;
	m_loadTransfer = true;
	m_cure = true;
	m_isAnnual = true;
	m_kbdiThreshold = 100;
	m_startKBDI = 100;
	m_isHumid = false;
	m_obsHour = 13;
	m_timeZoneOffsetHours = 0;
	m_herbParams.setLiveFuelMoistureMin(30);
	m_herbParams.setLiveFuelMoistureMax(250);
	m_woodyParams.setLiveFuelMoistureMin(60);
	m_woodyParams.setLiveFuelMoistureMax(200);
}

CNFDRSParams::CNFDRSParams(const CNFDRSParams& rhs)
{
	m_fuelModel = rhs.m_fuelModel;
	m_maxSC = rhs.m_maxSC;
	m_latitude = rhs.m_latitude;
	m_slopeClass = rhs.m_slopeClass;
	m_avgAnnualPrecip = rhs.m_avgAnnualPrecip;
	m_loadTransfer = rhs.m_loadTransfer;
	m_cure = rhs.m_cure;
	m_isAnnual = rhs.m_isAnnual;
	m_kbdiThreshold = rhs.m_kbdiThreshold;
	m_startKBDI = rhs.m_startKBDI;
	m_isHumid = rhs.m_isHumid;
	m_obsHour = rhs.m_obsHour;
	m_timeZoneOffsetHours = rhs.m_timeZoneOffsetHours;
	m_gsiParams = rhs.m_gsiParams;
	m_herbParams = rhs.m_herbParams;
	m_woodyParams = rhs.m_woodyParams;
	m_1HourParams = rhs.m_1HourParams;
	m_10HourParams = rhs.m_10HourParams;
	m_100HourParams = rhs.m_100HourParams;
	m_1000HourParams = rhs.m_1000HourParams;
}

CNFDRSParams::~CNFDRSParams()
{

}

void CNFDRSParams::InitNFDRS(NFDRS4* pNFDRS)
{
	if (!pNFDRS)
		return;
	pNFDRS->Init(getLatitude(), getFuelModel(), getSlopeClass(), getAvgAnnualPrecip(), getUseLoadTransfer(), getUseCure(),
		getIsAnnual(), getKbdiThreshold());
	pNFDRS->iSetFuelModel(getFuelModel());
	pNFDRS->SetOneHourParams(m_1HourParams.getRadius(), m_1HourParams.getAdsorptionRate(), m_1HourParams.getMaxLocalMoisture(), m_1HourParams.getStickDensity(), m_1HourParams.getDesorptionRate());
	pNFDRS->SetTenHourParams(m_10HourParams.getRadius(), m_10HourParams.getAdsorptionRate(), m_10HourParams.getMaxLocalMoisture(), m_10HourParams.getStickDensity(), m_10HourParams.getDesorptionRate());
	pNFDRS->SetHundredHourParams(m_100HourParams.getRadius(), m_100HourParams.getAdsorptionRate(), m_100HourParams.getMaxLocalMoisture(), m_100HourParams.getStickDensity(), m_100HourParams.getDesorptionRate());
	pNFDRS->SetThousandHourParams(m_1000HourParams.getRadius(), m_1000HourParams.getAdsorptionRate(), m_1000HourParams.getMaxLocalMoisture(), m_1000HourParams.getStickDensity(), m_1000HourParams.getDesorptionRate());
	CGSIParams gsi = getGsiParams();
	pNFDRS->SetGSIParams(gsi.getGsiMax(), gsi.getGsiHerbGreenup(), gsi.getGsiTminMin(), gsi.getGsiTminMax(), gsi.getGsiVpdMin(),
		gsi.getGsiVpdMax(), gsi.getGsiDaylenMin(), gsi.getGsiDaylenMax(), gsi.getGsiAveragingPeriod(),
		gsi.getUseVpdAverage(), gsi.getNumPrecipDays(), gsi.getRunningTotalPrecipMin(), gsi.getRunningTotalPrecipMax(), gsi.getUseRTPrecip());
	CGSIParams herb = getHerbParams();
	pNFDRS->SetHerbGSIparams(herb.getGsiMax(), herb.getGsiHerbGreenup(), herb.getGsiTminMin(), herb.getGsiTminMax(), herb.getGsiVpdMin(),
		herb.getGsiVpdMax(), herb.getGsiDaylenMin(), herb.getGsiDaylenMax(), herb.getGsiAveragingPeriod(),
		herb.getUseVpdAverage(), herb.getNumPrecipDays(), herb.getRunningTotalPrecipMin(), herb.getRunningTotalPrecipMax(), herb.getUseRTPrecip(),
		herb.getLiveFuelMoistureMin(), herb.getLiveFuelMoistureMax());
	CGSIParams woody = getWoodyParams();
	pNFDRS->SetWoodyGSIparams(woody.getGsiMax(), woody.getGsiHerbGreenup(), woody.getGsiTminMin(), woody.getGsiTminMax(), woody.getGsiVpdMin(),
		woody.getGsiVpdMax(), woody.getGsiDaylenMin(), woody.getGsiDaylenMax(), woody.getGsiAveragingPeriod(),
		woody.getUseVpdAverage(), woody.getNumPrecipDays(), woody.getRunningTotalPrecipMin(), woody.getRunningTotalPrecipMax(), woody.getUseRTPrecip(),
		woody.getLiveFuelMoistureMin(), woody.getLiveFuelMoistureMax());
	pNFDRS->SetStartKBDI(getStartKbdi());
	if (getMaxSC() > 0)
		pNFDRS->SetSCMax(getMaxSC());
	pNFDRS->SetMxdHumid(getIsHumid());
}
