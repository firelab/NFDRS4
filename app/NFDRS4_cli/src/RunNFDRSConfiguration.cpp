#include "RunNFDRSConfiguration.h"
#include "config4cpp/Configuration.h"
#include "config4cpp/SchemaValidator.h"
#include <stdio.h>
#include <string.h>

using CONFIG4CPP_NAMESPACE::Configuration;
using CONFIG4CPP_NAMESPACE::ConfigurationException;
using CONFIG4CPP_NAMESPACE::SchemaValidator;

//class RunNFDRSConfigurationException
RunNFDRSConfigurationException::RunNFDRSConfigurationException(const char *str)
{
	m_str = new char[strlen(str) + 1];
	strcpy(m_str, str);

}

RunNFDRSConfigurationException::RunNFDRSConfigurationException(const RunNFDRSConfigurationException& other)
{
	m_str = new char[strlen(other.m_str) + 1];
	strcpy(m_str, other.m_str);
}

RunNFDRSConfigurationException::~RunNFDRSConfigurationException()
{
	if (m_str)
		delete[] m_str;
}

const char *RunNFDRSConfigurationException::c_str() const
{
	return m_str;
}

//class RunNFDRSConfiguration

RunNFDRSConfiguration::RunNFDRSConfiguration(bool wantDiagnostics/* = false*/)
{
	m_wantDiagnostics = wantDiagnostics;
	m_cfg = Configuration::create();
}

RunNFDRSConfiguration::~RunNFDRSConfiguration()
{
	((Configuration *)m_cfg)->destroy();
	m_initFile = "";
	m_wxFile = "";
	m_loadStateFile = "";
	m_saveStateFile = "";
	m_allOutputsFile = "";
	m_indexOutputFile = "";
	m_fuelMoisturesOutputsFile = "";
	m_outputInterval = 0;//default to hourly
}

void RunNFDRSConfiguration::parse(
	const char *	cfgInput,
	const char *	cfgScope/* = ""*/)// throw (RunNFDRSConfigurationException)
{
	Configuration *		cfg = (Configuration*)m_cfg;
	SchemaValidator		sv;

	try 
	{
		if (strcmp(cfgInput, "") != 0) 
		{
			cfg->parse(cfgInput);
		}

		//--------
		// Perform schema validation.
		//--------
		sv.wantDiagnostics(m_wantDiagnostics);
		//sv.parseSchema(RunNFDRSConfiguration::getSchema());
		//sv.validate(cfg->getFallbackConfiguration(), "", "",
		//	SchemaValidator::FORCE_REQUIRED);
		//sv.validate(cfg, cfgScope, "");
		m_initFile = cfg->lookupString(cfgScope, "initFile");
		m_wxFile = cfg->lookupString(cfgScope, "wxFile");
		m_loadStateFile = cfg->lookupString(cfgScope, "loadFromStateFile");
		m_saveStateFile = cfg->lookupString(cfgScope, "saveToStateFile");
		m_allOutputsFile = cfg->lookupString(cfgScope, "allOutputsFile");
		m_indexOutputFile = cfg->lookupString(cfgScope, "indexOutputFile");
		m_fuelMoisturesOutputsFile = cfg->lookupString(cfgScope, "fuelMoisturesOutputFile");
		m_outputInterval = cfg->lookupInt(cfgScope, "outputInterval");
	}
	catch (const ConfigurationException & ex) {
		//do nothing but print the message
		//throw RunNFDRSConfigurationException(ex.c_str());
		printf("%s\n", ex.c_str());
	}

}

