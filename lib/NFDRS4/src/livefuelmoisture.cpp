//------------------------------------------------------------------------------
/*! \file LiveFuelMoisture.cpp
    \brief LiveFuelMoisture class definition and implementation.
    \author Copyright (C) 2015 by W. Matt Jolly, USFS, RMRS, Fire Sciences Laboratory
    \version 1.0.0 - Uses C++ and STL only.

    \par Licensed under GNU GPL
    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    If you have not received a copy of the GNU General Public License
    along with this program; write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
    The GNU GPL is also available in full from
    http://www.opensource.org/licenses/gpl-license.php
 */


#include <algorithm>
#include <iostream>
#include<numeric>

#include "livefuelmoisture.h"
//#include <ctime>

using namespace std;

void LiveFuelMoisture::Initialize(double Lat,bool IsHerb, bool IsAnnual = false)
{
    m_Lat = Lat;
    m_IsHerb = IsHerb;
    m_IsAnnual = IsAnnual;

    /*if(m_IsHerb)
    {
        SetLFMParameters(1.0,0.5,30,250);
    }
    else
    {
       SetLFMParameters(1.0,0.5,60,200);
    }*/
	//if (iGSI.size() > 0)
	//	iGSI.clear();
	while (qGSI.size() > 0)
		qGSI.pop_front();

	//while (qPrecip.size() > 0)
	//	qPrecip.pop();
}


//------------------------------------------------------------------------------
/*! \brief Method to retrieve calculated live fuel moisture for a given instance of the class.
    \param[in] NONE.
    \return Calculated live fuel moisture (% dry wt)
 */
double LiveFuelMoisture::GetMoisture(bool SnowDay)
{
     if (m_IsHerb)
     {
         return CalcRunningAvgHerbFM(SnowDay);
     }
     else
     {
         return CalcRunningAvgWoodyFM(SnowDay);
     }
}


void LiveFuelMoisture::Update(double TempF, double MaxTempF, double MinTempF, double RH, double MinRH, int Jday, double RTPrcp, time_t thisTime)
{
	double GSI;
	if (!m_UseVPDAvg)
		GSI = CalcGSI(MinRH, MaxTempF, MinTempF, RTPrcp,m_Lat, Jday);
	else
		GSI = CalcGSI_VPDAvg(RH, TempF, MaxTempF, MinTempF, RTPrcp, m_Lat, Jday);
	//cout << "iGSI: " << GSI << " " << CalcRunningAvgGSI() << " " << m_MaxGSI << " " << endl;
	//iGSI.push_back(GSI);
	int gDays = 0, days = 0, pDays = 0;
	if (lastUpdateTime != 0)
	{
        int secs = thisTime - lastUpdateTime;
        days = secs / 86400;//86400 seconds per day
		if (days > 1)//gap, deal with it by removing extra values
		{
			gDays = min(days - 1, (int)qGSI.size());
			for (int p = 0; p < gDays; p++)
				qGSI.pop_front();
		}
	}
	qGSI.push_back(GSI);
	while (qGSI.size() > m_LFIdaysAvg)
		qGSI.pop_front();
	lastUpdateTime = thisTime;
}

//------------------------------------------------------------------------------
/*! \brief Update function for GSI-based fuel moistures based on daily weather data.
    Note: Latitude is set during initialization.
    \param[in] RH: Relative Humidity (1-100%).
    \param[in] TempF: Air temperature (F).
    \param[in] MinTempF: 24-hour minimum temperature (F)
    \param[in] Jday: Day of Year (1-366)
	\param[in] Precip24: 24 hour precipitation
    \return NONE
 */
