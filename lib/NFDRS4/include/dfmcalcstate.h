#pragma once
#include <vector>
#include <string>

#include "nfdrs4statesizes.h"

class DFMCalcState
{
public:
	DFMCalcState();
	DFMCalcState(const DFMCalcState &rhs);
	~DFMCalcState();

	bool  ReadState(FILE *in);
	bool SaveState(FILE *out);

	//use to construct obstime member
	short m_JDay;
	short m_Year;
	char m_Month;
	char m_Day;
	char m_Hour;
	char m_Min;
	char m_Sec;
	time_t m_obstime;
	//internal members set explicitly
	//float  m_bp0;      //!< Previous observation's barometric presure (cal/cm3).
	//float  m_ha0;      //!< Previous observation's air humidity (dl).
	//float  m_rc0;      //!< Previous observation's cumulative rainfall amount (cm).
	//float  m_sv0;      //!< Previous observation's solar radiation (mV).
	//float  m_ta0;      //!< Previous observation's air temperature (oC).
	
	//use in initialize environment
	//NOTE: set bp0 = bp1 first!
	FP_STORAGE_TYPE  m_bp1;      //!< Current observation's barometric pressure (cal/cm3).
	FP_STORAGE_TYPE  m_et;       //!< Elapsed time since previous observation (h).
	FP_STORAGE_TYPE  m_ha1;      //!< Current observation's air humidity (dl).
	FP_STORAGE_TYPE  m_rc1;      //!< Current observation's cumulative rainfall amount (cm).
	FP_STORAGE_TYPE  m_sv1;      //!< Current observation's solar radiation (mV).
	FP_STORAGE_TYPE  m_ta1;      //!< Current observation's air temperature (oC).
	FP_STORAGE_TYPE  m_hf;		//!< Stick surface humidity (g water/g dry fuel).
	FP_STORAGE_TYPE  m_wsa;      //!< Stick fiber saturation point (g water/g dry fuel).

	//needed to be maintained for update to work
	FP_STORAGE_TYPE m_rdur;
	FP_STORAGE_TYPE m_ra1;

	// Stick moisture condition variables derived in update(): need stored for diffusivity
	short     m_nodes;    //!< Number of stick nodes in the radial direction. just for knowing how big vectors are when stored
	std::vector<FP_STORAGE_TYPE> m_t; //!< Array of nodal temperatures (oC).
	std::vector<FP_STORAGE_TYPE> m_s; //!< Array of nodal fiber saturation points (g water/g dry fuel).
	std::vector<FP_STORAGE_TYPE> m_d; //!< Array of nodal bound water diffusivities (cm2/h).
	std::vector<FP_STORAGE_TYPE> m_w; //!< Array of nodal moisture contents (g water/g dry fuel).

};

