//------------------------------------------------------------------------------
/*! \file DeadFuelMoisture.h
    \brief DeadFuelMoisture class interface and declarations.
    \author Copyright (C) 2005 by Collin D. Bevins.
    \version 1.0.0 - Uses C++ and STL only (i.e., no threads, Qt libraries,
        Loki Singletons, or SemMessages).

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

#ifndef _DEADFUELMOISTURE_H_INCLUDED_
#define _DEADFUELMOISTURE_H_INCLUDED_

#include "dfmcalcstate.h"
// Standard include files
#include <cmath>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <string.h>

// Custom include files
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Custom include files


//------------------------------------------------------------------------------
/*! \page DeadFuelMoisture Dead Fuel Moisture

    DeadFuelMoisture is an implementation of Nelson's dead fuel moisture model
    \ref nelson2000 as modified by \ref bevins2005.

    \section dfmmods Dead Fuel Moisture Model Modifications

    The DeadFuelMoisture class has been modified from Nelson \ref nelson2000
    description as follows:

    -- Moisture content and diffusivity computation time steps are independent
        of elapsed time since the previous update(), as recommended by Carlson
        2003 \ref carlson2003.
    -- Planar heat transfer rate, adsorption rate, rainfall runoff rate, and
        the number of moisture content and diffusivity computation steps are
        automatically derived from the stick radius using equations
        developed by Bevins \ref bevins2005.
    -- Precipitation is no longer divided into rainfall and rainstorm
        categories, making Nelson's storm transition value (stv) unnecessary.
    -- Rainfall events are no longer divided into first hour and subsequent
        hour categories, making Nelson's subsequent runoff factor (rai1)
        unnecessary.
    -- The following parameters are now assigned values independent of
        stick radius:
        -- maximum local moisture content is 0.60 g/g,
        -- desorption rate is 0.06 (cm3/cm2)/h.
        -- water film contribution is zero g/g.

    The above changes produce better stability and more accurate predictions
    for the available test data sets.

    \section dfmuse Using DeadFuelMoisture

    This is a quick tutorial on using the DeadFuelMoisture class.

    \subsection dfmuse1 Step 1: Create A Dead Fuel Moisture Stick

    To create a standard 1-h, 10-h, 100-h, or 1000-h time lag fuel moisture
    stick, call one of:

    -- DeadFuelMoisture* dfm1h = createDeadFuelMoisture1( "1-h stick name" );
    -- DeadFuelMoisture* dfm10h = createDeadFuelMoisture10( "10-h stick name" );
    -- DeadFuelMoisture* dfm100h = createDeadFuelMoisture100( "100-h stick name" );
    -- DeadFuelMoisture* dfm1000h = createDeadFuelMoisture1000( "1000-h stick name" );

    To create a stick with a specific radius:

    -- DeadFuelMoisture *dfm = new DeadFuelMoisture( radius, "stick name" );

    \subsection dfmuse2 Step 2: Customize the Dead Fuel Moisture Stick (Optional)

    This step is only necessary if you are experimenting with the internals
    of the DeadFuelMoisture class.  If not, go on to Step 3.

    Call any of the following to set internal stick parameters:

    -- void setAdsorptionRate( double adsorptionRate ) ;
    -- void setDesorptionRate( double desorptionRate=0.06 ) ;
    -- void setDiffusivitySteps( int diffusivitySteps );
    -- void setPlanarHeatTransferRate( double planarHeatTransferRate ) ;
    -- void setMaximumLocalMoisture( double localMaxMc=0.6 ) ;
    -- void setMoistureSteps( int moistureSteps );
    -- void setRainfallRunoffFactor( double rainfallRunoffFactor );
    -- void setRandomSeed( int randseed=0 ) ;
    -- void setStickDensity( double stickDensity=0.4 );
    -- void setStickLength( double stickLength=41.0 );
    -- void setStickNodes( int stickNodes=11 ) ;
    -- void setWaterFilmContribution( double waterFilm=0.0 ) ;

    Finally, you \a must call:

    -- void initializeStick( void ) ;

    \subsection dfmuse3 Step 3: Initialize the Stick and Its Environment (Optional)

    If you wish to initialize the stick's internal temperature and moisture
    profile, call one of:

    -- void initializeEnvironment( int year, int month, int day, int hour,
        int minute, int second, double at, double rh, double sr, double rcum,
        double ti, double hi, double wi, double bp );
    -- void initializeEnvironment( double at, double rh, double sr, double rcum,
        double ti, double hi, double wi, double bp );

    The first method initializes the stick date and time, and should be
    used if you are using the version of update() that takes the date and
    time as arguments.

    The second method should be called if you are using the version of
    update() that takes the elapsed time as an argument.

    If neither of these methods are invoked, the stick internal and external
    environments are initialized by the first update() invocation.

    \subsection dfmuse4 Step 4: Update the Stick Environment

    Call the update() method to update the stick's clock and external
    environment, and recalculate the stick's internal temperature and moisture
    profile.  There are two overloaded versions of update():

    -- bool update( int year, int month, int day, int hour, int minute,
        int second, double at, double rh, double sW, double rcum );
    -- bool update( double et, double at, double rh, double sW, double rcum );

    The first version determines elapsed time from the date and time arguments.
    Do not mix calls to the two versions for the same DeadFuelMoisture instance.

    \subsection dfmuse5 Step 5: Get Stick Temperature and Moisture Content

    To determine the current stick temperature and moisture content, call:

    -- double meanMoisture();
    -- double meanWtdMoisture();
    -- double meanWtdTemperature();
    -- double surfaceMoisture();
    -- double surfaceTemperature();

    Temperatures are oC and moisture contents are g/g.
 */



