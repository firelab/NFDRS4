#pragma once
#include "CNFDRSParams.h"

class NFDRSConfigurationException
{
public:
	NFDRSConfigurationException(const char *str);
	NFDRSConfigurationException(const NFDRSConfigurationException& other);
	~NFDRSConfigurationException();

	const char *c_str() const;//Accessor

private:
	char * m_str;

//--------
// Not implemented
//--------
	NFDRSConfigurationException();
	NFDRSConfigurationException operator = (const NFDRSConfigurationException &);
};

class NFDRSConfiguration
{
public:
	NFDRSConfiguration();
	~NFDRSConfiguration();
	void saveAs(const char *fileName);
	void parse(
		const char *	cfgInput,
		const char *	cfgScope = "") throw (NFDRSConfigurationException);

	const CNFDRSParams getNFDRSParams() { return m_nfdrsParams; }

private:
	void * m_cfg;
	bool m_wantDiagnostics;

	CNFDRSParams m_nfdrsParams;

};

