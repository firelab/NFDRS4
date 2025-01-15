#ifndef LIVEFUELMOISTURE_H
#define LIVEFUELMOISTURE_H
#include <math.h>
#include <vector>
#include <deque>
#include "lfmcalcstate.h"

#define NOVALUE -9999.9
#define RADPERDAY 0.017214
#define RADPERDEG 0.01745329
#define MINDECL -0.4092797
#define SECPERRAD 13750.9871
#define DAYSOFF 10.25

//-----------------------------------------------------------------------------
/*! \page citations References

    \section jolly2005 Jolly, William M, Running, SR, and Rama Nemani. 2005.
    A generalized, bioclimatic index to predict foliar phenology in response to climate
    <i>Glob. Chan. Biol.</i> 11<b>(4)</b>: 619-632.
 */

//------------------------------------------------------------------------------
/*! \class LiveFuelMoisture LiveFuelMoisture.h
    \brief Determines moisture content of live herbaceous and woody fuels
    from ambient air temperature, relative humidity, rainfall and time of year.

    LiveFuelMoisture is an implementation of the GSI-based LFMC model
    

 */
class LiveFuelMoisture
{
    public:
        LiveFuelMoisture();

        LiveFuelMoisture(double Lat,bool IsHerb, bool IsAnnual);
        void Initialize(double Lat,bool IsHerb, bool IsAnnual);
        void SetLimits(double,double,double,double, double, double, double, double);
		void Update(double TempF, double MaxTempF, double MinTempF, double RH, double minRH, int Jday, double RTPrcp, time_t thisTime);
        void SetMAPeriod(unsigned int MAPeriod);
        void SetLFMParameters(double MaxGSI,double GreenupThreshold,double MinLFMVal, double MaxLFMVal);
        void GetLFMParameters(double * MaxGSI,double * GreenupThreshold ,double * MinLFMVal, double * MaxLFMVal);
		void SetNumPrecipDays(int numDays);
		int GetNumPrecipDays();

        double GetMoisture(bool SnowDay = false);
        double CalcRunningAvgGSI();
        double CalcRunningAvgHerbFM(bool SnowDay = false);
        void ResetHerbState();
        double CalcRunningAvgWoodyFM(bool SnowDay = false);
        double CalcGSI(double minRH, double maxTempF, double minTempF, double RTPrcp, double lat, int doy);
        double CalcGSI_VPDAvg(double RH, double TempF, double maxTempF, double minTempF, double RTPrcp, double lat, int doy);
        double GetTminInd(double Tmin);
        double GetVPDInd(double VPD);
        double GetDaylInd(double Dayl);
        double GetPrcpInd(double RTPrcp);
        double CalcDayl(double lat,int yday);
        double CalcVPDavg(double TempDewF, double TempAvgF);
        double CalcVPD(double RH, double TempF);
        double CalcDPT(double tempF, double RH);
        double CalcVP(double tempF);

		double GetMaxGSI();
		double GetGreenupThreshold();
		double GetMinLFMVal();
		double GetMaxLFMVal();

		void SetUseVPDAvg(bool set);
		bool GetUseVPDAvg();
		bool GetIsAnnual();
		LFMCalcState GetState();
		bool SetState(LFMCalcState state);

        void SetUseRTPrecip(bool set);
        bool GetUseRTPrecip();
    private:
		bool m_UseVPDAvg;
        bool m_IsHerb;
        bool m_IsAnnual;
        int m_LFIdaysAvg;
        double m_Lat;
		std::deque<double> qGSI;
        double m_TminMin;
        double m_TminMax;
        double m_VPDMin;
        double m_VPDMax;
        double m_DaylenMin;
        double m_DaylenMax;
        double m_RTPrcpMin;
        double m_RTPrcpMax;
        bool m_useRTPrecip;

        //added for live fuel moistues
        double m_MaxGSI;
        double m_GreenupThreshold;
        double m_MaxLFMVal;
        double m_MinLFMVal;
        double m_Slope;
        double m_Intercept;
        bool hasGreenedUpThisYear;
        bool hasExceeded120ThisYear;
        bool canIncreaseHerb;
        double lastHerbFM;
		int m_nDaysPrecip;
		time_t lastUpdateTime;
};

#endif // LIVEFUELMOISTURE_H
