// RunNFDRS.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "nfdrs4.h"
#include "RunNFDRSConfiguration.h"
#include "NFDRSConfiguration.h"
#include "CNFDRSParams.h"
#include "fw21.h"
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
using namespace std;

string FormatToISO8061Offset(TM inTm, int offset)
{
	char buf[64];
	sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d%+03d:00", inTm.tm_year + 1900, inTm.tm_mon + 1,
		inTm.tm_mday, inTm.tm_hour, inTm.tm_min, inTm.tm_sec, offset);
	string ret = buf;
	return ret;
}

bool fileExists(const char *fileName)
{
	bool ret = false;
	if (access(fileName, 0) != -1)
		ret = true;
	return ret;
}

 
int main(int argc, char* argv[])
{
	//routine for testing NFDRS
	const char* nfdrsInitFileName = NULL;
	const char* wxFileName = NULL;
	const char* loadStateFileName = NULL;
	const char* saveStateFileName = NULL;
	const char* indexOutputsFileName = NULL;
	const char* allOutputsFileName = NULL;
	const char* fuelMoistureOutputsFileName = NULL;
	CNFDRSParams params;

	int	exitStatus = 0;
	if (argc < 2)
	{
		printf("NFDRS4_cli takes 1 parameter: A path to a NFDRS4_cli configuration file.\n"
			"NFDRS4_cli <configFileName>\n"
		"\twhere configFileName is the complete path to a NFDRS4_cli configuration file\n\n");
		exit(1);
	}
	if (!fileExists(argv[1]))
	{
		printf("Error, file %s does not exist!\n", argv[1]);
		exit(-1);
	}
	RunNFDRSConfiguration *cfg = new RunNFDRSConfiguration();
	try
	{
		cfg->parse(argv[1]);
		nfdrsInitFileName = cfg->getInitFile();
		wxFileName = cfg->getWxFile();
		loadStateFileName = cfg->getLoadStateFile();
		saveStateFileName = cfg->getSaveStateFile();
		allOutputsFileName = cfg->getAllOutputsFile();
		indexOutputsFileName = cfg->getIndexOutputFile();
		fuelMoistureOutputsFileName = cfg->getFuelMoisturesOutputsFile();
	}
	catch(const RunNFDRSConfigurationException & ex)
	{
		fprintf(stderr, "%s\n", ex.c_str());
		delete cfg;
		return -1;
	}

	if (!fileExists(nfdrsInitFileName) && !fileExists(loadStateFileName))
	{
		if(strlen(nfdrsInitFileName) == 0 && strlen(loadStateFileName) == 0)
			printf("Error, either an NFDRSInit file or a loadStateFile must be specified.\n");
		else
		{
			if (strlen(nfdrsInitFileName) > 0)
				printf("NFDRS Init file %s does not exist!\n", nfdrsInitFileName);
			if (strlen(loadStateFileName) > 0)
				printf("NFDRS State file %s does not exist!\n", loadStateFileName);
		}
		delete cfg;
		return -1;
	}

	if (strlen(wxFileName) == 0)
	{
		printf("A wxFile must be specified in the NFDRS4_cli Configuration file\n");
		delete cfg;
		return -3;
	}
	//everything else is optional...
	//first, try to open and parse the NFDRSInit file

	NFDRSConfiguration *nfdrsCfg = new NFDRSConfiguration();
	if (fileExists(nfdrsInitFileName))
	{
		try
		{
			nfdrsCfg->parse(nfdrsInitFileName);
			params = nfdrsCfg->getNFDRSParams();
		}
		catch (NFDRSConfigurationException & ex)
		{
			fprintf(stderr, "%s\n", ex.c_str());
			delete nfdrsCfg;
			delete cfg;
			return -4;
		}
	}
	//at this point we should have everything we need
	NFDRS4 fw21Calc;
	//use NFDRSParams to initialize NFDRS4 object
	params.InitNFDRS(&fw21Calc);
	//do we have a state file?
	if (strlen(loadStateFileName) > 0)
	{
		NFDRS4State state;
		state.LoadState(loadStateFileName);
		fw21Calc.LoadState(state);
	}
	CFW21Data FW21data;
	int status = FW21data.LoadFile(wxFileName, cfg->getStationID(), params.getTimeZoneOffsetHours(), cfg->getUseStoredOutputs() != 0 ? true : false);
	if (status != 0)
	{
		printf("Error loading %s as FW21 file\n", wxFileName);
		if (cfg->getUseStoredOutputs() != 0)
			printf("useStoredOutputs was not zero, is %s an 'allOutputsFile' from a previous NFDRS4_cli run?\n", wxFileName);
		delete nfdrsCfg;
		delete cfg;
		return -5;
	}
	//also need any output files for dumping data
	FILE* allOut = NULL, * indexOut = NULL, * moistOut = NULL, * wxAllOut = NULL;// , * fw21Out = NULL;

	if (allOutputsFileName && strlen(allOutputsFileName) > 0)
	{
		allOut = fopen(allOutputsFileName, "wt");
		if (!allOut)
		{
			printf("Error opening %s as output.\n", allOutputsFileName);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		for (int fieldNum = CFW21Data::FW21_STATION; fieldNum < CFW21Data::FW21_TEMPC; fieldNum++)
		{
			if (fieldNum == 0)
				fprintf(allOut, "%s", CFW21Data::GetFieldName((CFW21Data::FW21FIELDS)fieldNum).c_str());
			else
				fprintf(allOut, ",%s", CFW21Data::GetFieldName((CFW21Data::FW21FIELDS)fieldNum).c_str());
		}
		fprintf(allOut, "\n");
	}
	if (indexOutputsFileName && strlen(indexOutputsFileName) > 0)
	{
		indexOut = fopen(indexOutputsFileName, "wt");
		if (!indexOut)
		{
			
			printf("Error opening %s as output.\n", indexOutputsFileName);
			if (allOut)
				fclose(allOut);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		fprintf(indexOut, "%s", CFW21Data::GetFieldName(CFW21Data::FW21_DATE).c_str());
		for (int f = CFW21Data::FW21_BI; f < CFW21Data::FW21_TEMPC; f++)
			fprintf(indexOut, ",%s", CFW21Data::GetFieldName((CFW21Data::FW21FIELDS)f).c_str());
		fprintf(indexOut, "\n");
	}
	if (fuelMoistureOutputsFileName && strlen(fuelMoistureOutputsFileName) > 0)
	{
		moistOut = fopen(fuelMoistureOutputsFileName, "wt");
		if (!moistOut)
		{

			printf("Error opening %s as output.\n", fuelMoistureOutputsFileName);
			if (allOut)
				fclose(allOut);
			if (indexOut)
				fclose(indexOut);
			delete nfdrsCfg;
			delete cfg;
			return -3;
		}
		fprintf(moistOut, "%s", CFW21Data::GetFieldName(CFW21Data::FW21_DATE).c_str());
		for (int f = CFW21Data::FW21_DFM1; f <= CFW21Data::FW21_FUELTEMPC; f++)
			fprintf(moistOut, ",%s", CFW21Data::GetFieldName((CFW21Data::FW21FIELDS)f).c_str());
		fprintf(moistOut, "\n");
	}

	//now need to read the wxFile and process the records
	time_t startTime = clock();
	for (size_t r = 0; r < FW21data.GetNumRecs(); r++)
	{
		FW21Record fw21Rec = FW21data.GetRec(r);
		if (cfg->getUseStoredOutputs() != 0)
		{
			fw21Calc.iSetFuelMoistures(fw21Rec.GetMx1(), fw21Rec.GetMx10(),
				fw21Rec.GetMx100(), fw21Rec.GetMx1000(), fw21Rec.GetMxHerb(),
				fw21Rec.GetMxWood(), fw21Rec.GetFuelTTempC());
			double tSC, tERC, tBI, tIC;
			fw21Calc.iCalcIndexes(fw21Rec.GetWindSpeed(), params.getSlopeClass(), &tSC, &tERC, &tBI, &tIC, fw21Rec.GetGSI(), fw21Rec.GetKBDI());
			//are these even necessary?????????
			fw21Calc.SC = tSC;
			fw21Calc.ERC = tERC;
			fw21Calc.BI = tBI;
			fw21Calc.IC = tIC;
			fw21Calc.m_GSI = fw21Rec.GetGSI();
			fw21Calc.KBDI = fw21Rec.GetKBDI();
		}
		else
			fw21Calc.Update(fw21Rec.GetYear(), fw21Rec.GetMonth(), fw21Rec.GetDay(), fw21Rec.GetHour(), fw21Rec.GetTemp(), fw21Rec.GetRH(), fw21Rec.GetPrecip(),
				fw21Rec.GetSolarRadiation(), fw21Rec.GetWindSpeed(), fw21Rec.GetSnowFlag());
		if (cfg->getOutputInterval() == 0 || (cfg->getOutputInterval() == 1 && fw21Rec.GetHour() == params.getObsHour()))
		{
			//output to open csv files
			if (allOut)
			{
				fprintf(allOut, "%s,%s,%.1f,%.1f,%.3f,%.1f,%d,%.1f,%d,%.1f,%d,%.10f,"
					"%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
					fw21Rec.GetStation().c_str(),
					FormatToISO8061Offset(fw21Rec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					fw21Rec.GetTemp(), fw21Rec.GetRH(), fw21Rec.GetPrecip(), fw21Rec.GetWindSpeed(), fw21Rec.GetWindAzimuth(), fw21Rec.GetSolarRadiation(),
					fw21Rec.GetSnowFlag(), fw21Rec.GetGustSpeed(), fw21Rec.GetGustAzimuth(), 
					fw21Calc.MC1, fw21Calc.MC10, fw21Calc.MC100, fw21Calc.MC1000, fw21Calc.MCHERB, fw21Calc.MCWOOD, fw21Calc.GetFuelTemperature(),
					fw21Calc.BI, fw21Calc.ERC, fw21Calc.SC, fw21Calc.IC, fw21Calc.m_GSI, fw21Calc.KBDI);
			}
			if (indexOut)
			{
				fprintf(indexOut, "%s,%s,%.2f,%.2f,%.2f,%.2f,%.2f,%d\n",
					fw21Rec.GetStation().c_str(),
					FormatToISO8061Offset(fw21Rec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					fw21Calc.BI, fw21Calc.ERC, fw21Calc.SC, fw21Calc.IC, fw21Calc.m_GSI, fw21Calc.KBDI);
			}
			if (moistOut)
			{
				fprintf(moistOut, "%s,%s,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f,%.10f\n",
					fw21Rec.GetStation().c_str(),
					FormatToISO8061Offset(fw21Rec.GetDateTime(), params.getTimeZoneOffsetHours()).c_str(),
					fw21Calc.MC1, fw21Calc.MC10, fw21Calc.MC100, fw21Calc.MC1000, fw21Calc.MCHERB, fw21Calc.MCWOOD, fw21Calc.GetFuelTemperature());
			}
		}
	}
	time_t endTime = clock();
	double total = endTime - startTime;
	printf("Total seconds time for NFDRS: %.2f\n", total / (double) CLOCKS_PER_SEC);
	if (saveStateFileName && strlen(saveStateFileName) > 0)
	{
		bool success = fw21Calc.SaveState(saveStateFileName);
		if (!success)
			printf("Error saving %s as NFDRS State file\n", saveStateFileName);
	}
	if (allOut)
		fclose(allOut);
	if (indexOut)
		fclose(indexOut);
	if (moistOut)
		fclose(moistOut);
	delete nfdrsCfg;
	delete cfg;
	return exitStatus;
}

