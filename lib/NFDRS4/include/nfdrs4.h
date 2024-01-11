#ifndef _NFDRS4_H
#define _NFDRS4_H


// Standard include files
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <numeric>
#include <algorithm>
#include <deque>
#include <unordered_map>
#include "deadfuelmoisture.h"
#include "livefuelmoisture.h"
#include "nfdrs4calcstate.h"
#include "utctime.h"

/*Fuel Model Definition*/
class CFuelModelParams
{
public:
	CFuelModelParams();
	CFuelModelParams(const CFuelModelParams& rhs);
	~CFuelModelParams();
	char getFuelModel();
	const char* getDescription();
	int getSG1();
	int getSG10();
	int getSG100();
	int getSG1000();
	int getSGHerb();
	int getSGWood();
	double getL1();
	double getL10();
	double getL100();
	double getL1000();
	double getLHerb();
	double getLWood();
	double getDepth();
	int getMXD();
	int getHD();
	int getSCM();
	double getLDrought();
	double getWNDFC();
	void setFuelModel(char fm);
	void setDescription(const char* description);
	void setSG1(int sg1);
	void setSG10(int sg10);
	void setSG100(int sg100);
	void setSG1000(int sg1000);
	void setSGHerb(int sgHerb);
	void setSGWood(int sgWood);
	void setL1(double l1);
	void setL10(double l10);
	void setL100(double l100);
	void setL1000(double l1000);
	void setLHerb(double lHerb);
	void setLWood(double lWood);
	void setDepth(double depth);
	void setMXD(int mxd);
	void setHD(int hd);
	void setSCM(int scm);
	void setLDrought(double ldrought);
	void setWNDFC(double wndfc);
private:
	char m_fuelModel;
	std::string m_description;
	int m_SG1;
	int m_SG10;
	int m_SG100;
	int m_SG1000;
	int m_SGHERB;
	int m_SGWOOD;
	double m_L1;
	double m_L10;
	double m_L100;
	double m_L1000;
	double m_LHERB;
	double m_LWOOD;
	double m_DEPTH;
	int m_MXD;
	int m_HD;
	int m_SCM;
	double m_LDROUGHT;
	double m_WNDFC;
};

/**************************** nfdrs4.h ***********************************

First Letter of Variable or Function Name:

Variable Names Used in Function Calls:

    iAdjust			<desired +/- addition to 1-hour fuel moisture>
    iAspect       	1=N, 2=E, 3=S, 4=W
   iBI				Burning Index
   fDEPTH			Fuel Model's depth
   iDoHerbXfer		1=Transfer (appropriate amt of) Herb Load to 1Hr, 0=No
   fERC				Energy Release Component
   iExposure		1=Exposed, 0=Not
   iFIL				Fire Intensity Level
    fFL				Flame Length
   iFM				ASCII Value of Fuel Model ID:  65=A, 66=B, ...
   iHD				Fuel Model's Dead (& Live) Heat of Combustion
   iHour				1, 2, ... 24
   iIC				Ignition Component
   iJulian			Annualized Julian:  1=Jan1,... , 365=Dec31
   iLat				Nearest Degree of Latitude
   iMaxRH			24-Hour Max RH (-999 means missing)
   iMaxTemp			24-Hour Max Temp (F) (-999 means missing)
   fMCn				Fuel Moistures for fuel class "n"
   iMinRH			24-Hour Min RH (-999 means missing)
   iMinTemp			24-Hour Min Temp (F) (-999 means missing)
    iMonth			1=Jan, ... 12=Dec
   iMXD				Fuel Model's Moisture of Extinction (%)
   iNFMAS			1=Use NFMAS Variations, 0=No (Use standard)
   iRH				Observed RH
   fROS				Rate of Spread
   iSAn				Fuel Model's Surface/Area for fuel class "n"
   iSC				Spread Component
   iSCM				Fuel Model's Spread At Which All Ignitiona -> Reportable
   iSite          1=Low, 2=Mid, 3=Upper
   iSlope30       1=30% or less, 2=greater than 30%
   iSlopeCls		NFDRS Slope Class (1-5)
   iSOW				State of Weather (0=Clear, ...) (-1 allowed in iCalcIC)
   iTemp				Observed Temp (F) (Can be fuel temp in iCalcIC)
   fWn				Fuel Model's Loading for fuel class "n"
   fWNDFC			Fuel Model's (or desired) Wind Reduction Factor
   iWS				20-Foot Windspeed


Functions:

    Keywords in function names:

    Calc		Performs a calculation, returns value(s)
    Do			Performs an action
    Get		Returns values
    Set		Sets values

    Some functions return calculated values. Most, especially integer
   typed functions, return "success status": 1=Success, 0=Failure.

   Most requested values are returned as function arguments.


***************************************************************************/