//------------------------------------------------------------------------------
/*! \class DeadFuelMoisture DeadFuelMoisture.h
    \brief Determines moisture content of dead and down woody fuels
    from ambient air temperature, relative humidity, insolation, and rainfall.

    DeadFuelMoisture is an implementation of Nelson's dead fuel moisture model
    \ref nelson2000. This section describes DeadFuelMoisture class usage only;
    for a more complete discussion of the Nelson model,
    see \ref DeadFuelMoisture.
 */
typedef enum 
{
	DFM_State_None = 0,
	DFM_State_Adsorption = 1,
	DFM_State_Desorption = 2,
	DFM_State_Condensation1 = 3,
	DFM_State_Condensation2 = 4,
	DFM_State_Evaporation = 5,
	DFM_State_Rainfall1 = 6,
	DFM_State_Rainfall2 = 7,
	DFM_State_Rainstorm = 8,
	DFM_State_Stagnation = 9,
	DFM_State_Error = 10
} DFM_State;


class DeadFuelMoisture
{

/*! \var Aks
    \brief Permeability of a water saturated stick (2.0e-13 cm2).
 */
static const double Aks;    // = 2.0e-13;

/*! \var Alb
    \brief Shortwave albido (0.6 dl).
 */
static const double Alb;    // = 0.6;

/*! \var Alpha
    \brief Fraction of cell length that overlaps adjacent cells (0.25 cm/cm).
 */
static const double Alpha;  // = 0.25;

/*! \var Ap
    \brief Psychrometric constant (0.000772 / oC).
 */
static const double Ap;     // = 0.000772;

/*! \var Aw
    \brief Ratio of cell cavity to total cell width (0.8 cm/cm).
 */
static const double Aw;     // = 0.8;

/*! \var Eps
    \brief Longwave emissivity of stick surface (0.85 dl).
 */
static const double Eps;    // = 0.85;

/*! \var Hfs
    \brief Saturation value of the stick surface humidity (0.99 g/g).
 */
static const double Hfs;    // = 0.99;

/*! \var Kelvin
    \brief Celcius-to-Kelvin offset (273.2 oC).
 */
static const double Kelvin; // = 273.2;

/*! \var Pi
    \brief A well-rounded number (dl).
 */
static const double Pi;     // = 3.141592654;

/*! \var Pr
    \brief Prandtl number (0.7 dl).
 */
static const double Pr;     // = 0.7;

/*! \var Sbc
    \brief Stefan-Boltzmann constant (1.37e-12 cal/cm2-s-K4).
 */
static const double Sbc;    // = 1.37e-12;

/*! \var Sc
    \brief Schmidt number (0.58 dl).
 */
static const double Sc;     // = 0.58;

/*! \var Smv
    \brief Factor to convert solar radiation from W/m2 to milliVolts
    \f$mv = \frac {W / m^{2}} {94.743}\f$
 */
static const double Smv;    // = 94.743;

/*! \var St
    \brief Surface tension (72.8).
 */
static const double St;     // = 72.8;

/*! \var Tcd
    \brief Day time clear sky temperature (6 oC).
 */
static const double Tcd;    // = 6.;

/*! \var Tcn
    \brief Night time clear sky temperature (3 oC).
 */
static const double Tcn;    // = 3.;

/*! \var Thdiff
    \brief Thermal diffusivity ( 8.0 cms/h).
 */
static const double Thdiff; // = 8.0;

/*! \var Wl
    \brief Diameter of interior cell cavity ( 0.0023 cm).
 */
static const double Wl;     // = 0.0023;

//------------------------------------------------------------------------------
// Stick-independent intermediates derived in Fms_CreateConstant().
//------------------------------------------------------------------------------

/*! \var Srf
    \brief Factor to derive "sr", solar radiation received (14.82052 cal/cm2-h).
 */
static const double Srf;    // = 14.82052;

/*! \var Wsf
    \brief Manifest constant equal to -log(1.0 - 0.99)
 */
static const double Wsf;    // = 4.60517;

/*! \var Hrd
    \brief Factor to derive daytime long wave radiative surface heat transfer.
    \arg Let Sb = 4. * 3600. * Sbc * Eps = 1.67688e-08;
    \arg Let tsk = Tcd + Kelvin;
    \arg Then Hrd = Sb * tsk * tsk * tsk / Pi;
    \arg And Hrd = 0.116171;
 */
static const double Hrd;    // = 0.116171;

/*! \var Hrn
    \brief Factor to derive nighttime long wave radiative surface heat transfer.
    \arg Let Sb = 4. * 3600. * Sbc * Eps = 1.67688e-08;
    \arg Let tsk = Tcn + Kelvin;
    \arg Then Hrn = Sb * tsk * tsk * tsk / Pi;
    \arg And Hrn = 0.112467;
 */
static const double Hrn;    // = 0.112467;

/*! \var Sir
    \brief Saturation value below which liquid water columns no longer exist.
    \arg Sir = Aw * Alpha / (4. * (1.-(2.-Aw) * Alpha));
 */
static const double Sir;    // = 0.0714285;

/*! \var Scr
    \brief Saturation value at which liquid miniscus first enters the tapered
    portion of wood cells.
    \arg Scr = 4. * Sir = 0.285714;
 */
static const double Scr;    // = 0.285714;

//------------------------------------------------------------------------------
/*! \enum DFM_State
    \brief DeadFuelMoisture environmental condition states.
 */

//------------------------------------------------------------------------------
/*! \var DFM_States
    \brief Number of DFM_State enumerations.
 */
static const int DFM_States = 11;

// Friends
public:
    friend std::ostream &operator<<(std::ostream& output, const DeadFuelMoisture& r );
    friend std::istream &operator>>(std::istream& input, DeadFuelMoisture& r );

// Public methods
public:
    // Constructors
    DeadFuelMoisture( double radius=0.64, const std::string& name="" ) ;
    // Virtual destructor permits subclassing
    virtual ~DeadFuelMoisture( void ) ;
    // Copy constructor
    DeadFuelMoisture( const DeadFuelMoisture& rhs ) ;
    // Assignment operator
    const DeadFuelMoisture& operator=( const DeadFuelMoisture& rhs ) ;
    const char* className( void ) const ;
    // Static convenience functions
    static DeadFuelMoisture* createDeadFuelMoisture1( const std::string& name="" ) ;
    static DeadFuelMoisture* createDeadFuelMoisture10( const std::string& name="" ) ;
    static DeadFuelMoisture* createDeadFuelMoisture100( const std::string& name="" ) ;
    static DeadFuelMoisture* createDeadFuelMoisture1000( const std::string& name="" ) ;
    static double  deriveAdsorptionRate( double radius ) ;
    static int     deriveDiffusivitySteps( double radius ) ;
    static int     deriveMoistureSteps( double radius ) ;
    static double  derivePlanarHeatTransferRate( double radius ) ;
    static double  deriveRainfallRunoffFactor( double radius ) ;
    static int     deriveStickNodes( double radius ) ;
    static double  uniformRandom( double min, double max ) ;

