#include "NFDRSConfiguration.h"
#include <config4cpp/Configuration.h>
#include <config4cpp/SchemaValidator.h>
#include "NFDRSInitConfig.h"
#include <stdio.h>

using CONFIG4CPP_NAMESPACE::Configuration;
using CONFIG4CPP_NAMESPACE::ConfigurationException;
using CONFIG4CPP_NAMESPACE::SchemaValidator;

NFDRSConfigurationException::NFDRSConfigurationException(const char *str)
{
	m_str = new char[strlen(str) + 2];
	strcpy(m_str, str);

}

NFDRSConfigurationException::NFDRSConfigurationException(const NFDRSConfigurationException& other)
{
	m_str = new char[strlen(other.m_str) + 2];
	strcpy(m_str, other.m_str);
}

NFDRSConfigurationException::~NFDRSConfigurationException()
{
	if (m_str)
		delete[] m_str;
}

const char *NFDRSConfigurationException::c_str() const
{
	return m_str;
}

NFDRSConfiguration::NFDRSConfiguration()
{
	m_wantDiagnostics = false;
	m_cfg = Configuration::create();
}

NFDRSConfiguration::~NFDRSConfiguration()
{
	((Configuration *)m_cfg)->destroy();
}

void NFDRSConfiguration::parse(
	const char* cfgInput,
	const char* cfgScope/* = ""*/)// throw (NFDRSConfigurationException)
{
	Configuration *		cfg = (Configuration*)m_cfg;
	SchemaValidator		sv;

	try
	{
		if (strcmp(cfgInput, "") != 0)
		{
			cfg->parse(cfgInput);
		}
		cfg->setFallbackConfiguration(Configuration::INPUT_STRING,
			NFDRSInitConfig::getString());

		//--------
		// Perform schema validation.
		//--------
		sv.wantDiagnostics(m_wantDiagnostics);
		sv.parseSchema(NFDRSInitConfig::getSchema());
		sv.validate(cfg->getFallbackConfiguration(), "", "",
			SchemaValidator::FORCE_REQUIRED);
		sv.validate(cfg, cfgScope, "");
		//m_nfdrsParams.setStationID(cfg->lookupString(cfgScope, "stationID"));
		m_nfdrsParams.setAvgAnnualPrecip(cfg->lookupFloat(cfgScope, "avgAnnualPrecip"));
		const char *fm = cfg->lookupString(cfgScope, "fuelModel");
		if(strlen(fm) > 0)
			m_nfdrsParams.setFuelModel(fm[0]);
		m_nfdrsParams.setIsAnnual(cfg->lookupInt(cfgScope, "isAnnuals"));
		m_nfdrsParams.setMXD(cfg->lookupInt(cfgScope, "MXD"));
		m_nfdrsParams.setKbdiThreshold(cfg->lookupInt(cfgScope, "kbdiThreshold"));
		m_nfdrsParams.setLatitude(cfg->lookupFloat(cfgScope, "latitude"));
		//m_nfdrsParams.setLongitude(cfg->lookupFloat(cfgScope, "longitude"));
		m_nfdrsParams.setMaxSC(cfg->lookupInt(cfgScope, "maxSC"));
		m_nfdrsParams.setObsHour(cfg->lookupInt(cfgScope, "obsHour"));
		m_nfdrsParams.setSlopeClass(cfg->lookupInt(cfgScope, "slopeClass"));
		m_nfdrsParams.setStartKbdi(cfg->lookupInt(cfgScope, "startKBDI"));
		m_nfdrsParams.setUseCure(cfg->lookupInt(cfgScope, "useCure"));
		m_nfdrsParams.setUseLoadTransfer(cfg->lookupInt(cfgScope, "useLoadTransfer"));
		m_nfdrsParams.setTimeZoneOffsetHours(cfg->lookupInt(cfgScope, "timeZoneOffset"));

		CGSIParams gsiParams;
		gsiParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "gsi_opts.gsiAveragingDays"));
		gsiParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiDayLenMax"));
		gsiParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiDayLenMin"));
		gsiParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "gsi_opts.gsiHerbGreenup"));
		gsiParams.setGsiMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiMax"));
		gsiParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiTminMax"));
		gsiParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiTminMin"));
		gsiParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiVpdMax"));
		gsiParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiVpdMin"));
		gsiParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "gsi_opts.fuelMoistureMax"));
		gsiParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "gsi_opts.fuelMoistureMin"));
		gsiParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "gsi_opts.gsiNumPrecipDays"));
		gsiParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "gsi_opts.gsiRTPrecipMax"));
		gsiParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "gsi_opts.gsiRTPrecipMin"));
		gsiParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "gsi_opts.gsiUseVpdAvg"));
		gsiParams.setUseRTPrecip(cfg->lookupInt(cfgScope, "gsi_opts.gsiUseRTPrecip"));
		m_nfdrsParams.setGsiParams(gsiParams);
		CGSIParams herbParams;
		herbParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "herb_opts.gsiAveragingDays"));
		herbParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiDayLenMax"));
		herbParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiDayLenMin"));
		herbParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "herb_opts.gsiHerbGreenup"));
		herbParams.setGsiMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiMax"));
		herbParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiTminMax"));
		herbParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiTminMin"));
		herbParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiVpdMax"));
		herbParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiVpdMin"));
		herbParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "herb_opts.fuelMoistureMax"));
		herbParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "herb_opts.fuelMoistureMin"));
		herbParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "herb_opts.gsiNumPrecipDays"));
		herbParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "herb_opts.gsiRTPrecipMax"));
		herbParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "herb_opts.gsiRTPrecipMin"));
		herbParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "herb_opts.gsiUseVpdAvg"));
		herbParams.setUseRTPrecip(cfg->lookupInt(cfgScope, "herb_opts.gsiUseRTPrecip"));
		m_nfdrsParams.setHerbParams(herbParams);
		CGSIParams woodyParams;
		woodyParams.setGsiAveragingPeriod(cfg->lookupInt(cfgScope, "woody_opts.gsiAveragingDays"));
		woodyParams.setGsiDaylenMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiDayLenMax"));
		woodyParams.setGsiDaylenMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiDayLenMin"));
		woodyParams.setGsiHerbGreenup(cfg->lookupFloat(cfgScope, "woody_opts.gsiHerbGreenup"));
		woodyParams.setGsiMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiMax"));
		woodyParams.setGsiTminMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiTminMax"));
		woodyParams.setGsiTminMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiTminMin"));
		woodyParams.setGsiVpdMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiVpdMax"));
		woodyParams.setGsiVpdMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiVpdMin"));
		woodyParams.setLiveFuelMoistureMax(cfg->lookupFloat(cfgScope, "woody_opts.fuelMoistureMax"));
		woodyParams.setLiveFuelMoistureMin(cfg->lookupFloat(cfgScope, "woody_opts.fuelMoistureMin"));
		woodyParams.setNumPrecipDays(cfg->lookupInt(cfgScope, "woody_opts.gsiNumPrecipDays"));
		woodyParams.setRunningTotalPrecipMax(cfg->lookupFloat(cfgScope, "woody_opts.gsiRTPrecipMax"));
		woodyParams.setRunningTotalPrecipMin(cfg->lookupFloat(cfgScope, "woody_opts.gsiRTPrecipMin"));
		woodyParams.setUseVpdAverage(cfg->lookupInt(cfgScope, "woody_opts.gsiUseVpdAvg"));
		woodyParams.setUseRTPrecip(cfg->lookupInt(cfgScope, "woody_opts.gsiUseRTPrecip"));
		m_nfdrsParams.setWoodyParams(woodyParams);

		CDeadFuelMoistureParams oneHourParams;
		oneHourParams.setRadius(cfg->lookupFloat(cfgScope, "1hr_opts.radius"));
		oneHourParams.setAdsorptionRate(cfg->lookupFloat(cfgScope, "1hr_opts.adsorptionRate"));
		oneHourParams.setStickDensity(cfg->lookupFloat(cfgScope, "1hr_opts.stickDensity"));
		oneHourParams.setMaxLocalMoisture(cfg->lookupFloat(cfgScope, "1hr_opts.maxLocalMoisture"));
		oneHourParams.setDesortionRate(cfg->lookupFloat(cfgScope, "1hr_opts.desorptionRate"));
		oneHourParams.setMoistureSteps(cfg->lookupInt(cfgScope, "1hr_opts.moistureSteps"));
		oneHourParams.setStickNodes(cfg->lookupInt(cfgScope, "1hr_opts.stickNodes"));
		m_nfdrsParams.set1HourParams(oneHourParams);
		CDeadFuelMoistureParams tenHourParams;
		tenHourParams.setRadius(cfg->lookupFloat(cfgScope, "10hr_opts.radius"));
		tenHourParams.setAdsorptionRate(cfg->lookupFloat(cfgScope, "10hr_opts.adsorptionRate"));
		tenHourParams.setStickDensity(cfg->lookupFloat(cfgScope, "10hr_opts.stickDensity"));
		tenHourParams.setMaxLocalMoisture(cfg->lookupFloat(cfgScope, "10hr_opts.maxLocalMoisture"));
		tenHourParams.setDesortionRate(cfg->lookupFloat(cfgScope, "10hr_opts.desorptionRate"));
		tenHourParams.setMoistureSteps(cfg->lookupInt(cfgScope, "10hr_opts.moistureSteps"));
		tenHourParams.setStickNodes(cfg->lookupInt(cfgScope, "10hr_opts.stickNodes"));
		m_nfdrsParams.set10HourParams(tenHourParams);
		CDeadFuelMoistureParams hundredHourParams;
		hundredHourParams.setRadius(cfg->lookupFloat(cfgScope, "100hr_opts.radius"));
		hundredHourParams.setAdsorptionRate(cfg->lookupFloat(cfgScope, "100hr_opts.adsorptionRate"));
		hundredHourParams.setStickDensity(cfg->lookupFloat(cfgScope, "100hr_opts.stickDensity"));
		hundredHourParams.setMaxLocalMoisture(cfg->lookupFloat(cfgScope, "100hr_opts.maxLocalMoisture"));
		hundredHourParams.setDesortionRate(cfg->lookupFloat(cfgScope, "100hr_opts.desorptionRate"));
		hundredHourParams.setMoistureSteps(cfg->lookupInt(cfgScope, "100hr_opts.moistureSteps"));
		hundredHourParams.setStickNodes(cfg->lookupInt(cfgScope, "100hr_opts.stickNodes"));
		m_nfdrsParams.set100HourParams(hundredHourParams);
		CDeadFuelMoistureParams thousandHourParams;
		thousandHourParams.setRadius(cfg->lookupFloat(cfgScope, "1000hr_opts.radius"));
		thousandHourParams.setAdsorptionRate(cfg->lookupFloat(cfgScope, "1000hr_opts.adsorptionRate"));
		thousandHourParams.setStickDensity(cfg->lookupFloat(cfgScope, "1000hr_opts.stickDensity"));
		thousandHourParams.setMaxLocalMoisture(cfg->lookupFloat(cfgScope, "1000hr_opts.maxLocalMoisture"));
		thousandHourParams.setDesortionRate(cfg->lookupFloat(cfgScope, "1000hr_opts.desorptionRate"));
		thousandHourParams.setMoistureSteps(cfg->lookupInt(cfgScope, "1000hr_opts.moistureSteps"));
		thousandHourParams.setStickNodes(cfg->lookupInt(cfgScope, "1000hr_opts.stickNodes"));
		m_nfdrsParams.set1000HourParams(thousandHourParams);
		if (m_nfdrsParams.getFuelModel() == 'C' || m_nfdrsParams.getFuelModel() == 'c')//custom fuel model expected
		{
			CFuelModelParams fmParams;
			fmParams.setFuelModel(cfg->lookupString(cfgScope, "customFuelModel.FuelModel")[0]);
			fmParams.setDescription(cfg->lookupString(cfgScope, "customFuelModel.Description"));
			fmParams.setSG1(cfg->lookupInt(cfgScope, "customFuelModel.SG1"));
			fmParams.setSG10(cfg->lookupInt(cfgScope, "customFuelModel.SG10"));
			fmParams.setSG100(cfg->lookupInt(cfgScope, "customFuelModel.SG100"));
			fmParams.setSG1000(cfg->lookupInt(cfgScope, "customFuelModel.SG1000"));
			fmParams.setSGHerb(cfg->lookupInt(cfgScope, "customFuelModel.SGHERB"));
			fmParams.setSGWood(cfg->lookupInt(cfgScope, "customFuelModel.SGWOOD"));
			fmParams.setL1(cfg->lookupFloat(cfgScope, "customFuelModel.L1"));
			fmParams.setL10(cfg->lookupFloat(cfgScope, "customFuelModel.L10"));
			fmParams.setL100(cfg->lookupFloat(cfgScope, "customFuelModel.L100"));
			fmParams.setL1000(cfg->lookupFloat(cfgScope, "customFuelModel.L1000"));
			fmParams.setLHerb(cfg->lookupFloat(cfgScope, "customFuelModel.LHERB"));
			fmParams.setLWood(cfg->lookupFloat(cfgScope, "customFuelModel.LWOOD"));
			fmParams.setHD(cfg->lookupInt(cfgScope, "customFuelModel.HD"));
			fmParams.setDepth(cfg->lookupFloat(cfgScope, "customFuelModel.DEPTH"));
			fmParams.setLDrought(cfg->lookupFloat(cfgScope, "customFuelModel.LDROUGHT"));
			fmParams.setMXD(cfg->lookupInt(cfgScope, "customFuelModel.MXD"));
			fmParams.setSCM(cfg->lookupInt(cfgScope, "customFuelModel.SCM"));
			fmParams.setWNDFC(cfg->lookupFloat(cfgScope, "customFuelModel.WNDFC"));
			m_nfdrsParams.setFuelModelParams(fmParams);
		}
		//let's try dump to see what we get!
		//saveAs("Dummyfile.cfg");
	}
	catch (const ConfigurationException & ex) {
		throw NFDRSConfigurationException(ex.c_str());
		//throw ConfigurationException(ex.c_str());
		printf("%s\n", ex.c_str());
	}
}

void NFDRSConfiguration::saveAs(const char *fileName)
{
	CONFIG4CPP_NAMESPACE::StringBuffer strBuf;
	Configuration *		cfg = (Configuration*)m_cfg;
	cfg->dump(strBuf, false);
	//printf("%s\n", strBuf.c_str());
}