/*void LiveFuelMoisture::Update(double RH, double TempF, double MinTempF,double Jday, double Precip24, time_t thisTime)
{


	double GSI = CalcGSI(RH, TempF, MinTempF, m_Lat, Jday);
   //cout << "iGSI: " << GSI << " " << CalcRunningAvgGSI() << " " << m_MaxGSI << " " << endl;
    //iGSI.push_back(GSI);
	qGSI.push(GSI);
	while (qGSI.size() > m_LFIdaysAvg)
		qGSI.pop();
	qPrecip.push(Precip24);
	while (qPrecip.size() > nPrecipQueueDays)
		qPrecip.pop();
}*/
//------------------------------------------------------------------------------
/*! \brief Set the environmental limits for the GSI calculations
    This function defines the environmental limits for the minimum temperature,
    vapor pressure deficit and daylength indicator functions used to calculate
    GSI.

    \param[in] TminMin: Lower limit for minimum temperature (C).
    \param[in] TminMax: Upper limit for minimum temperature (C).
    \param[in] VPDMin: Lower limit for vapor pressure deficit (Pa).
    \param[in] VPDMax: Upper limit for vapor pressure deficit (Pa).
    \param[in] DaylMin: Lower limit for daylength (minutes)
    \param[in] DaylMax: Upper limit for daylength (minutes)
    \return NONE
 */
void LiveFuelMoisture::SetLimits(double TminMin = -2.0,double TminMax = 5.0,double VPDMin = 900,double VPDMax = 4100,double DaylMin = 36000,double DaylMax = 39600, double PcpMin = 0.5, double PcpMax = 1.5)
{
    m_TminMin = TminMin;
    m_TminMax = TminMax;
    m_VPDMin = VPDMin;
    m_VPDMax = VPDMax;
    m_DaylenMin = DaylMin;
    m_DaylenMax = DaylMax;
	m_RTPrcpMin = PcpMin;
	m_RTPrcpMax = PcpMax;
}

void LiveFuelMoisture::SetMAPeriod(unsigned int MAPeriod=21)
{
    m_LFIdaysAvg = m_LFIdaysAvg = max((unsigned int) 1, MAPeriod);;
}

void LiveFuelMoisture::SetUseVPDAvg(bool set)
{
	m_UseVPDAvg = set;
}

bool LiveFuelMoisture::GetUseVPDAvg()
{
	return m_UseVPDAvg;
}

void LiveFuelMoisture::SetUseRTPrecip(bool set)
{
    m_useRTPrecip = set;
}
bool LiveFuelMoisture::GetUseRTPrecip()
{
    return m_useRTPrecip;
}

bool LiveFuelMoisture::GetIsAnnual()
{
	return m_IsAnnual;
}

void LiveFuelMoisture::SetLFMParameters(double MaxGSI = 1.0,double GreenupThreshold =0.5,double MinLFMVal=30, double MaxLFMVal=250)
{
    m_MaxGSI = MaxGSI;
    m_GreenupThreshold = GreenupThreshold;
    if(m_GreenupThreshold == 1.0)
        m_GreenupThreshold = 0.9999;
    if(m_IsHerb)
    {
        m_MaxLFMVal = MaxLFMVal;
        m_MinLFMVal = MinLFMVal;
        m_Slope = (m_MaxLFMVal - m_MinLFMVal) / (1.0 - m_GreenupThreshold);
        m_Intercept = m_MaxLFMVal - m_Slope;

    }
    else
    {
        m_MaxLFMVal = MaxLFMVal;
        m_MinLFMVal = MinLFMVal;
        m_Slope = (m_MaxLFMVal - m_MinLFMVal) / (1.0 - m_GreenupThreshold);
        m_Intercept = m_MaxLFMVal - m_Slope;

    }
}
void LiveFuelMoisture::GetLFMParameters(double * MaxGSI,double * GreenupThreshold ,double * MinLFMVal, double * MaxLFMVal)
{
    *MaxGSI = m_MaxGSI;
    *GreenupThreshold = m_GreenupThreshold;
    *MinLFMVal = m_MinLFMVal;
    *MaxLFMVal = m_MaxLFMVal;

}

void LiveFuelMoisture::SetNumPrecipDays(int numDays)
{
	m_nDaysPrecip = numDays;
}

int LiveFuelMoisture::GetNumPrecipDays()
{
	return m_nDaysPrecip;
}

double LiveFuelMoisture::GetMaxGSI()
{
	return m_MaxGSI;
}

double LiveFuelMoisture::GetGreenupThreshold()
{
	return m_GreenupThreshold;
}

double LiveFuelMoisture::GetMinLFMVal()
{
	return m_MinLFMVal;
}

double LiveFuelMoisture::GetMaxLFMVal()
{
	return m_MaxLFMVal;
}