//------------------------------------------------------------------------------
/*! \class NFDRS4
    \brief Main calculator for the US National Fire Danger Rating System components
    and indices.
*/
class NFDRS4
{
    public:
		NFDRS4();
        NFDRS4(double Lat,char FuelModel,int SlopeClass, double AvgAnnPrecip,bool LT,bool Cure, bool IsAnnual);
        ~NFDRS4();
        // Member functions
		//CreateFuelModels called in constructors
		void CreateFuelModels();

		void Init(double Lat, char FuelModel, int SlopeClass, double AvgAnnPrecip, bool LT, bool Cure, bool isAnnual, int kbdiThreshold, int RegObsHour = 13, bool isReinit = false);
      // void Update(Wx);
	   void Update(int Year, int Month, int Day, int Hour, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double MinRH, double PPTAmt, double pcp24, double SolarRad, double WS, bool SnowDay, int RegObsHr);
       void Update(int Year, int Month, int Day, int Hour, double Temp, double RH, double PPTAmt, double SolarRad, double WS, bool SnowDay);
       void UpdateDaily(int Year, int Month, int Day, int Julian, double Temp, double MinTemp, double MaxTemp, double RH, double MinRH, double pcp24, double WS, double fMC1, double fMC10, double fMC100, double fMC1000, double fuelTemp, bool SnowDay/* = false*/);
        //void iSetFuelModel (char cFM);
		bool iSetFuelModel(char cFM);
        int iSetFuelMoistures (double fMC1, double fMC10,double fMC100, double fMC1000, double fMCWood, double fMCHerb, double fuelTempC);
        int iCalcIndexes (int iWS, int iSlopeCls,double* fSC,double* fERC, double* fBI, double* fIC,double fGSI = -999,double fKBDI = -999);
        int iCalcKBDI (double fPrecipAmt, int iMaxTemp,double fCummPrecip, int iYKBDI, double fAvgPrecip);
		double Cure(double fGSI = -999, double fGreenupThreshold = 0.5, double fGSIMax = 1.0);