    // Methods for initializing the dead fuel stick and its environment
    void initializeEnvironment(
        int     year,
        int     month,
        int     day,
        int     hour,
        int     minute,
        int     second,
        double  at,
        double  rh,
        double  sr,
        double  rcum,
        double  ti,
        double  hi,
        double  wi,
        double  bp=0.0218
    );
    void initializeEnvironment(
        double  at,
        double  rh,
        double  sr,
        double  rcum,
        double  ti,
        double  hi,
        double  wi,
        double  bp=0.0218
    );

    // Methods for updating the fuel moisture condition
    bool update(
        int     year,
        int     month,
        int     day,
        int     hour,
        int     minute,
        int     second,
        double  at,
        double  rh,
        double  sW,
        double  rcum,
        double  bpr=0.0218,
        bool prcpAsAmnt = false
    ) ;
    bool update(
        double  et,
        double  at,
        double  rh,
        double  sW,
        double  rcum,
        double  bpr=0.0218,
        bool prcpAsAmnt = false
    ) ;
    void zero( void ) ;

    // Methods to access update() results
    double elapsedTime( void ) const ;
    bool   initialized( void ) const ;
    std::string name( void ) const ;
    double meanMoisture( void ) const ;
    double meanWtdMoisture( void ) const ;
	double medianRadialMoisture(void) const;
    double meanWtdTemperature( void ) const;