LiveFuelMoisture::LiveFuelMoisture()
{
    SetLimits();
    SetMAPeriod();
    // Parameters to control green-up and curing of annual herbs
    m_IsAnnual = true; // Overriden when the object is initialized
    ResetHerbState();
	m_UseVPDAvg = false;
	lastUpdateTime = 0;
	m_nDaysPrecip = 30;
    m_useRTPrecip = false;
}

LiveFuelMoisture::LiveFuelMoisture(double Lat,bool IsHerb, bool IsAnnual)
{
    SetLimits();
    SetMAPeriod();
	m_UseVPDAvg = false;
    m_Lat = Lat;
    if(IsHerb)
    {
        SetLFMParameters(1.0,0.5,30,250);
    }
    else
    {
       SetLFMParameters(1.0,0.5,60,200);
    }

	lastUpdateTime = 0;
	m_nDaysPrecip = 30;
    m_useRTPrecip = false;
}

double LiveFuelMoisture::CalcRunningAvgGSI()
{
 	double val = 0.0, gsi = 0.0;
	gsi = std::accumulate(qGSI.begin(), qGSI.end(), val);
	if(qGSI.size() > 0)
		gsi /= qGSI.size();
    return gsi;
}
double LiveFuelMoisture::CalcRunningAvgHerbFM(bool SnowDay)
{
    double GSI = CalcRunningAvgGSI();
    double rescale = GSI / m_MaxGSI;
    double ret = m_MinLFMVal;
    rescale = min(1.0, rescale);
    rescale = max(0.0, rescale);
    if(rescale >= m_GreenupThreshold && !SnowDay)
    {
        ret = m_Slope * rescale + m_Intercept;
        if(!hasGreenedUpThisYear)
        {
            hasGreenedUpThisYear = true;
            canIncreaseHerb = true;
        }
    }
    if(!canIncreaseHerb && lastHerbFM >= 0)
        ret = min(ret, lastHerbFM);
    if(!hasExceeded120ThisYear && ret >= 120)
        hasExceeded120ThisYear = true;
    if(hasExceeded120ThisYear && ret < 120.0 && m_IsAnnual)
    {
        canIncreaseHerb = false;
    }
    lastHerbFM = ret;
    return ret;
}

void LiveFuelMoisture::ResetHerbState()
{
    hasGreenedUpThisYear = false;
    canIncreaseHerb = false;
    hasExceeded120ThisYear = false;
    lastHerbFM = -1.0;
}

double LiveFuelMoisture::CalcRunningAvgWoodyFM(bool SnowDay)
{
    double GSI = CalcRunningAvgGSI();
    double rescale = GSI / m_MaxGSI;
    rescale = min(1.0, rescale);
    rescale = max(0.0, rescale);
    //cout << GSI << " " << rescale << " " << m_GreenupThreshold << endl;
    if(rescale >= m_GreenupThreshold && !SnowDay)
    {
        return m_Slope * rescale + m_Intercept;
    }

    return m_MinLFMVal;
}


double LiveFuelMoisture::CalcGSI(double minRH, double maxTempF, double minTempF, double RTPrcp, double lat, int doy)
{

    double GSI = 0.0, tMinInd, vpdInd, daylenInd, prcpInd;
    tMinInd = GetTminInd(minTempF);
    vpdInd = GetVPDInd(CalcVPD(max(minRH, 5.0), maxTempF));
    daylenInd = GetDaylInd(CalcDayl(lat, doy));
    prcpInd = GetPrcpInd(RTPrcp);
    if(m_useRTPrecip)
        GSI = tMinInd * vpdInd * daylenInd * prcpInd;
    else
        GSI = tMinInd * vpdInd * daylenInd;
    // cout << tMinInd << " " << CalcVPD(max(minRH, 5.0), maxTempF) << " " <<  vpdInd << " " << daylenInd << endl;
    return GSI;
}