		void SetGSIParams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false);
		void SetHerbGSIparams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false,
            double herbMin = 30.0, double herbMax = 250.0);
		void SetWoodyGSIparams(double MaxGSI, double GreenupThreshold, double TminMin = -2.0, double TminMax = 5.0, double VPDMin = 900, 
			double VPDMax = 4100, double DaylMin = 36000, double DaylMax = 39600, unsigned int MAPeriod = 21U, bool UseVPDAvg = false, 
			unsigned int nPrecipDays = 30, double rtPrecipMin = 0.5, double rtPrecipMax = 1.5, bool UseRTPrecip = false,
            double woodyMin = 60.0, double woodyMax = 200.0);

        void SetOneHourParams(double radius, double adsorptionRate, double maxLocalMoisture, double stickDensity, double desorptionRate);
        void SetTenHourParams(double radius, double adsorptionRate, double maxLocalMoisture, double stickDensity, double desorptionRate);
        void SetHundredHourParams(double radius, double adsorptionRate, double maxLocalMoisture, double stickDensity, double desorptionRate);
        void SetThousandHourParams(double radius, double adsorptionRate, double maxLocalMoisture, double stickDensity, double desorptionRate);
        void Set1HourRadius(double radius);
        void Set1HourAdsorptionRate(double adsorptionRate);
        void Set1HourStickDensity(double stickDensity);
        void Set1HourMaxLocalMoisture(double maxLocalMoisture);
        void Set1HourDesorptionRate(double desorptionRate);
        void Set10HourRadius(double radius);
        void Set10HourAdsorptionRate(double adsorptionRate);
        void Set10HourStickDensity(double stickDensity);
        void Set10HourMaxLocalMoisture(double maxLocalMoisture);
        void Set10HourDesorptionRate(double desorptionRate);
        void Set100HourRadius(double radius);
        void Set100HourAdsorptionRate(double adsorptionRate);
        void Set100HourStickDensity(double stickDensity);
        void Set100HourMaxLocalMoisture(double maxLocalMoisture);
        void Set100HourDesorptionRate(double desorptionRate);
        void Set1000HourRadius(double radius);
        void Set1000HourAdsorptionRate(double adsorptionRate);
        void Set1000HourStickDensity(double stickDensity);
        void Set1000HourMaxLocalMoisture(double maxLocalMoisture);
        void Set1000HourDesorptionRate(double desorptionRate);

        void SetStartKBDI(int sKBDI);
		int GetStartKBDI();

		void SetSCMax(int maxSC);
		int GetSCMax();
		//void SetMxdHumid(bool isHumid);
        void SetMXD(int mxd);
        int GetMXD();
		//bool GetMxdHumid();
		double GetFuelTemperature();
		double GetXDaysPrecipitation(int nDays);
		bool ReadState(std::string fileName);
		bool SaveState(std::string fileName);
		bool LoadState(NFDRS4State state);
		const int nPrecipQueueDays = 90;
        const int nHoursPerDay = 24;
        double GetMinTemp();
        double GetMaxTemp();
        double GetMinRH();
        double GetPcp24();

        //bool SetCustomFuelModel(CFuelModelParams fmParams);
		void AddCustomFuel(CFuelModelParams fmParams);
		double CTA;
        double Lat;
        int NFDRSVersion;
        int YesterdayJDay;
        int SlopeClass;
       // double AvgAnnPrecip;        // Average Annual Precipitation (inches)
        DeadFuelMoisture OneHourFM;
        DeadFuelMoisture TenHourFM;
        DeadFuelMoisture HundredHourFM;
        DeadFuelMoisture ThousandHourFM;
		LiveFuelMoisture GsiFM;
		LiveFuelMoisture HerbFM;
		LiveFuelMoisture WoodyFM;

		int FuelModel;
		std::string FuelDescription;
        int SG1, SG10, SG100, SG1000, SGWOOD, SGHERB;
        double W1, W10, W100, W1000, WWOOD, WHERB, WDROUGHT, W1P, WHERBP,WTOT;
        double L1, L10, L100, L1000, LWOOD, LHERB, LDROUGHT;
		double DroughtUnit, fctCur;
		double DEPTH, WNDFC;
        int MXD, HD, SCM;
        double MC1, MC10, MC100, MC1000, MCWOOD, MCHERB;
		double BI, ERC, SC, IC;
		int PrevYear;
		int KBDI, YKBDI, StartKBDI, KBDIThreshold;
		double CummPrecip, AvgPrecip;
        bool UseLoadTransfer, UseCuring;
        bool SnowCovered;
        double WTMCD, WTMCL,WTMCDE, WTMCLE;
		double FuelTemperature;
		double m_GSI;
		int nConsectiveSnowDays;
        int m_regObsHour;
        time_t utcHourDiff;
        utctime::UTCTime lastUtcUpdateTime;
        utctime::UTCTime lastDailyUpdateTime;
        std::deque<double> qPrecip;
        std::deque<double> qHourlyPrecip;
        std::deque<double> qHourlyTemp;
        std::deque<double> qHourlyRH;
		std::unordered_map<char, CFuelModelParams> mapFuels;
};


#endif