    double pptRate( void ) const ;
    int state( void ) const;
    const char* stateName( void ) const;
    double surfaceMoisture( void ) const ;
    double surfaceTemperature( void ) const;
    long   updates( void ) const;

    // Methods to access model parameters
    double adsorptionRate( void ) const ;
    double desorptionRate( void ) const ;
    int    diffusivitySteps( void ) const ;
    double maximumLocalMoisture( void ) const ;
    int    moistureSteps( void ) const ;
    double planarHeatTransferRate( void ) const ;
    double rainfallRunoffFactor( void ) const ;
    double stickDensity( void ) const ;
    double stickLength( void ) const ;
    int    stickNodes( void ) const ;
    double waterFilmContribution( void ) const ;

    // For those who want to experiment with the model parameters...
    void initializeStick( void ) ;
    void setAdsorptionRate( double adsorptionRate ) ;
    void setAllowRainstorm( bool allow=true ) ;
    void setAllowRainfall2( bool allow=true ) ;
    void setDesorptionRate( double desorptionRate=0.06 ) ;
    void setDiffusivitySteps( int diffusivitySteps );
    void setMaximumLocalMoisture( double localMaxMc=0.6 ) ;
    void setMoistureSteps( int moistureSteps );
    void setPertubateColumn( bool pertubate=true ) ;
    void setPlanarHeatTransferRate( double planarHeatTransferRate ) ;
    void setRainfallRunoffFactor( double rainfallRunoffFactor );
    void setRandomSeed( int randseed=0 ) ;
    void setRampRai0( bool ramp=true ) ;
    void setStickDensity( double stickDensity=0.4 );
    void setStickLength( double stickLength=41.0 );
    void setStickNodes( int stickNodes=11 ) ;
    void setWaterFilmContribution( double waterFilm=0.0 ) ;
	void setMoisture(float initFM);
	double eqmc(double fTemp, double fRH);
	void initializeParameters( double radius, const std::string& name ) ;
	DFMCalcState GetState();
	bool SetState(DFMCalcState state);
// Protected methods
protected:
    void diffusivity( double bp ) ;