double LiveFuelMoisture::CalcGSI_VPDAvg(double RH, double TempF, double maxTempF, double minTempF, double RTPrcp, double lat, int doy)
{

    double GSI = 0.0,tMinInd, tDew, vpdInd, daylenInd, vpd, prcpInd;
    tMinInd = GetTminInd(minTempF);
    tDew = CalcDPT(TempF, RH);
    vpd = CalcVPDavg(tDew, (maxTempF + minTempF) / 2);
    vpdInd = GetVPDInd(vpd);
    daylenInd = GetDaylInd(CalcDayl(lat, doy));
    prcpInd = GetPrcpInd(RTPrcp);
    if (m_useRTPrecip)
        GSI = tMinInd * vpdInd * daylenInd * prcpInd;// *100.0;
    else
        GSI = tMinInd * vpdInd * daylenInd;
    return GSI;
}


double LiveFuelMoisture::GetTminInd(double Tmin)
{

    double tmin =  (Tmin - 32.0) * 5.0 / 9.0; // Convert Tmin from Fahrenheit to celcuius
    if(m_TminMax == m_TminMin)
        return 0;
    if( tmin < m_TminMin)
    {
        return 0;
    }
    else if(tmin > m_TminMax)
    {
        return 1;
    }
    else
    {
        return (tmin - m_TminMin) / (m_TminMax - m_TminMin);
    }
}

double LiveFuelMoisture::GetPrcpInd(double RTPrcp)
{

    if (RTPrcp < m_RTPrcpMin)
    {
        return 0;
    }
    else if (RTPrcp > m_RTPrcpMax)
    {
        return 1;
    }
    else
    {
        return (RTPrcp - m_RTPrcpMin) / (m_RTPrcpMax - m_RTPrcpMin);
    }
}
double LiveFuelMoisture::GetVPDInd(double VPD)
{
    if(m_VPDMax == m_VPDMin)
        return 0;
    if( VPD < m_VPDMin)
    {
        return 1;
    }
    else if(VPD > m_VPDMax)
    {
        return 0;
    }
    else
    {
        return 1 - (VPD - m_VPDMin) / (m_VPDMax - m_VPDMin);

    }
}

double LiveFuelMoisture::GetDaylInd(double Dayl)
{
    if(m_DaylenMin == m_DaylenMax)
        return 0;
    if( Dayl < m_DaylenMin)
    {
        return 0;
    }
    else if(Dayl > m_DaylenMax)
    {
        return 1;
    }
    else
    {
        return (Dayl - m_DaylenMin) / (m_DaylenMax - m_DaylenMin);

    }
}

double LiveFuelMoisture::CalcDPT(double tempF, double RH)
{
    //double vp = CalcVP(tempF);
    //return (273/(1-log(vp/611)/19.59)) - 273;
    double safeTemp = min(140.0, tempF);
    int safeRH = (int)max(5.0, RH);
    double dp = -398.36 - 7428.6 / (-15.674 + log(safeRH / 100.0 * exp(-7482.6/(safeTemp + 398.36) + 15.675)));
    return dp;
}

double LiveFuelMoisture::CalcVP(double tempF)
{
    double tmpC =  (tempF - 32.0) / 1.8;
    double vp = 610.7 * exp((17.38 * tmpC)/(239 + tmpC));
    return vp;
}
double LiveFuelMoisture::CalcVPD(double RH, double TempF)
{
    double vp = CalcVP(TempF);
    double vpd = vp - (RH / 100) * vp;
    if(vpd < 0.0)
        vpd = 0.0;
    return vpd;
}

double LiveFuelMoisture::CalcVPDavg(double TempDewF, double TempAvgF)
{
    double vpDew = CalcVP(TempDewF);
    double vpAvg = CalcVP(TempAvgF);
    double vpd = vpAvg - vpDew;
    if(vpd < 0.0)
        vpd = 0.0;
    return vpd;
}

double  LiveFuelMoisture::CalcDayl(double lat,int yday)
{
    /* Daylength function from MT-CLIM */
    double coslat,sinlat; //,dt,dh,h;
    double decl,cosdecl,sindecl,cosegeom,sinegeom,coshss,hss;

    /* check for (+/-) 90 degrees latitude, throws off daylength calc */
    lat *= RADPERDEG;
    if (lat > 1.5707) lat = 1.5707;
    if (lat < -1.5707) lat = -1.5707;
    coslat = cos(lat);
    sinlat = sin(lat);

    /* calculate cos and sin of declination */
    decl = MINDECL * cos((yday + DAYSOFF) * RADPERDAY);
    cosdecl = cos(decl);
    sindecl = sin(decl);

    /* calculate daylength as a function of lat and decl */
    cosegeom = coslat * cosdecl;
    sinegeom = sinlat * sindecl;
    coshss = -(sinegeom) / cosegeom;
    if (coshss < -1.0) coshss = -1.0;  /* 24-hr daylight */
    if (coshss > 1.0) coshss = 1.0;    /* 0-hr daylight */
    hss = acos(coshss);                /* hour angle at sunset (radians) */
    /* daylength (seconds) */
    return 2.0 * hss * SECPERRAD;
}

