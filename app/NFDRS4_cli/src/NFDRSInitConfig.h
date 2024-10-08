//----------------------------------------------------------------------
// WARNING: This file was generated by config2cpp. Do not edit.
//
// Description: a class providing access to an embedded
//              configuration string.
//----------------------------------------------------------------------
#ifndef NFDRSInitConfig_h
#define NFDRSInitConfig_h

#include <config4cpp/Configuration.h>


class NFDRSInitConfig
{
public:
	//--------
	// Constructor and destructor. Ideally these would be private 
	// but they have to be public so we can initialize the singleton.
	//--------
	NFDRSInitConfig();
	~NFDRSInitConfig();

	//--------
	// Get the configuration string from the singleton object
	//--------
	static const char * getString()
	{
		return s_singleton.m_str.c_str();
	}

	//--------
	// Get the configuration's schema
	//--------
	static void getSchema(const char **& schema, int & schemaSize)
	{
		schema = s_singleton.m_schema;
		schemaSize = 134;
	}
	static const char ** getSchema() // null terminated array
	{
		return s_singleton.m_schema;
	}

private:
	//--------
	// Variables
	//--------
	CONFIG4CPP_NAMESPACE::StringBuffer m_str;
	const char *                       m_schema[135];
	static NFDRSInitConfig s_singleton;

	//--------
	// The following are not implemented
	//--------
	NFDRSInitConfig & operator=(const NFDRSInitConfig &);
	NFDRSInitConfig(const NFDRSInitConfig &);
};


#endif