    void initializeParameters(
            const std::string& name,
            double  radius,
            int     stickNodes,
            int     moistureSteps,
            int     diffusivitySteps,
            double  planarHeatTransferRate,
            double  adsorptionRate,
            double  rainfallRunoffFactor,
            double  stickLength=41.,
            double  stickDensity=0.4,
            double  waterFilmContribution=0.,
            double  localMaxMc=0.6,
            double  desorptionRate=0.06,
            double  rainfallAdjustmentFactor=0.5,
            double  stormTransitionValue=99999.,
            int     randseed=0,
            bool    allowRainfall2=false,
            bool    allowRainstorm=false,
            bool    pertubateColumn=false,
            bool    rampRai0=false
         ) ;

// Protected data members
protected:
    //SemTime m_semTime;  //!< Current observation's date and time.
    double m_Jday;
    double m_Year;
    double m_Month;
    double m_Day;
    double m_Hour;
    double m_Min;
    double m_Sec;
    time_t obstime;

    // Stick and model parameters defined or inferred during construction:
    double  m_density;  //!< Stick density (g/cm3).
    int     m_dSteps;   //!< Number of diffusivity computation steps per observation.
    double  m_hc;       //!< Stick planar heat transfer rate (cal/cm2-h-C).
    double  m_length;   //!< Stick length (cm).
    std::string  m_name;     //!< Stick name or other descriptive text.
    int     m_nodes;    //!< Number of stick nodes in the radial direction.
    double  m_radius;   //!< Stick radius (cm).
    double  m_rai0;     //!< Rain runoff factor during the initial hour of rainfall (dl).
    double  m_rai1;     //!< Rain runoff factor after the initial hour of rainfall (dl) [no longer used].
    double  m_stca;     //!< Adsorption surface mass transfer rate ((cm3/cm2)/h).
    double  m_stcd;     //!< Desorption surface mass transfer rate ((cm3/cm2)/h).
    int     m_mSteps;   //!< Number of moisture content computation steps per observation.
    double  m_stv;      //!< Storm transition value (cm/h) [no longer used].
    double  m_wfilmk;   //!< Water film contribution to stick moisture content (g water/g dry fuel).
    double  m_wmx;      //!< Stick maximum local moisture due to rain (g water/g dry fuel).

    // Configuration parameters
    bool    m_allowRainfall2;   // If TRUE, applies Nelson's logic for rainfall runoff after the first hour
    bool    m_allowRainstorm;   // If TRUE, applies Nelson's logic for rainstorm transition and state
    bool    m_pertubateColumn;  // If TRUE, the continuous liquid column condition get pertubated
    bool    m_rampRai0;         // If TRUE, used Bevins' ramping of rainfall runoff factor rather than Nelsons rai0 *= 0.15

    // Intermediate stick variables derived in initializeStick()
    double  m_dx;       //!< Internodal radial distance (cm).
    double  m_wmax;     //!< Maximum possible stick moisture content (g water/g dry fuel).
    std::vector<double> m_x; //!< Array of nodal radial distances from stick center (cm).
    std::vector<double> m_v; //!< Array of nodal volume weighting fractions (cm3 node/cm3 stick).

    // Optimization factors derived in initializeStick()
    double  m_amlf;     //!< \a aml optimization factor.
    double  m_capf;     //!< \a cap optimization factor.
    double  m_hwf;      //!< \a hw and \a aml computation factor.
    double  m_dx_2;     //!< 2 times the internodal distance \a m_dx (cm).
    double  m_vf;       //!< optimization factor used in update().

    // Environmental variables provided to initializeEnvironment():
    double  m_bp0;      //!< Previous observation's barometric presure (cal/cm3).
    double  m_ha0;      //!< Previous observation's air humidity (dl).
    double  m_rc0;      //!< Previous observation's cumulative rainfall amount (cm).
    double  m_sv0;      //!< Previous observation's solar radiation (mV).
    double  m_ta0;      //!< Previous observation's air temperature (oC).
    bool    m_init;     //!< Flag set by initialize().

    // Environmental variables provided to update():
    double  m_bp1;      //!< Current observation's barometric pressure (cal/cm3).
    double  m_et;       //!< Elapsed time since previous observation (h).
    double  m_ha1;      //!< Current observation's air humidity (dl).
    double  m_rc1;      //!< Current observation's cumulative rainfall amount (cm).
    double  m_sv1;      //!< Current observation's solar radiation (mV).
    double  m_ta1;      //!< Current observation's air temperature (oC).