LFMCalcState LiveFuelMoisture::GetState()
{
	LFMCalcState ret;
	ret.lastHerbFM = lastHerbFM;
	ret.m_canIncreaseHerb = canIncreaseHerb;
	ret.m_DaylenMax = m_DaylenMax;
	ret.m_DaylenMin = m_DaylenMin;
	ret.m_GreenupThreshold = m_GreenupThreshold;
	ret.m_hasExceeded120ThisYear = hasExceeded120ThisYear;
	ret.m_hasGreenedUpThisYear = hasGreenedUpThisYear;
	ret.m_Intercept = m_Intercept;
	ret.m_IsAnnual = m_IsAnnual;
	ret.m_IsHerb = m_IsHerb;
	ret.m_Lat = m_Lat;
	ret.m_LFIdaysAvg = m_LFIdaysAvg;
	ret.m_MaxGSI = m_MaxGSI;
	ret.m_MaxLFMVal = m_MaxLFMVal;
	ret.m_MinLFMVal = m_MinLFMVal;
	deque<double> copyQ = qGSI;
	while (copyQ.size() > 0)
	{
		double qVal = copyQ.front();
		ret.m_qGSI.push_back((float)qVal);
		copyQ.pop_front();
	}
	ret.m_Slope = m_Slope;
	ret.m_TminMax = m_TminMax;
	ret.m_TminMin = m_TminMin;
	ret.m_UseVPDAvg = m_UseVPDAvg;
	ret.m_VPDMax = m_VPDMax;
	ret.m_VPDMin = m_VPDMin;
	ret.m_lastUpdateTime = lastUpdateTime;
	ret.m_nDaysPrecip = m_nDaysPrecip;
    ret.m_useRTPrecip = m_useRTPrecip;
    ret.m_pcpMin = m_RTPrcpMin;
    ret.m_pcpMax = m_RTPrcpMax;
    return ret;
}

bool LiveFuelMoisture::SetState(LFMCalcState state)
{
	lastHerbFM = state.lastHerbFM;
	canIncreaseHerb = state.m_canIncreaseHerb;
	m_DaylenMax = state.m_DaylenMax;
	m_DaylenMin = state.m_DaylenMin;
	m_GreenupThreshold = state.m_GreenupThreshold;
	hasExceeded120ThisYear = state.m_hasExceeded120ThisYear;
	hasGreenedUpThisYear = state.m_hasGreenedUpThisYear;
	m_Intercept = state.m_Intercept;
	m_IsAnnual = state.m_IsAnnual;
	m_IsHerb = state.m_IsHerb;
	m_Lat = state.m_Lat;
	m_LFIdaysAvg = state.m_LFIdaysAvg;
	m_MaxGSI = state.m_MaxGSI;
	m_MaxLFMVal = state.m_MaxLFMVal;
	m_MinLFMVal = state.m_MinLFMVal;
	vector<FP_STORAGE_TYPE> copyV = state.m_qGSI;
	for(int i = 0; i < copyV.size(); i++)
	{
		double qVal = copyV[i];
		qGSI.push_back((float)qVal);
	}
	m_Slope = state.m_Slope;
	m_TminMax = state.m_TminMax;
	m_TminMin = state.m_TminMin;
	m_UseVPDAvg = state.m_UseVPDAvg;
	m_VPDMax = state.m_VPDMax;
	m_VPDMin = state.m_VPDMin;
	lastUpdateTime = state.m_lastUpdateTime;
	m_nDaysPrecip = state.m_nDaysPrecip;
    m_useRTPrecip = state.m_useRTPrecip;
    m_RTPrcpMin = state.m_pcpMin;
    m_RTPrcpMax = state.m_pcpMax;
	return true;
}
