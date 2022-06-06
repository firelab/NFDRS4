#pragma once

class RunNFDRSConfigurationException
{
public:
	RunNFDRSConfigurationException(const char *str);
	RunNFDRSConfigurationException(const RunNFDRSConfigurationException& other);
	~RunNFDRSConfigurationException();

	const char *c_str() const;//Accessor

private:
	char *m_str;

	//--------
	// Not implemented
	//--------
	RunNFDRSConfigurationException();
	RunNFDRSConfigurationException operator=(const RunNFDRSConfigurationException &);
};

class RunNFDRSConfiguration
{
public:
	RunNFDRSConfiguration(bool wantDiagnostics = false);
	~RunNFDRSConfiguration();

	void parse(
		const char *	cfgInput,
		const char *	cfgScope = "") throw (RunNFDRSConfigurationException);

	//--------
	// Acccessors for configuration variables.
	//--------
	const char *	getInitFile() { return m_initFile; }
	const char *	getWxFile() { return m_wxFile; }
	const char *	getLoadStateFile() { return m_loadStateFile; }
	const char *	getSaveStateFile() { return m_saveStateFile; }
	const char *	getAllOutputsFile() { return m_allOutputsFile; }
	const char *	getIndexOutputFile() { return m_indexOutputFile; }
	const char *	getFuelMoisturesOutputsFile() { return m_fuelMoisturesOutputsFile; }
	int getOutputInterval() { return m_outputInterval; }
private:
	void * m_cfg;
	bool m_wantDiagnostics;

	//instance variables
	const char * m_initFile;
	const char * m_wxFile;
	const char * m_loadStateFile;
	const char * m_saveStateFile;
	const char * m_allOutputsFile;
	const char * m_indexOutputFile;
	const char * m_fuelMoisturesOutputsFile;
	int m_outputInterval;//0 = hourly(each record), 1 = daily
	//--------
	// Not implemented
	//--------
	RunNFDRSConfiguration & operator=(const RunNFDRSConfiguration &);
	RunNFDRSConfiguration(const RunNFDRSConfiguration &);
};