    // Intermediate environmental variables derived by update():
    double  m_ddt;      //!< Stick diffusivity computation interval (h).
    double  m_mdt;      //!< Stick moisture content computation interval (h).
    double  m_mdt_2;    //!< 2 times the moisture time step \a m_mdt (h).
    double  m_pptrate;  //!< Current observation period's rainfall rate (cm/h).
    double  m_ra0;      //!< Previous observation period's rainfall amount (cm).
    double  m_ra1;      //!< Current observation period's rainfall amount (cm).
    double  m_rdur;     //!< Current rainfall event duration (h)
    double  m_sf;       //!< optimization factor used in update().

    // Stick moisture condition variables derived in update():
    double  m_hf;       //!< Stick surface humidity (g water/g dry fuel).
    double  m_wsa;      //!< Stick fiber saturation point (g water/g dry fuel).
    double  m_sem;      //!< Stick equilibrium moisture content (g water/g dry fuel).
    double  m_wfilm;    //!< Amount of water film (0 or \a m_wfilmk) (g water/g dry fuel).
    double  m_elapsed;  //!< Total simulation elapsed time (h).
    std::vector<double> m_t; //!< Array of nodal temperatures (oC).
    std::vector<double> m_s; //!< Array of nodal fiber saturation points (g water/g dry fuel).
    std::vector<double> m_d; //!< Array of nodal bound water diffusivities (cm2/h).
    std::vector<double> m_w; //!< Array of nodal moisture contents (g water/g dry fuel).
    long    m_updates;  //!< Number of calls made to update().
    int m_state;  //!< Prevailing dead fuel moisture state.
    int     m_randseed; //!< If not zero, nodal temperature, saturation, and moisture contents are pertubated by some small amount. If < 0, uses system clock for seed.
    std::vector<double> m_Ttold; //!< Temporary array of nodal temperatures (oC).
    std::vector<double> m_Tsold; //!< Temporary array of nodal fiber saturation points (g water/g dry fuel).
    std::vector<double> m_Twold; //!< Temporary array of nodal moisture contents (g water/g dry fuel).
    std::vector<double> m_Tv;    //!< Temporary array used to redistribute nodal temperatures
    std::vector<double> m_To;    //!< Temporary array used to redistribute moisture contents
    std::vector<double> m_Tg;    //!< Temporary array of nodal free water transport coefficients
};



//-----------------------------------------------------------------------------
/*! \page citations Cited Documents

    \section bevins2005 Bevins, Collin D. 2005.
    Adapting Nelson's dead fuel moisture model for wildland fire modeling.

    \section carlson2003 Carlson, J. D. 2003.
    Evaluation of a new dead fuel moisture model in a near-real-time data
    assimilation and forecast environment.  Progress report on file at USDA
    Forest Service Intermountain Research Station, Fire Sciences Laboratory,
    Missoula, MT.  September 15, 2003.

    \section carlson2004a Carlson, J. D. 2004a.
    Evaluation of a new dead fuel moisture model in a near-real-time data
    assimilation and forecast environment.  Progress report on file at USDA
    Forest Service Intermountain Research Station, Fire Sciences Laboratory,
    Missoula, MT.  February 10, 2004.

    \section carlson2004b Carlson, J. D. 2004b.
    Evaluation of a new dead fuel moisture model in a near-real-time data
    assimilation and forecast environment.  Progress report on file at USDA
    Forest Service Intermountain Research Station, Fire Sciences Laboratory,
    Missoula, MT.  July 13, 2004.

    \section meeus1982 Meeus, Jean. 1982.
    Astronomical formulae for calculators.  Fourth Ed. Willman-Bell, Inc. 218p.

    \section nelson2000 Nelson, Ralph M. Jr. 2000.
    Prediction of diurnal change in 1-h fuel stick moisture content.
    <i>Can. J. For. Res.</i> <b>30</b>: 1071-1087. (2000).
 */

#endif

//------------------------------------------------------------------------------
//  End of DeadFuelMoisture.h
//------------------------------------------------------------------------------

