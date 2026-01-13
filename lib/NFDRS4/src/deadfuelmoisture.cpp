//------------------------------------------------------------------------------
/*! \file DeadFuelMoisture.cpp
    \brief DeadFuelMoisture class definition and implementation.
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

// Standard include files
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <vector>

using std::cerr;
using std::endl;
using std::ostringstream;
using std::istream;
using std::ostream;
using std::string;
using std::vector;

// Custom include files
#include "deadfuelmoisture.h"

//#define DEBUG
#undef DEBUG

/*! \def USE_CDB_METHOD
    \brief Disables Nelson's rainstorm sections,
    making the storm transition value unnecessary.
 */
#define USE_CDB_METHOD
//#undef USE_CDB_METHOD

// Non-integral static data members must be initialized outside the class
const double DeadFuelMoisture::Aks = 2.0e-13;
const double DeadFuelMoisture::Alb = 0.6;
const double DeadFuelMoisture::Alpha = 0.25;
const double DeadFuelMoisture::Ap = 0.000772;
const double DeadFuelMoisture::Aw = 0.8;
const double DeadFuelMoisture::Eps = 0.85;
const double DeadFuelMoisture::Hfs = 0.99;
const double DeadFuelMoisture::Kelvin = 273.2;
const double DeadFuelMoisture::Pi = 3.141592654;
const double DeadFuelMoisture::Pr = 0.7;
const double DeadFuelMoisture::Sbc = 1.37e-12;
const double DeadFuelMoisture::Sc = 0.58;
const double DeadFuelMoisture::Smv = 94.743;
const double DeadFuelMoisture::St = 72.8;
const double DeadFuelMoisture::Tcd = 6.;
const double DeadFuelMoisture::Tcn = 3.;
const double DeadFuelMoisture::Thdiff = 8.0;
const double DeadFuelMoisture::Wl = 0.0023;
const double DeadFuelMoisture::Srf = 14.82052;
const double DeadFuelMoisture::Wsf = 4.60517;
const double DeadFuelMoisture::Hrd = 0.116171;
const double DeadFuelMoisture::Hrn = 0.112467;
const double DeadFuelMoisture::Sir = 0.0714285;
const double DeadFuelMoisture::Scr = 0.285714;

//------------------------------------------------------------------------------
/*! \brief Default class constructor.

    Creates a dead fuel moisture stick with the passed \a radius and \a name,
    and derives all other parameters using interpolations from \ref bevins2005.

    \param[in] radius Dead fuel stick radius (cm).
    \param[in] name Name or description of the dead fuel stick.
 */

DeadFuelMoisture::DeadFuelMoisture( double radius, const string& name )
{
    initializeParameters( radius, name ) ;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Copy constructor.

    \param[in] r Reference to the DeadFuelMoisture from which to copy.
 */

DeadFuelMoisture::DeadFuelMoisture( const DeadFuelMoisture& r )
{
    //m_semTime   = r.m_semTime;
    m_density   = r.m_density;
    m_dSteps    = r.m_dSteps;
    m_hc        = r.m_hc;
    m_length    = r.m_length;
    m_name      = r.m_name;
    m_nodes     = r.m_nodes;
    m_radius    = r.m_radius;
    m_rai0      = r.m_rai0;
    m_rai1      = r.m_rai1;
    m_stca      = r.m_stca;
    m_stcd      = r.m_stcd;
    m_mSteps    = r.m_mSteps;
    m_stv       = r.m_stv;
    m_wfilmk    = r.m_wfilmk;
    m_wmx       = r.m_wmx;
    m_allowRainfall2  = r.m_allowRainfall2;
    m_allowRainstorm  = r.m_allowRainstorm;
    m_pertubateColumn = r.m_pertubateColumn;
    m_rampRai0  = r.m_rampRai0;
    m_dx        = r.m_dx;
    m_wmax      = r.m_wmax;
    m_x         = r.m_x;
    m_v         = r.m_v;
    m_amlf      = r.m_amlf;
    m_capf      = r.m_capf;
    m_hwf       = r.m_hwf;
    m_dx_2      = r.m_dx_2;
    m_vf        = r.m_vf;
    m_bp0       = r.m_bp0;
    m_ha0       = r.m_ha0;
    m_rc0       = r.m_rc0;
    m_sv0       = r.m_sv0;
    m_ta0       = r.m_ta0;
    m_init      = r.m_init;
    m_bp1       = r.m_bp1;
    m_et        = r.m_et;
    m_ha1       = r.m_ha1;
    m_rc1       = r.m_rc1;
    m_sv1       = r.m_sv1;
    m_ta1       = r.m_ta1;
    m_ddt       = r.m_ddt;
    m_mdt       = r.m_mdt;
    m_mdt_2     = r.m_mdt_2;
    m_pptrate   = r.m_pptrate;
    m_ra0       = r.m_ra0;
    m_ra1       = r.m_ra1;
    m_rdur      = r.m_rdur;
    m_sf        = r.m_sf;
    m_hf        = r.m_hf;
    m_wsa       = r.m_wsa;
    m_sem       = r.m_sem;
    m_wfilm     = r.m_wfilm;
    m_elapsed   = r.m_elapsed;
    m_t         = r.m_t;
    m_s         = r.m_s;
    m_d         = r.m_d;
    m_w         = r.m_w;
    m_updates   = r.m_updates;
    m_state     = r.m_state;
    m_randseed  = r.m_randseed;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Assignment operator.

    \param[in] r Reference to the DeadFuelMoisture instance from which to copy.
 */

const DeadFuelMoisture& DeadFuelMoisture::operator=( const DeadFuelMoisture& r )
{
    if ( this != &r )
    {
        //m_semTime   = r.m_semTime;
        m_density   = r.m_density;
        m_dSteps    = r.m_dSteps;
        m_hc        = r.m_hc;
        m_length    = r.m_length;
        m_name      = r.m_name;
        m_nodes     = r.m_nodes;
        m_radius    = r.m_radius;
        m_rai0      = r.m_rai0;
        m_rai1      = r.m_rai1;
        m_stca      = r.m_stca;
        m_stcd      = r.m_stcd;
        m_mSteps    = r.m_mSteps;
        m_stv       = r.m_stv;
        m_wfilmk    = r.m_wfilmk;
        m_wmx       = r.m_wmx;
        m_allowRainfall2  = r.m_allowRainfall2;
        m_allowRainstorm  = r.m_allowRainstorm;
        m_pertubateColumn = r.m_pertubateColumn;
        m_rampRai0  = r.m_rampRai0;
        m_dx        = r.m_dx;
        m_wmax      = r.m_wmax;
        m_x         = r.m_x;
        m_v         = r.m_v;
        m_amlf      = r.m_amlf;
        m_capf      = r.m_capf;
        m_hwf       = r.m_hwf;
        m_dx_2      = r.m_dx_2;
        m_vf        = r.m_vf;
        m_bp0       = r.m_bp0;
        m_ha0       = r.m_ha0;
        m_rc0       = r.m_rc0;
        m_sv0       = r.m_sv0;
        m_ta0       = r.m_ta0;
        m_init      = r.m_init;
        m_bp1       = r.m_bp1;
        m_et        = r.m_et;
        m_ha1       = r.m_ha1;
        m_rc1       = r.m_rc1;
        m_sv1       = r.m_sv1;
        m_ta1       = r.m_ta1;
        m_ddt       = r.m_ddt;
        m_mdt       = r.m_mdt;
        m_mdt_2     = r.m_mdt_2;
        m_pptrate   = r.m_pptrate;
        m_ra0       = r.m_ra0;
        m_ra1       = r.m_ra1;
        m_rdur      = r.m_rdur;
        m_sf        = r.m_sf;
        m_hf        = r.m_hf;
        m_wsa       = r.m_wsa;
        m_sem       = r.m_sem;
        m_wfilm     = r.m_wfilm;
        m_elapsed   = r.m_elapsed;
        m_t         = r.m_t;
        m_s         = r.m_s;
        m_d         = r.m_d;
        m_w         = r.m_w;
        m_updates   = r.m_updates;
        m_state     = r.m_state;
        m_randseed  = r.m_randseed;
    }
    return( *this );
}

//------------------------------------------------------------------------------
/*! \brief Virtual class destructor.
 */

DeadFuelMoisture::~DeadFuelMoisture( void )
{
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's adsorption surface mass transfer rate.

    \return Current surface mass transfer rate for adsorption
    (\f$\frac {cm^{3}} {cm^{2} \cdot h}\f$).
 */

double DeadFuelMoisture::adsorptionRate( void ) const
{
    return( m_stca );
}

//------------------------------------------------------------------------------
/*! \brief Access to DeadFuelMoisture class name.

    \return Pointer to static DeadFuelMoisture char string.
 */

const char* DeadFuelMoisture::className( void ) const
{
    return( "DeadFuelMoisture" );
}



 //------------------------------------------------------------------------------
 /*! \brief Convenience method to create a 1-h time lag
     DeadFuelMoisture instance and set all all parameters
     \param[in] NONE
     \relates DeadFuelMoisture
     \return Void.
  */
void DeadFuelMoisture::initDeadFuelMoisture1()
{
    initializeParameters(0.2, "One Hour");
    setAdsorptionRate(0.462252733);
    setMaximumLocalMoisture(0.35);
    setMoisture(0.2f);
    initializeStick();
}


//------------------------------------------------------------------------------
/*! \brief Convenience method to create a 10-h time lag
    DeadFuelMoisture instance and set all all parameters
    \param[in] NONE
    \relates DeadFuelMoisture
    \return Void.
 */
void DeadFuelMoisture::initDeadFuelMoisture10()
{
    initializeParameters(0.64, "Ten Hour");
    setAdsorptionRate(0.079548303);
    setMaximumLocalMoisture(0.35);
    setMoisture(0.2f);
    initializeStick();
}


//------------------------------------------------------------------------------
/*! \brief Convenience method to create a 100-h time lag
    DeadFuelMoisture instance and set all all parameters
    \param[in] NONE
    \relates DeadFuelMoisture
    \return Void.
 */
void DeadFuelMoisture::initDeadFuelMoisture100()
{
    initializeParameters(2.0, "Hundred Hour");
    setAdsorptionRate(0.06);
    setMaximumLocalMoisture(0.35);
    setMoisture(0.2f);
    initializeStick();
}


//------------------------------------------------------------------------------
/*! \brief Convenience method to create a 1000-h time lag
    DeadFuelMoisture instance and set all all parameters
    \param[in] NONE
    \relates DeadFuelMoisture
    \return Void.
 */
void DeadFuelMoisture::initDeadFuelMoisture1000()
{
    initializeParameters(3.81, "Thousand Hour");
    setAdsorptionRate(0.06);
    setMaximumLocalMoisture(0.35);
    setMoisture(0.2f);
    initializeStick();
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to create a 1-h time lag
    DeadFuelMoisture instance with a 0.20-cm radius (0.16-in diameter).

    \param[in] name Dead fuel moisture stick name.

    \relates DeadFuelMoisture

    \return Pointer to the dynamically-allocated DeadFuelMoisture instance.
 */
DeadFuelMoisture* DeadFuelMoisture::createDeadFuelMoisture1(const string& name)
{
    return(new DeadFuelMoisture(0.2, name));
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to create a 10-h time lag
    DeadFuelMoisture instance with a 0.64-cm radius (0.5-in diameter).

    \param[in] name Dead fuel moisture stick name.

    \relates DeadFuelMoisture

    \return Pointer to the dynamically-allocated DeadFuelMoisture instance.
 */

DeadFuelMoisture* DeadFuelMoisture::createDeadFuelMoisture10( const string& name )
{
    return( new DeadFuelMoisture( 0.64, name ) );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to create a 100-h time lag
    DeadFuelMoisture instance with a 2.0-cm radius (1.57-in diameter).

    \param[in] name Dead fuel moisture stick name.

    \relates DeadFuelMoisture

    \return Pointer to the dynamically-allocated DeadFuelMoisture instance.
 */

DeadFuelMoisture* DeadFuelMoisture::createDeadFuelMoisture100( const string& name )
{
    return( new DeadFuelMoisture( 2.00, name ) );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to create a 1000-h time lag
    lag DeadFuelMoisture instance with a 6.4-cm radius (5-in diameter).

    \param[in] name Dead fuel moisture stick name.

    \relates DeadFuelMoisture

    \return Pointer to the dynamically-allocated DeadFuelMoisture instance.
 */

DeadFuelMoisture* DeadFuelMoisture::createDeadFuelMoisture1000( const string& name )
{
    return( new DeadFuelMoisture( 6.40, name ) );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the adsorption surface mass
    transfer rate for a DeadFuelMoisture stick of given \a radius.

    The adsorption surface mass transfer rate is determined from
    \ref bevins2005
    \f[\alpha = 0.0004509 + \frac {0.006126}{r^2.6}\f]
    where
    - \f$\alpha\f$ is the adsorption surface mass transfer rate
        (\f$\frac {cm^{3}} {cm^{2} \cdot h}\f$), and
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Estimated surface mass transfer rate for adsorption
    (\f$\frac {cm^{3}} {cm^{2} \cdot h}\f$).
 */

double DeadFuelMoisture::deriveAdsorptionRate( double radius )
{
    //double alpha = 0.0004509 + 0.006126 / pow( radius, 2.6 );
	double alpha = 0.06 + 0.006126 / pow(radius, 2.6);
    return( alpha );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the minimum number of
    diffusivity computation time steps per observation
    for a DeadFuelMoisture stick of given \a radius.

    The minimum number of diffusivity computation time steps per observation
    must be large enough to provide stability in the model computations
    (\ref carlson2004a ), and is used to determine the internal diffusivity
    computation time step within update().

    The minumum number of diffusivity computation time steps is determined from
    \ref bevins2005
    \f[$n_{d} = int( 4.777 + \frac {2.496}{r^1.3} )\f]
    where
    - \f$n_{d}\f$ is the minimum number of diffusivity computation time steps
    per observation, and
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Minimum number of diffusivity computation time steps per observation.
 */

int DeadFuelMoisture::deriveDiffusivitySteps( double radius )
{
    int steps = (int) ( 4.777 + 2.496 / pow( radius, 1.3 ) );
    return( steps );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the minimum number of
    moisture content computation time steps per observation
    for a DeadFuelMoisture stick of given \a radius.

    The number of moisture computation time steps per observation must be
    large enough to provide stability in the model computations
    (\ref carlson2004a ), and is used to determine the internal moisture content
    computation time step within update().

    The minimum number of moisture content computation time steps
    is determined from
    \ref bevins2005 \f[$n_{m} = int( 9.8202 + \frac {26.865}{r^1.4} )\f]
    where
    - \f$n_{m}\f$ is the minimum number of moisture content computation time
    steps per observation, and
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Minimum number of moisture computation time steps per observation.
 */

int DeadFuelMoisture::deriveMoistureSteps( double radius )
{
    int steps = (int) ( 9.8202 + 26.865 / pow( radius, 1.4 ) );
    return( steps );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the planar heat transfer rate
    for a DeadFuelMoisture stick of given \a radius.

    The planar heat transfer rate is determined from \ref bevins2005
    \f[$h_{c} = 0.2195 + \frac {0.05260}{r^2.5}\f]
    where
    - \f$h_{c}\f$ is the planar heat transfer rate
        (\f$\frac {cal} {cm^{2} \cdot h \cdot C}\f$), and
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Estimated planar heat transfer rate
        (\f$\frac {cal} {cm^{2} \cdot h \cdot C}\f$).
 */

double DeadFuelMoisture::derivePlanarHeatTransferRate( double radius )
{
    double hc = 0.2195 + 0.05260 / pow( radius, 2.5 );
    return( hc );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the rainfall runoff factor
    for a DeadFuelMoisture stick of given \a radius.

    The initial rainfall runoff factor is determined from \ref bevins2005
    \f[$f_{0} = 0.02822 + \frac {0.1056}{r^2.2}\f]
    where
    - \f$f_{0}\f$ is the initial rainfall runoff rate (\f$dl\f$), and
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Estimated initial rainfall runoff factor (\f$dl\f$).
 */

double DeadFuelMoisture::deriveRainfallRunoffFactor( double radius )
{
    double rrf0 = 0.02822 + 0.1056 / pow( radius, 2.2 );
    return( rrf0 );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to determine the number of moisture content
    radial computation nodes for a DeadFuelMoisture stick of given \a radius.

    The number of moisture computation nodes must be large enough to provide
    stability in the model computations (\ref carlson2004a ).

    The minimum number of moisture content computation nodes
    is determined from
    \ref bevins2005 \f[$n_{m} = int( 10.727 + \frac {0.1746}{r} )\f]
    where
    - \f$n_{m}\f$ is the minimum number of moisture content computation
        radial nodes,
    - \f$r\f$ is the stick radius (\f$cm\f$).

    \param[in] radius Dead fuel stick radius (\f$cm\f$).

    \return Minimum number of moisture computation radial nodes.
 */

int DeadFuelMoisture::deriveStickNodes( double radius )
{
    int nodes = (int) ( 10.727 + 0.1746 / radius );
    if ( ( nodes % 2 ) == 0 )
    {
        nodes++;
    }
    return( nodes );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's desorption surface mass transfer rate.

    \return Current surface mass transfer rate for desorption
    (\f$\frac {cm^{3}} {cm^{2} \cdot h}\f$).
 */

double DeadFuelMoisture::desorptionRate( void ) const
{
    return( m_stcd );
}

//------------------------------------------------------------------------------
/*! \brief Determines bound water diffusivity at each radial nodes.

    \param[in] bp Barometric pressure (cal/m3)
 */

void DeadFuelMoisture::diffusivity ( double bp )
{
	double tk, qv, cpv, dv, ps1, c1, c2, wc, daw, svaw, vfaw, vfcw, rfcw, fac, con, qw, e, dvpr;
	// Loop for each node
    for ( int i=0; i<m_nodes; i++ )
    {
        // Stick temperature (oK)
        //double 
		tk    = m_t[i] + 273.2;
        // Latent heat of vaporization of water (cal/mol
       //double 
		qv    = 13550. - 10.22 * tk;
        // Specific heat of water vapor (cal/(mol*K))
        //double 
		cpv   = 7.22 + .002374 * tk + 2.67e-07 * tk * tk;
        // Sea level atmospheric pressure = 0.0242 cal/cm3
       // double 
		dv    = 0.22 * 3600. * ( 0.0242 / bp )
                     * pow( ( tk / 273.2 ), 1.75 );
        // Water saturation vapor pressure at surface temp (cal/cm3)
        //double 
			ps1   = 0.0000239 * exp(20.58 - (5205. / tk));
        // Emc sorption isotherm parameter (g/g)
        //double 
			c1    = 0.1617 - 0.001419 * m_t[i];
        // Emc sorption isotherm parameter (g/g)
        //double 
			c2    = 0.4657 + 0.003578 * m_t[i];
        // Lesser of nodal or fiber saturation moisture (g/g)
       // double 
			//wc;
        // Reciprocal slope of the sorption isotherm
        double dhdm = 0.0;
        if ( m_w[i] < m_wsa )
        {
            wc = m_w[i];
            if ( c2 != 1. && m_hf<1.0 && c1 != 0.0 && c2 != 0.0 )
            {
                dhdm = (1.0 - m_hf) * pow(-log(1.0-m_hf), (1.0 - c2))
                     / (c1 * c2);
            }
        }
        else
        {
            wc = m_wsa;
            if ( c2 != 1. && Hfs<1.0 && c1 != 0.0 && c2 != 0.0 )
            {
                dhdm = (1.0 - Hfs) * pow(Wsf, (1.0 - c2)) / (c1 * c2);
            }
        }
        // Density of adsorbed water (g/cm3)
        //double 
			daw  = 1.3 - 0.64 * wc;
        // Specific volume of adsorbed water (cm3/g)
        //double 
			svaw = 1. / daw;
        // Volume fraction of adborbed water (dl)
        //double 
			vfaw = svaw * wc / (0.685 + svaw * wc);
        // Volume fraction of moist cell wall (dl)
        //double 
			vfcw = (0.685 + svaw * wc) / ((1.0 / m_density) + svaw * wc);
        // Converts D from wood substance to whole wood basis
        //double 
			rfcw = 1.0 - sqrt(1.0 - vfcw);
        // Converts D from wood substance to whole wood basis
        //double 
			fac  = 1.0 / (rfcw * vfcw);
        // Correction for tortuous paths in cell wall
       // double 
			con  = 1.0 / (2.0 - vfaw);
        // Differential heat of sorption of water (cal/mol)
        //double
			qw   = 5040. * exp(-14.0 * wc);
        // Activation energy for bound water diffusion (cal/mol)
        //double 
			e    = (qv + qw - cpv * tk) / 1.2;

        //----------------------------------------------------------------------
        // The factor 0.016 below is a correction for hindered water vapor
        // diffusion (this is 62.5 times smaller than the bulk vapor diffusion);
        //  0.0242 cal/cm3 = sea level atm pressure
        //      -- note from Ralph Nelson
        //----------------------------------------------------------------------

        //double 
			dvpr = 18.0 * 0.016 * (1.0-vfcw) * dv * ps1 * dhdm
                    / ( m_density * 1.987 * tk );
        m_d[i] = dvpr + 3600. * 0.0985 * con * fac * exp(-e/(1.987*tk));
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's number of moisture diffusivity computation
    time steps per observation.

    \return Current number of moisture diffusivity computation time steps
    per observation.
 */

int DeadFuelMoisture::diffusivitySteps( void ) const
{
    return( m_dSteps );
}

//------------------------------------------------------------------------------
/*! \brief Access to the current total running elapsed time.

    \return The current total running elapsed time (h).
 */

double DeadFuelMoisture::elapsedTime( void ) const
{
    return( m_elapsed );
}

//------------------------------------------------------------------------------
/*! \brief Reports whether the client has called initializeEnvironment().

    \retval TRUE if client has called initializeEnvironment().
    \retval FALSE if client has not called initializeEnvironment().
 */

bool DeadFuelMoisture::initialized( void ) const
{
    return( m_init );
}

//------------------------------------------------------------------------------
/*! \brief Initializes a dead fuel moisture stick's internal and external
    environment and its DateTime.

    Initializes the stick's internal and external environmental variables.
    The stick's internal temperature and water content are presumed to be
    uniformly distributed.

    This overloaded version also initializes the stick's Julian date from
    the passed date and time parameters.  The first call to update()
    calculates elapsed time from the date/time passed here.

    \param[in] year     Observation year (4 digits).
    \param[in] month    Observation month (Jan==1, Dec==12).
    \param[in] day      Observation day-of-the-month [1..31].
    \param[in] hour     Observation elapsed hours in the day [0..23].
    \param[in] minute   Observation elapsed minutes in the hour (0..59].
    \param[in] second   Observation elapsed seconds in the minute [0..59].
    \param[in] ta Initial ambient air temperature (oC).
    \param[in] ha Initial ambient air relative humidity (g/g).
    \param[in] sr Initial solar radiation (W/m2).
    \param[in] rc Initial cumulative rainfall amount (cm).
    \param[in] ti Initial stick temperature (oC).
    \param[in] hi Initial stick surface relative humidty (g/g).
    \param[in] wi Initial stick fuel moisture fraction (g/g).
    \param[in] bp Initial stick barometric pressure (cal/cm3).
 */

void DeadFuelMoisture::initializeEnvironment (
    int     year,
    int     month,
    int     day,
    int     hour,
    int     minute,
    int     second,
    double  ta,
    double  ha,
    double  sr,
    double  rc,
    double  ti,
    double  hi,
    double  wi,
    double  bp
)
{
    //m_semTime.set( year, month, day, hour, minute, second );
    initializeEnvironment( ta, ha, sr, rc, ti, hi, wi, bp );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Initializes a dead fuel moisture stick's internal and external
    environment.

    Initializes the stick's internal and external environmental variables.
    The stick's internal temperature and water content are presumed to be
    uniformly distributed.

    \param[in] ta Initial ambient air temperature (oC).
    \param[in] ha Initial ambient air relative humidity (g/g).
    \param[in] sr Initial solar radiation (W/m2).
    \param[in] rc Initial cumulative rainfall amount (cm).
    \param[in] ti Initial stick temperature (oC).
    \param[in] hi Initial stick surface relative humidty (g/g).
    \param[in] wi Initial stick fuel moisture fraction (g/g).
    \param[in] bp Initial stick barometric pressure (cal/cm3).
 */

void DeadFuelMoisture::initializeEnvironment (
    double ta,
    double ha,
    double sr,
    double rc,
    double ti,
    double hi,
    double wi,
    double bp
)
{
    // Environment initialization
    m_ta0 = m_ta1 = ta;     // Previous and current ambient air temperature (oC)
    m_ha0 = m_ha1 = ha;     // Previous and current ambient air relative humidity (g/g)
    m_sv0 = m_sv1 = sr/Smv; // Previous and current solar insolation (millivolts)
    m_rc0 = m_rc1 = rc;     // Previous and current cumulative rainfall amount (cm)
    m_ra0 = m_ra1 = 0.;     // Previous and current observation period's rainfall amount (cm)
    m_bp0 = m_bp1 = bp;     // Previous and current observation barometric pressure (cal/m3)

    // Stick initialization
    m_hf    = hi;           // Relative humidity at fuel surface (g/g)
	m_hf = (m_hf > Hfs) ? Hfs : m_hf;		// m_hf should never be greater than 0.99 or it breaks other code calcs (i.e. diffusivity)
    m_wfilm = 0.0;          // Water film moisture contribution (g/g)
    m_wsa   = wi + .1;      // Stick fiber saturation point (g/g)
    fill( m_t.begin(), m_t.end(), ti );
    fill( m_w.begin(), m_w.end(), wi );
    fill( m_s.begin(), m_s.end(), 0.0 );

    diffusivity( m_bp0 );
    m_init = true;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Initializes a DeadFuelMoisture stick with model parameters inferred
    from the \a radius.

    \param[in] radius Dead fuel stick radius (cm).
    \param[in] name Name or description of the dead fuel stick.
 */

void DeadFuelMoisture::initializeParameters( double radius, const string& name )
{
    initializeParameters(
        name,
        radius,
        deriveStickNodes( radius ),
        deriveMoistureSteps( radius ),
        deriveDiffusivitySteps( radius ),
        derivePlanarHeatTransferRate( radius ),
        deriveAdsorptionRate( radius ),
        deriveRainfallRunoffFactor( radius ),
        41.0,   // stick length (cm)
        0.400,  // stick density (g/cm3)
        0.0,    // water film contribution "wfilmk"
        0.60,   // maximum local moisture content "wmx"
        0.06,   // desorption rate "stcd"
        0.5,    // rainfall adjustment factor "rai1" (not used)
        9999.,  // storm transition value "stv" (not used)
        0,      // random number seed (none)
        false,  // If TRUE, applies Nelson's logic for rainfall runoff after the first hour
        false,  // If TRUE, applies Nelson's logic for rainstorm transition and state
        false,   // If TRUE, the continuous liquid column condition get pertubated
        false    // If TRUE, used Bevins' ramping of rainfall runoff factor rather than Nelsons rai0 *= 0.15
    );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Initializes a DeadFuelMoisture stick.

    Initializes a dead fuel moisture stick, applying the passed parameters.

    \param[in] name Name or description of the dead fuel stick.
    \param[in] radius Dead fuel stick radius (cm).
    \param[in] stickNodes Number of stick nodes in the radial direction
                [required]
    \param[in] moistureSteps Number of moisture content computation steps
                per observation [required].
    \param[in] diffusivitySteps Number of diffusivity computation steps per
                observation [required].
    \param[in] planarHeatTransferRate Stick planar heat transfer rate
                (cal/cm2-h-C) [required].
    \param[in] adsorptionRate Adsorption surface mass transfer rate
                ((cm3/cm2)/h) [required].
    \param[in] rainfallRunoffFactor Rain runoff factor (dl) [required].
    \param[in] stickLength Stick length (cm) [optional, default = 41 cm].
    \param[in] stickDensity Stick density (g/cm3) [optional, default = 0.40].
    \param[in] waterFilmContribution Water film contribution to stick moisture
                content (g water/g dry fuel) [optional, default = 0].
    \param[in] localMaxMc Stick maximum local moisture due to rain
                (g water/g dry fuel) [optional, default = 0.6].
    \param[in] desorptionRate Desorption surface mass transfer rate
                ((cm3/cm2)/h) [optional, default = 0.06].
    \param[in] rainfallAdjustmentFactor [not used].
    \param[in] stormTransitionValue Storm transition value (cm/h) [not used].
    \param[in] randseed If not zero, nodal temperature, saturation, and
                moisture contents are pertibated by some small amount.
                If > 0, this value is used as the seed.
                If < 0, uses system clock for seed.
    \param[in] allowRainfall2 If TRUE, applies Nelson's logic for rainfall runoff
                after the first hour [default=true].
    \param[in] allowRainstorm If TRUE, applies Nelson's logic for rainstorm transition
                and state [default=true].
    \param[in] pertubateColumn If TRUE, whenever the continuous liquid column condition
                occurs, the nodal moistures get pertubated by a small amount [default=true].
    \param[in] rampRai0 If TRUE, uses Bevins' ramping of rainfall runoff factor
                rather than Nelsons rai0 *= 0.15 [default=false].
 */

void DeadFuelMoisture::initializeParameters(
        const string& name,
        double  radius,
        int     stickNodes,
        int     moistureSteps,
        int     diffusivitySteps,
        double  planarHeatTransferRate,
        double  adsorptionRate,
        double  rainfallRunoffFactor,
        double  stickLength,
        double  stickDensity,
        double  waterFilmContribution,
        double  localMaxMc,
        double  desorptionRate,
        double  rainfallAdjustmentFactor,
        double  stormTransitionValue,
        int     randseed,
        bool    allowRainfall2,
        bool    allowRainstorm,
        bool    pertubateColumn,
        bool    rampRai0
     )
{
    // Start with everything set to zero
    zero();
    // Constrain and store the passed parameters
    m_name     = name;
    m_randseed = randseed;
    m_radius   = radius;
    m_length   = stickLength;
    m_density  = stickDensity;
    m_dSteps   = diffusivitySteps;
    m_hc       = planarHeatTransferRate;
    m_nodes    = stickNodes;
    m_rai0     = rainfallRunoffFactor;
    m_rai1     = rainfallAdjustmentFactor;
    m_stca     = adsorptionRate;
    m_stcd     = desorptionRate;
    m_mSteps   = moistureSteps;
    m_stv      = stormTransitionValue;
    m_wmx      = localMaxMc;
    m_wfilmk   = waterFilmContribution;
    m_allowRainfall2  = allowRainfall2;
    m_allowRainstorm  = allowRainstorm;
    m_pertubateColumn = pertubateColumn;
    m_rampRai0        = rampRai0;
    // Initialize all other stick parameters and intermediates
    initializeStick();
    return;
}

//------------------------------------------------------------------------------
/*! \brief Initializes a dead fuel moisture stick's model parameters.

    \note Should be called after and set<parameter>().
 */

void DeadFuelMoisture::initializeStick( void )
{
    // Should we randomize nodal moisture, saturation, and temperatures by some
    // small, insignificant amount to introduce computational stability when
    // propagating these parameters within the stick?
    // If > 0, the value is used as a random generator seed.
    // If < 0, the system clock is used to get the seed.
    setRandomSeed( m_randseed );

    // Internodal distance (cm)
    m_dx = m_radius / (double ) ( m_nodes - 1 );
    m_dx_2 = m_dx * 2.;

    // Maximum possible stick moisture content (g/g)
    m_wmax = ( 1. / m_density ) - ( 1. / 1.53 );

    // Initialize ambient air temperature to 20 oC
	m_t.clear();
    m_t.insert( m_t.begin(), m_nodes, 20.0 );

    // Initialize fiber saturation point to 0 g/g
	m_s.clear();
    m_s.insert( m_s.begin(), m_nodes, 0.0 );

    // Initialize bound water diffusivity to 0 cm2/h
	m_d.clear();
    m_d.insert( m_d.begin(), m_nodes, 0.0 );

    // Initialize moisture content to half the local maximum (g/g)
	m_w.clear();
    m_w.insert( m_w.begin(), m_nodes, ( 0.5 * m_wmx ) );
	//m_w.insert(m_w.begin(), m_nodes, (m_wmx));

    // Derive nodal radial distances
    m_x.resize(0);
    for ( int i=0; i<m_nodes-1; i++ )
    {
        // Initialize radial distance from center of stick (cm)
        m_x.push_back( m_radius - ( m_dx * i) );
    }
    m_x.push_back( 0.0 );

    // Derive nodal volume fractions
    m_v.resize(0);
    double ro = m_radius;
    double ri = ro - 0.5 * m_dx;
    double a2 = m_radius * m_radius;
    m_v.push_back( ( ro * ro - ri * ri ) / a2 );
    double vwt = m_v[0];
    for ( int i=1; i<m_nodes-1; i++ )
    {
        ro = ri;
        ri = ro - m_dx;
        m_v.push_back( ( ro * ro - ri * ri) / a2 );
        vwt += m_v[i];
    }
    m_v.push_back( ri*ri / a2 );
    vwt += m_v[m_nodes-1];

    // Added by Stuart Brittain on 1/14/2007
    // for performance improvement in update()
	m_Twold.clear();
	m_Ttold.clear();
	m_Tsold.clear();
	m_Tv.clear();
	m_To.clear();
	m_Tg.clear();
	m_Twold.insert( m_Twold.begin(), m_nodes, 0.0 );
    m_Ttold.insert( m_Ttold.begin(), m_nodes, 0.0 );
    m_Tsold.insert( m_Tsold.begin(), m_nodes, 0.0 );
    m_Tv.insert( m_Tv.begin(), m_nodes, 0.0 );
    m_To.insert( m_To.begin(), m_nodes, 0.0 );
    m_Tg.insert( m_Tg.begin(), m_nodes, 0.0 );

    // Initialize the environment, but set m_init to FALSE when done
    initializeEnvironment(
        20.,        // Ambient air temperature (oC)
        0.20,       // Ambient air relative humidity (g/g)
        0.0,        // Solar radiation (W/m2)
        0.0,        // Cumulative rainfall (cm)
        20.0,       // Initial stick temperature (oC)
        0.20,       // Initial stick surface humidity (g/g)
        0.5*m_wmx   // Initial stick moisture content
    );
    m_init = false;

    //-------------------------------------------------------------------------
    // Computation optimization parameters
    //-------------------------------------------------------------------------

    // m_hwf == hw and aml computation factor used in update()
    m_hwf = 0.622 * m_hc * pow(( Pr / Sc), 0.667 );

    // m_amlf == aml optimization factor
    m_amlf = m_hwf / ( 0.24 * m_density * m_radius );

    // m_capf = cap optimization factor. */
    double rcav = 0.5 * Aw * Wl;
    m_capf = 3600. * Pi * St * rcav * rcav
           / ( 16. * m_radius * m_radius * m_length * m_density );

    // m_vf == optimization factor used in update()
    // WAS: m_vf = St / (Aw * Wl * Scr);
    // WAS: m_vf = St / (Wl * Scr);
    m_vf = St / ( m_density * Wl * Scr );
    return;
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current maximum local moisture content.

    \return Stick's current maximum local moisture content (g/g).
 */

double DeadFuelMoisture::maximumLocalMoisture( void ) const
{
    return( m_wmx );
}

//------------------------------------------------------------------------------
/*! \brief Determines the mean moisture content of the stick's radial profile.

    \note
    The integral average of the stick's nodal moisture contents is calculated
    without consideration to the nodes' volumetric representation.

    \deprecated Use Fms_MeanWtdMoisture() for a volume-weighted mean
    moisture content.

    \return The mean moisture content of the stick's radial profile (g/g).
 */

double DeadFuelMoisture::meanMoisture( void ) const
{
    double wea, web;
    double wec = m_w[0];
    double wei = m_dx / ( 3. * m_radius );
    for ( int i=1; i<m_nodes-1; i+=2 )
    {
        wea = 4. * m_w[i];
        web = 2. * m_w[i+1];
        if ( ( i + 1 ) == ( m_nodes - 1 ) )
        {
            web = m_w[ m_nodes-1 ];
        }
        wec += web + wea;
    }
    double wbr = wei * wec;
    wbr = ( wbr > m_wmx ) ? m_wmx : wbr;

    // Add water film
    wbr += m_wfilm;
    return( wbr );
}

//------------------------------------------------------------------------------
/*! \brief Determines the volume-weighted mean moisture content of the
    stick's radial profile.

    \return The volume-weighted mean moisture content of the stick's radial
    profile (g/g).
 */

double DeadFuelMoisture::meanWtdMoisture( void ) const
{
    double wbr = 0.0;
	for ( int i=0; i<m_nodes; i++ )
    {
        wbr += m_w[i] * m_v[i];
	}
		
    wbr = ( wbr > m_wmx ) ? m_wmx : wbr;

    // Add water film
    wbr += m_wfilm;
	return( wbr );

}

//------------------------------------------------------------------------------
/*! \brief Determines the median of the radial moisture profile

\return The volume-weighted mean moisture content of the stick's radial
profile (g/g).
*/

double DeadFuelMoisture::medianRadialMoisture(void) const
{
	vector<double> vMw;
	for (int i = 0; i<m_nodes; i++)
	{
		
		vMw.push_back(m_w[i]);
	}
	// Sort the radial node moisture vector
	std::sort(vMw.begin(), vMw.end());
	
	
	return(vMw[(int)vMw.size() / 2]);

}

//------------------------------------------------------------------------------
/*! \brief Determines the volume-weighted mean temperature of the stick's
    radial profile.

    \return The volume-weighted mean temperature of the stick's radial
    profile (oC).
 */

double DeadFuelMoisture::meanWtdTemperature( void ) const
{
    double wbr = 0.0;
    for ( int i=0; i<m_nodes; i++ )
    {
        wbr += m_t[i] * m_v[i];
    }
    return( wbr );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's number of moisture content computation time
    steps per observation.

    \return Current number of moisture content computation time steps
     per observation.
 */

int DeadFuelMoisture::moistureSteps( void ) const
{
    return( m_mSteps );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's name.

    \return The stick's name.
 */

string DeadFuelMoisture::name( void ) const
{
    return( m_name );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current precipitation rate (cm/h).

    \return The current (most recent) precipitation rate (cm/h).
 */

double DeadFuelMoisture::pptRate( void ) const
{
    return( ( m_et > 0.00 ) ? ( m_ra1 / m_et ) : 0.00 );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current planar heat transfer rate..

    \return Stick's current planar heat transfer rate
        (\f$\frac {cal} {cm^{2} \cdot h \cdot C}\f$).
 */

double DeadFuelMoisture::planarHeatTransferRate( void ) const
{
    return( m_hc );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current rainfall runoff factor.

    \return Stick's current rainfall runoff factor (dl).
 */

double DeadFuelMoisture::rainfallRunoffFactor( void ) const
{
    return( m_rai0 );
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's adsorption rate.

    \param[in] adsorptionRate Adsorption surface mass transfer rate ((cm3/cm2)/h).
 */

void DeadFuelMoisture::setAdsorptionRate( double adsorptionRate )
{
    m_stca = adsorptionRate;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's configuration to toggle Nelson's logic
    for rainfall runoff factor after the first hour of rain.

    \param[in]  allow (default=true).
 */

void DeadFuelMoisture::setAllowRainfall2( bool allow )
{
    m_allowRainfall2 = allow;;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's configuration to toggle Nelson's logic
    to allow the Rainstorm state and rainfall-to-rainstorm transition.

    \param[in]  allow (default=true).
 */

void DeadFuelMoisture::setAllowRainstorm( bool allow )
{
    m_allowRainstorm = allow;;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's desorption rate.

    \param[in] desorptionRate Desorption surface mass transfer rate ((cm3/cm2)/h).
 */

void DeadFuelMoisture::setDesorptionRate( double desorptionRate )
{
    m_stcd = desorptionRate;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's diffusivity computation steps per update().

    \param[in] diffusivitySteps Number of diffusivity computation steps per observation.
 */

void DeadFuelMoisture::setDiffusivitySteps( int diffusivitySteps )
{
    m_dSteps = diffusivitySteps;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's maximum local moisture content.

    \param[in] localMaxMc Stick maximum local moisture due to rain (g water/g dry fuel).
 */

void DeadFuelMoisture::setMaximumLocalMoisture( double localMaxMc )
{
    m_wmx = localMaxMc;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's moisture content computation steps per update().

    \param[in] moistureSteps Number of moisture content computation steps per observation.
 */

void DeadFuelMoisture::setMoistureSteps( int moistureSteps )
{
    m_mSteps = moistureSteps;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's planar heat transfer rate.

    \param[in] planarHeatTransferRate Stick planar heat transfer rate (cal/cm2-h-C).
 */

void DeadFuelMoisture::setPlanarHeatTransferRate( double planarHeatTransferRate )
{
    m_hc = planarHeatTransferRate;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's column pertubation configuration.

    \param[in] pertubate (default=true).
 */

void DeadFuelMoisture::setPertubateColumn( bool pertubate )
{
    m_pertubateColumn = pertubate;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's rainfall runoff factor.

    \param[in] rainfallRunoffFactor Rain runoff factor during the initial hour of rainfall (dl).
 */

void DeadFuelMoisture::setRainfallRunoffFactor( double rainfallRunoffFactor )
{
    m_rai0 = rainfallRunoffFactor;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick's configuration to toggle Bevins' logic
    that ramps the rainfall runoff factor during falling humidity, rather than
    use Nelson's rai0 *= 0.15.

    \param[in]  ramp (default=true).
 */

void DeadFuelMoisture::setRampRai0( bool ramp )
{
    m_rampRai0 = ramp;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Sets the ransom seed behavior.

    \param[in] randseed If not zero, nodal temperature, saturation,
    and moisture contents are pertubated by some small amount.
    If > 0, this value is used as the seed. If < 0, uses system clock for seed.
 */

void DeadFuelMoisture::setRandomSeed( int randseed )
{
    m_randseed = randseed;
    if ( m_randseed > 0 )
    {
        srand( (unsigned int) m_randseed );
    }
    else if ( m_randseed < 0 )
    {
        srand( (unsigned int) time(NULL) );
    }
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick density.

    \param[in] stickDensity Stick density (g/cm3) [optional, default = 0.40].
 */

void DeadFuelMoisture::setStickDensity( double stickDensity )
{
    m_density = stickDensity;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the stick length.

    \param[in] stickLength Stick length (cm) [optional, default= 41 cm].
 */

void DeadFuelMoisture::setStickLength( double stickLength )
{
    m_length = stickLength;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the number of stick radial computation nodes.

    \param[in] stickNodes Number of stick nodes in the radial direction [optional, default = 11].
 */

void DeadFuelMoisture::setStickNodes( int stickNodes )
{
    m_nodes = stickNodes;
    return;
}

//------------------------------------------------------------------------------
/*! \brief Updates the water film contribution to stick weight.

    \param[in] waterFilm Water film contribution to stick moisture content (g water/g dry fuel).
 */

void DeadFuelMoisture::setWaterFilmContribution( double waterFilm )
{
    m_wfilm = waterFilm;
    return;
}


//------------------------------------------------------------------------------
/*! \brief Set all the nodes of a stick to a startup value

\return EXIT_SUCCESS
*/
void DeadFuelMoisture::setMoisture(float initFM)
{
	
	for (int i = 0; i<m_nodes; i++)
	{
		m_w[i] = initFM;
		
	}
	return;
	

}
//------------------------------------------------------------------------------
/*! \brief Access to prevailing state for most recent update.

    \return Prevailing state for most recent update.
 */

int DeadFuelMoisture::state( void ) const
{
    return( m_state );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current state name.

    \return The current state name.
 */

const char* DeadFuelMoisture::stateName( void ) const
{
    static const char *State[] =
    {
        "None",             // 0,
        "Adsorption",       // 1,
        "Desorption",       // 2,
        "Condensation1",    // 3,
        "Condensation2",    // 4,
        "Evaporation",      // 5,
        "Rainfall1",        // 6,
        "Rainfall2",        // 7,
        "Rainstorm",        // 8,
        "Stagnation",       // 9,
        "Error"             // 10
    };
    return( State[m_state] );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's particle density.

    \return Current stick density (g/cm3).
 */

double DeadFuelMoisture::stickDensity( void ) const
{
    return( m_density );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's length.

    \return Current stick length (cm).
 */

double DeadFuelMoisture::stickLength( void ) const
{
    return( m_length );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's number of moisture content radial computation
    nodes.

    \return Current number of moisture content radial computation  nodes.
     per observation.
 */

int DeadFuelMoisture::stickNodes( void ) const
{
    return( m_nodes );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's surface fuel moisture content.

    \return The current surface fuel moisture content (g/g).
 */

double DeadFuelMoisture::surfaceMoisture( void ) const
{
    return( m_w[0] );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's surface fuel temperature.

    \return The current surface fuel temperature (oC).
 */

double DeadFuelMoisture::surfaceTemperature( void ) const
{
    return( m_t[0] );
}

//------------------------------------------------------------------------------
/*! \brief Static convenience method to derive a random number uniformly
    distributed in the range [\a min .. \a max].

    \param[in] min  Minimum range value.
    \param[in] max  Maximum range value.

    Uses the system rand() to generate the number.

    \return A uniformly distributed random number within [\a min .. \a max].
 */

double DeadFuelMoisture::uniformRandom( double min, double max )
{
    return( (max - min) * ( (double) rand() / (double) RAND_MAX ) + min );
}

bool isLeapYear(int year)
{
	bool isLeap = false;
	if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
		isLeap = true;
	return isLeap;
}
const int SecondsPerMinute = 60;
const int SecondsPerHour = 3600;
const int SecondsPerDay = 86400;
const int DaysOfMonth[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

time_t mkgmtime(short year, short month, short day, short hour, short minute, short second, int *jDay)
{
	time_t secs = 0;
	int doy = 0;
	for (short y = 1970; y < year; ++y)
		secs += (isLeapYear(y) ? 366 : 365) * SecondsPerDay;
	for (short m = 1; m < month; ++m) 
	{
		secs += DaysOfMonth[m - 1] * SecondsPerDay;
		doy += DaysOfMonth[m - 1];
		if (m == 2 && isLeapYear(year))
		{
			secs += SecondsPerDay;
			doy++;
		}
	}
	secs += (day - 1) * SecondsPerDay;
	secs += hour * SecondsPerHour;
	secs += minute * SecondsPerMinute;
	secs += second;
	doy += (day - 1);
	*jDay = doy;
	return secs;
}

//------------------------------------------------------------------------------
/*! \brief Updates a dead moisture stick's internal and external environment
    based on the current weather observation values.

    This overloaded version accepts the current date and time as arguments,
    and automatically calculates elapsed time since the previous update().

    \note The client must have called the corresponding initializeEnvironment()
    method that accepts date and time arguments to ensure that the date and
    time has been initialized.

    \note The client program should try it's hardest to catch all bad input data
    and corrected or duplicated records \b before calling this method.

    \param[in] year     Observation year (4 digits).
    \param[in] month    Observation month (Jan==1, Dec==12).
    \param[in] day      Observation day-of-the-month [1..31].
    \param[in] hour     Observation elapsed hours in the day [0..23].
    \param[in] minute   Observation elapsed minutes in the hour (0..59].
    \param[in] second   Observation elapsed seconds in the minute [0..59].
    \param[in] at   Current observation's ambient air temperature (oC).
    \param[in] rh   Current observation's ambient air relative humidity (g/g).
    \param[in] sW   Current observation's solar radiation (W/m2).
    \param[in] rcum Current observation's total cumulative rainfall amount (cm).
    \param[in] bpr  Current observation's stick barometric pressure (cal/cm3).

    \retval TRUE if all inputs are ok and the stick is updated.
    \retval FALSE if inputs are out of range and the stick is \b not updated.
 */


bool DeadFuelMoisture::update(
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
        double  bpr,
        bool prcpAsAmnt
    )
{
	//const time_t SECS_PER_HOUR = 60 * 60;
    // Determine Julian date for this new observation
    time_t loctime;
	int jDay = 1;
	loctime = mkgmtime(year, month, day, hour, minute, second, &jDay);
	double seconds = (loctime - obstime);//our mkgmtime is always seconds! Removed OS ambiguity and calls to mktime() and difftime() SB 20201/01/13
#ifdef DEBUG
    if(seconds <= 0){
        std::cout << m_Hour << " " << hour << endl;
        std::cout << m_Day << " " << day << endl;
        std::cout << m_Month << " " << month << endl;
        std::cout << m_Year << " " << year << endl;
        std::cout << loctime << " " << obstime << endl;
        char str1[80],str2[80];
        strftime (str1,80,"%Y%m%d%H:%M:%S",localtime(&loctime));
        strftime (str2,80,"%Y%m%d%H:%M:%S",localtime(&obstime));
        std::cout << str1 << " " << str2 << endl;
    }
#endif

    m_Hour = hour;
    m_Day = day;
    m_Month = month;
    m_Year = year;
	m_Jday = jDay;
	obstime = loctime;

    // Determine elapsed time (h) between the current and previous dates
    double et = seconds / 3600;

    // If the Julian date wasn't initialized,
    // or if the new time is less than the old time,
    // assume a 1-h elapsed time.
    //if ( jd1 < jd0 )
    //{
    //    et = 1.0;
    //}
#ifdef DEBUG
fprintf( stderr, "%d/%02d/%02d %02d:%02d:%02d\n",
year, month, day, hour, minute, second );
#endif
    if(m_updates == 0) et = 1;
    // Update!
    return( update( et, at, rh, sW, rcum, bpr,prcpAsAmnt ) );
}

//------------------------------------------------------------------------------
/*! \brief Updates a dead moisture stick's internal and external environment
    based on the passed (current) weather observation values.

    This overloaded version accepts the elapsed time since the previous
    observation, and does not automatically update the Julian date.

    \param[in] et   Elapsed time since the previous observation (h).
    \param[in] at   Current observation's ambient air temperature (oC).
    \param[in] rh   Current observation's ambient air relative humidity (g/g).
    \param[in] sW   Current observation's solar radiation (W/m2).
    \param[in] rcum Current observation's total cumulative rainfall amount (cm).
    \param[in] bpr  Current observation's stick barometric pressure (cal/cm3).

    \note The client program should try it's hardest to catch all bad input data
    and corrected or duplicated records \b before calling this method.

    \retval TRUE if all inputs are ok and the stick is updated.
    \retval FALSE if inputs are out of range and the stick is \b not updated.
 */

bool DeadFuelMoisture::update(
    double  et,
    double  at,
    double  rh,
    double  sW,
    double  rcum,
    double  bpr,
    bool prcpAsAmnt
)
{
    // Increment update counter
    m_updates++;
    m_elapsed += et;

    //--------------------------------------------------------------------------
    // Catch bad data here
    // The client program should try to catch all bad data and corrected records
    // before calling this function.
    //--------------------------------------------------------------------------

    // If the elapsed time < 0.00027 hours (<0.01 sec), then treat this as
    // a duplicate or corrected observation and return
    if (et < 0.0000027)
    {
        ostringstream str;
        str << "DeadFuelMoisture::update() "
            << m_updates
            << " has a regressive elapsed time of "
            << et
            << " hours.";
        cerr << str.str() << "\n";

        // Msg::Instance().userWarning( str.str() );
        return(false);
    }
    // Cumulative rainfall must equal or exceed its previous value
    if (rcum < m_rc1 && !prcpAsAmnt)
    {
        ostringstream str;
        str << "DeadFuelMoisture::update() "
            << m_updates
            << " has a regressive cumulative rainfall amount of "
            << rcum
            << " cm.";
        cerr << str.str() << "\n";
        //Msg::Instance().userWarning( str.str() );
        // Assume a RAWS station reset and return
        m_rc1 = rcum;
        m_ra0 = 0.;
        return(false);
    }
    // Relative humidity must be reasonable
    if (rh < 0.001 || rh > 1.0)
    {
        ostringstream str;
        str << "DeadFuelMoisture::update() "
            << m_updates
            << " has a an out-of-range relative humidity of  "
            << rh
            << " g/g.";
        cerr << str.str() << "\n";
        //Msg::Instance().userWarning( str.str() );
        return(false);
    }
    // Ambient temperature must be reasonable
    if (at < -60. || at > 60.)
    {
        ostringstream str;
        str << "DeadFuelMoisture::update() "
            << m_updates
            << " has a an out-of-range air temperature of  "
            << at
            << " oC.";
        cerr << str.str() << "\n";
        //Msg::Instance().userWarning( str.str() );
        return(false);
    }
    // Insolation must be reasonable
    sW = (sW < 0.0) ? 0.0 : sW;
    if (sW > 2000.)
    {
        ostringstream str;
        str << "DeadFuelMoisture::update() "
            << m_updates
            << " has a an out-of-range solar insolation of  "
            << sW
            << " W/m2.";
        cerr << str.str() << "\n";
        //Msg::Instance().userWarning( str.str() );
        return(false);
    }

    // First save the previous weather observation values
    m_ta0 = m_ta1;      // Previous air temperature (oC)
    m_ha0 = m_ha1;      // Previous air relative humidity (g/g)
    m_sv0 = m_sv1;      // Previous pyranometer voltage (millivolts)
    m_rc0 = m_rc1;      // Previous cumulative rainfall (cm)
    m_ra0 = m_ra1;      // Previous period's rainfall amount (cm)
    m_bp0 = m_bp1;      // Previous barometric pressure (cal/m3)

    // Then save the current weather observation values
    m_ta1 = at;         // Current air temperature (oC)
    m_ha1 = rh;         // Current air relative humidity (g/g)
    m_sv1 = sW / Smv;   // Current pyranometer voltage (millivolts)
    m_rc1 = rcum;       // Current cumulative rainfall (cm)
    m_bp1 = bpr;        // Current barometric pressure (cal/m3)
    m_et = et;         // Current elapsed time since previous update (h)

    // Precipitation amount since last observation
    if (!prcpAsAmnt) {m_ra1 = m_rc1 - m_rc0;
    }
    else
    {
        m_ra1 = rcum;
    }
    // If no precipitation, reset the precipitation duration timer
    m_rdur = ( m_ra1 < 0.0001 ) ? 0.0 : m_rdur;
    // Precipitation rate since last observation adjusted by Pi (cm/h)
    m_pptrate = m_ra1 / et / Pi;
    // Determine moisture computation time step interval (h)
    m_mdt   = et / (double) m_mSteps;
    m_mdt_2 = m_mdt * 2.;
    // Nelson's "s" factor used in update() loop
    m_sf = 3600. * m_mdt / ( m_dx_2 * m_density );
    // Determine bound water diffusivity time step interval (h)
    m_ddt = et / (double) m_dSteps;
    // First hour runoff factor h-(g/(g-h))
    double rai0 = m_mdt * m_rai0 * ( 1.0 - exp(-100. * m_pptrate) );
    // Adjustment for rainfall cases when humidity is dropping
    if ( m_ha1 < m_ha0 )
    {
        if ( m_rampRai0 )
        {
            rai0 *= ( 1.0 - ( ( m_ha0 - m_ha1 ) / m_ha0 ) );
        }
        else
        {
            rai0 *= 0.15;
        }
    }
    // Subsequent runoff factor h-(g/(g/h))
    double rai1 = m_mdt * m_rai1 * m_pptrate;

    // DFM state counter
    int tstate[DFM_States];
    for ( int i=0; i<DFM_States; i++ )
    {
        tstate[i] = 0;
    }
    // Next time (tt) to run diffusivity computations.
    double ddtNext = m_ddt;
    // Elapsed moisture computation time (h)
    double tt = m_mdt;
    // Loop for each moisture time step between environmental inputs.
    for ( int nstep=1; tt <= et; tt = nstep*m_mdt, nstep++ )
    {
        // Fraction of time elapsed between previous and current obs (dl)
        double tfract = tt / et;
        // Air temperature interpolated between previous and current obs (oC)
        double ta = m_ta0 + ( m_ta1 - m_ta0 ) * tfract;
        // Air humidity interpolated between previous and current obs (dl)
        double ha = m_ha0 + ( m_ha1 - m_ha0 ) * tfract;
        // Solar radiation interpolated between previous and current obs (millivolts)
        double sv = m_sv0 + ( m_sv1 - m_sv0 ) * tfract;
        // Barometric pressure interpolated between previous and current obs (bal/m3)
        double bp = m_bp0 + ( m_bp1 - m_bp0 ) * tfract;
        // Fraction of the solar constant interpolated between obs (mv)
        double fsc = sv / Srf;
        // Ambient air temperature (oK)
        double tka = ta + Kelvin;
        // Dew point temperature (oK)
        double tdw = 5205. / ( ( 5205. / tka ) - log( ha ) );
        // Dew point temperature (oC)
        double tdp = tdw - Kelvin;
        // Sky temperature (oK)
        double tsk = ( fsc < 0.000001 ) ? Tcn + Kelvin : Tcd + Kelvin;
        // Long wave radiative surface heat transfer coefficient (cal/cm2-h-C)
        double hr  = ( fsc < 0.000001 ) ? Hrn : Hrd ;
        // Solar radiation received by half the stick (cal/cm2-h)
        double sr  = ( fsc < 0.000001 ) ? 0.0 : Srf * fsc;
        // Water saturation vapor pressure in ambient air (cal/cm3)
        double psa = 0.0000239 * exp( 20.58 - ( 5205. / tka ) );
        // Water saturation vapor pressure in air (cal/cm3)
        double pa = ha * psa;
        // Water saturation vapor pressure at dewpoint (cal/cm3)
        double psd = 0.0000239 * exp( 20.58 - ( 5205. / tdw ) );
        // Rainfall duration (h)
        m_rdur = ( m_ra1 > 0.0001 ) ? ( m_rdur + m_mdt ) : 0.;

        //----------------------------------------------------------------------
        // Stick surface temperature and humidity
        //----------------------------------------------------------------------

        // Intermediate stick surface temperature (oC)
        double tfd = ta + ( sr - hr * ( ta - tsk + Kelvin ) ) / ( hr + m_hc );
        // Latent heat of vaporization of water (cal/mole)
        double qv = 13550. - 10.22 * ( tfd + Kelvin );
        // Stick heat transfer coefficient for vapor diffusion above FSP
        double hw = ( m_hwf * Ap / 0.24 ) * qv / 18.;
        // Stick surface temperature (oC)
        m_t[0] = tfd - ( hw * ( tfd - ta ) / ( hr + m_hc + hw ) );

        // Differential heat of sorption of water (cal/mole)
        double qw = 5040. * exp( -14. * m_w[0] );
        // Stick surface temperature (oK)
        double tkf = m_t[0] + Kelvin;
        // Kinematic viscosity of liquid water (cm2/s)
        double gnu = 0.00439 + 0.00000177 * pow( ( 338.76 - tkf ), 2.1237 );

        // EMC sorption isotherm parameter (g/g)
        double c1 = 0.1617 - 0.001419 * m_t[0];
        // EMC sorption isotherm parameter (g/g)
        double c2 = 0.4657 + 0.003578 * m_t[0];
        // Stick fiber saturation point (g/g)
        m_wsa = c1 * pow( Wsf, c2 );
        // Maximum minus current fiber saturation (g/g)
        double wdiff = m_wmax - m_wsa;
        wdiff = ( wdiff < 0.000001 ) ? 0.000001 : wdiff;
        // Water saturation vapor pressure at surface temp (cal/cm3)
        double ps1 = 0.0000239 * exp( 20.58 - ( 5205. / tkf ) );
        // Water vapor pressure at the stick surface (cal/cm3)
        double p1 = pa + Ap * bp * ( qv / (qv + qw) ) * ( tka - tkf );
        //std::cout << pa << " " << Ap << " " << bp << " " << qv << " " << qw << " " << tka << " " << tkf << " " << endl;
        p1 = ( p1 < 0.000001 ) ? 0.000001 : p1;

        // Stick surface humidity (g/g)
        m_hf = p1 / ps1;
        m_hf = ( m_hf > Hfs ) ? Hfs : m_hf;
        // Stick equilibrium moisture content (g/g). */
        double hf_log = -log( 1. - m_hf );
        m_sem = c1 * pow( hf_log, c2 );
		//m_sem = eqmc(ta, ha) / 100.0;
		//m_sem /= 2.0;
        //if(name() == "Ten Hour") std::cout << p1 << " " << ps1 << " " << m_hf << " " << c1 << " " << c2 << " " << hf_log << " " << m_sem << endl;
        //----------------------------------------------------------------------
        // Stick surface moisture content
        //----------------------------------------------------------------------

        // Initialize state for this m_mdt
        m_state = DFM_State_None;
        // Start with no water film contribution
        m_wfilm = 0.;
        // Factor related to rate of evaporation or condensation ((g/g)/h)
        double aml = 0.0;
        // Mass transfer biot number (dl)
        double bi  = 0.0;
        // Previous and new value of m_w[0] (g/g) and m_s[0]
        double s_new = m_s[0];
        double w_new = m_w[0];
        double w_old = m_w[0];

//......1: If it is RAINING (Rainfall for this timestep is greater than 0):
        if ( m_ra1 > 0.0 )
        {
//..........1a: If this is a RAINSTORM:
            if ( m_allowRainstorm && m_pptrate >= m_stv )
            {
                m_state = DFM_State_Rainstorm;
                m_wfilm = m_wfilmk;
                w_new   = m_wmx;
            }
//..........1b: Else this is RAINFALL:
            else
            {
                // If m_allowRainfall2 is FALSE, this section is always used
                // This is the first hour of rainfall after a period of no rain
                if ( m_rdur < 1.0 || ! m_allowRainfall2 )
                {
                    m_state = DFM_State_Rainfall1;
                    w_new = w_old + rai0;
                }
                // This will only apply if m_allowRainfall2 is TRUE and m_rdur >=1
                // This is the subsequent
                else
                {

                    m_state = DFM_State_Rainfall2;
                    w_new = w_old + rai1;
                }
            }
            m_wfilm = m_wfilmk; //
            s_new  = ( w_new - m_wsa ) / wdiff;
            m_t[0] = tfd;
            m_hf   = Hfs;
        }
//......2: Else it is not raining:
        else
        {
//.........2a: If moisture content exceeds the fiber saturation point:
            if ( w_old > m_wsa )
            {
                p1   = ps1;
                m_hf = Hfs;

                // Factor related to evaporation or condensation rate ((g/g)/h)
                aml = m_amlf * (ps1 - psd) / bp;

                if ( m_t[0] <= tdp && p1 > psd )
                {
                    aml = 0.;
                }
                w_new = w_old - aml * m_mdt_2;
                if ( aml > 0. )
                {
                    w_new -= ( m_mdt * m_capf / gnu );
                }
                w_new = ( w_new > m_wmx ) ? m_wmx : w_new;
                s_new = ( w_new - m_wsa ) / wdiff;

//..............2a1: if moisture content is rising: CONDENSATION
                if ( w_new > w_old )
                {
                    m_state = DFM_State_Condensation1;
                }
//..............2a2: else if moisture content is steady: STAGNATION
                else if ( w_new == w_old )
                {
                    m_state = DFM_State_Stagnation;
                }
//..............2a3: else if moisture content is falling: EVAPORATION
                else if ( w_new < w_old )
                {
                    m_state = DFM_State_Evaporation;
                }
            }
//..........2b: else if fuel temperature is less than dewpoint: CONDENSATION
            else if ( m_t[0] <= tdp )
            {
                m_state = DFM_State_Condensation2;
                // Factor related to evaporation or condensation rate ((g/g)/h)
                aml = ( p1 > psd ) ? 0.0 : m_amlf * (p1 - psd) / bp;
                w_new = w_old - aml * m_mdt_2;
                s_new = ( w_new - m_wsa ) / wdiff;
            }
//..........2c: else surface moisture content less than fiber saturation point
//              and stick temperature greater than dewpoint ...
            else
            {
//..............2c1: if surface moisture greater than equilibrium: DESORPTION
                if ( w_old >= m_sem )
                {
                    m_state = DFM_State_Desorption;
                    bi = m_stcd * m_dx / m_d[0];
                }
//..............2c2: else surface moisture less than equilibrium: ADSORPTION
                else
                {
                    m_state = DFM_State_Adsorption;
                    bi = m_stca * m_dx / m_d[0];
                }
                w_new = ( m_w[1] + bi * m_sem ) / ( 1. + bi );
                s_new = 0.;
            }
        }   // end of not raining

        // Store the new surface moisture and saturation
        m_w[0] = ( w_new > m_wmx ) ? m_wmx : w_new;
        m_s[0] = ( s_new < 0. ) ? 0.0 : s_new;
        tstate[m_state]++;

#ifdef DEBUG
fprintf( stdout,
"%03d: ta=%7.4f ha=%6.4f sv=%6.2f rc=%f wold=%f rai0=%f rai1=%f state=%s t0=%f w0=%f\n",
nstep, ta, ha, sv, m_rc1, w_old, rai0, rai1, stateName(), m_t[0], m_w[0] );
#endif
        //----------------------------------------------------------------------
        // Compute interior nodal moisture content values.
        //----------------------------------------------------------------------

/* Declaration of vectors moved outside of loop for performance reasons
SB 1/6/2007
        // Nodal moisture contents at the previous m_mdt (g/g)
        vector<double> wold( m_nodes );
        // Nodal temperatures at the previous m_mdt (oC)
        vector<double> told( m_nodes );
        // Nodal fiber saturation points at the previous m_mdt (g/g)
        vector<double> sold( m_nodes );
        // Used to redistribute fuel temperature
        vector<double> v( m_nodes );
        // Used to redistribute moisture content
        vector<double> o( m_nodes );
        // Free water transport coefficient (cm2/h)
        vector<double> g( m_nodes );
*/
        for ( int i=0; i<m_nodes; i++ )
        {
            m_Twold[i] = m_w[i];
            m_Tsold[i] = m_s[i];
            m_Ttold[i] = m_t[i];
            m_Tv[i] = Thdiff * m_x[i];
            m_To[i] = m_d[i] * m_x[i];
        }

        // Propagate the moisture content changes
        if ( m_state != DFM_State_Stagnation )
        {
            for ( int i=0; i<m_nodes; i++ )
            {
                m_Tg[i] = 0.0;
                double svp = ( m_w[i] - m_wsa ) / wdiff;
                if ( svp >= Sir && svp <= Scr )
                {
                    // Permeability of stick when nonsaturated (cm2)
                    double ak = Aks * ( 2. * sqrt( svp / Scr ) - 1. );

                    // Free water transport coefficient (cm2/h)
                    m_Tg[i] = ( ak / ( gnu * wdiff ) )
                         * m_x[i] * m_vf
                         * pow( ( Scr / svp ), 1.5 ) ;
                }
            }

            // Propagate the fiber saturation moisture content changes
            for ( int i=1; i<m_nodes-1; i++ )
            {
                double ae = m_Tg[i+1] / m_dx;
                double aw = m_Tg[i-1] / m_dx;
                double ar = m_x[i] * m_dx / m_mdt;
                double ap = ae + aw + ar;
                m_s[i] = ( ae * m_Tsold[i+1] + aw * m_Tsold[i-1] + ar * m_Tsold[i] ) / ap;
                if ( m_randseed )
                {
                    double rn = uniformRandom( -.0001, 0.0001 );
                    m_s[i] += rn;
                }
                //constrain to Sir instead of 1.0 as otherwise once we get in here we never leave saturation (continuousLiquid stays always true)
                //this has only been found to occur when m_wmx is > 0.45 via call to setMaxLocalMoisture() for 1 hour sticks
                m_s[i] = ( m_s[i] > Sir ) ? Sir : m_s[i];
                m_s[i] = ( m_s[i] < 0. ) ? 0. : m_s[i];
            }
            m_s[ m_nodes-1 ] = m_s[ m_nodes-2 ];

            // Check if m_s[] is less than Sir (limit of continuous liquid
            // columns) at ANY stick node.
            bool continuousLiquid = true;
            for ( int i=1; i<m_nodes-1; i++ )
            {
                if ( m_s[i] < Sir )
                {
                    continuousLiquid = false;
                    break;
                }
            }

            // If all nodes have continuous liquid columns (s >= Sir) ...
            // This never happens for the 1-h or 10-h test data!
            if ( continuousLiquid)
            {
                for ( int i=1; i<m_nodes-1; i++ )
                {
                    m_w[i] = m_wsa + m_s[i] * wdiff;
                    if ( m_pertubateColumn )
                    {
                        double rn = uniformRandom( -.0001, 0.0001 );
                        m_w[i] += rn;
                    }
                    m_w[i] = ( m_w[i] > m_wmx ) ? m_wmx : m_w[i];
                    m_w[i] = ( m_w[i] < 0.0 ) ? 0.0 : m_w[i];
                }
            }
            // ... else at least one node has s < Sir.
            else
            {
                // Propagate the moisture content changes
                for ( int i=1; i<m_nodes-1; i++ )
                {
                    double ae = m_To[i+1] / m_dx;
                    double aw = m_To[i-1] / m_dx;
                    double ar = m_x[i] * m_dx / m_mdt;
                    double ap = ae + aw + ar;
                    m_w[i] = ( ae * m_Twold[i+1] + aw * m_Twold[i-1] + ar * m_Twold[i] )
                           / ap;
                    if ( m_randseed )
                    {
                        double rn = uniformRandom( -.0001, 0.0001 );
                        m_w[i] += rn;
                    }
                    m_w[i] = ( m_w[i] > m_wmx ) ? m_wmx : m_w[i];
                    m_w[i] = ( m_w[i] < 0.0 ) ? 0.0 : m_w[i];
                }
            }
            m_w[ m_nodes-1 ] = m_w[ m_nodes-2 ];
        }

        // Propagate the fuel temperature changes
        for ( int i=1; i<m_nodes-1; i++ )
        {
            double ae = m_Tv[i+1] / m_dx;
            double aw = m_Tv[i-1] / m_dx;
            double ar = m_x[i] * m_dx / m_mdt;
            double ap = ae + aw + ar;
            m_t[i] = ( ae * m_Ttold[i+1] + aw * m_Ttold[i-1] + ar * m_Ttold[i] ) / ap;
            if ( m_randseed )
            {
                double rn = uniformRandom( -.0001, 0.0001 );
                m_t[i] += rn;
            }
            m_t[i] = ( m_t[i] > 71. ) ? 71. : m_t[i];
        }
        m_t[ m_nodes-1 ] = m_t[ m_nodes-2 ];

        // Update the moisture diffusivity if within less than half a time step
        if ( ( ddtNext - tt ) < ( 0.5 * m_mdt ) )
        {
            diffusivity( bp );
            ddtNext += m_ddt;
        }
    }   // Next moisture time step

    // Store prevailing state
    m_state = DFM_State_None;
    int max = tstate[0];
    for ( int i=1; i<DFM_States; i++ )
    {
        if ( tstate[i] > max )
        {
            m_state = (DFM_State) i;
            max = tstate[i];
        }
    }
    return( true );
}
//------------------------------------------------------------------------------
/*! \brief EQMC equation from NFDRS 1978

\return Calculated EQMC
*/


double DeadFuelMoisture::eqmc(double fTemp, double fRH)
{
	fRH = fRH * 100.;
	fTemp = (fTemp * (9 / 5)) + 32.;
	double eqmc_ans = 0.0;

	if (fRH > 50)
		return (21.0606 + 0.005565 * pow((double)fRH, 2) - 0.00035 * fRH * fTemp - 0.483199 * fRH);
	if ((fRH > 10) && (fRH < 51))
		return (2.22749 + 0.160107 * fRH - 0.014784 * fTemp);

	return (0.03229 + 0.281073 * fRH - 0.000578 * fRH * fTemp) / 100.;
}

//------------------------------------------------------------------------------
/*! \brief Access to the current number of observation updates.

    \return The current number of observation updates.
 */

long DeadFuelMoisture::updates( void ) const
{
    return( m_updates );
}

//------------------------------------------------------------------------------
/*! \brief Access to the stick's current water film contribution to the
    moisture content.

    \return Stick's current water film contribution to the moisture content
    (g/g).
 */

double DeadFuelMoisture::waterFilmContribution( void ) const
{
    return( m_wfilmk );
}

//------------------------------------------------------------------------------
/*! \brief Sets everything to zero.
 */

void DeadFuelMoisture::zero( void )
{
    //m_semTime.set( 0, 0, 0, 0, 0, 0, 0 );
    m_Jday = 0.0;
    m_density   = 0.0;
    m_dSteps    = 0;
    m_hc        = 0.0;
    m_length    = 0.0;
    m_name      = "";
    m_nodes     = 0;
    m_radius    = 0.0;
    m_rai0      = 0.0;
    m_rai1      = 0.0;
    m_stca      = 0.0;
    m_stcd      = 0.0;
    m_mSteps    = 0;
    m_stv       = 0.0;
    m_wfilmk    = 0.0;
    m_wmx       = 0.0;
    m_dx        = 0.0;
    m_wmax      = 0.0;
    m_x.resize(0);
    m_v.resize(0);
    m_amlf      = 0.0;
    m_capf      = 0.0;
    m_hwf       = 0.0;
    m_dx_2      = 0.0;
    m_vf        = 0.0;
    m_bp0       = 0.0;
    m_ha0       = 0.0;
    m_rc0       = 0.0;
    m_sv0       = 0.0;
    m_ta0       = 0.0;
    m_init      = false;
    m_bp1       = 0.0;
    m_et        = 0.0;
    m_ha1       = 0.0;
    m_rc1       = 0.0;
    m_sv1       = 0.0;
    m_ta1       = 0.0;
    m_ddt       = 0.0;
    m_mdt       = 0.0;
    m_mdt_2     = 0.0;
    m_pptrate   = 0.0;
    m_ra0       = 0.0;
    m_ra1       = 0.0;
    m_rdur      = 0.0;
    m_sf        = 0.0;
    m_hf        = 0.0;
    m_wsa       = 0.0;
    m_sem       = 0.0;
    m_wfilm     = 0.0;
    m_elapsed   = 0.0;
    m_t.resize(0);
    m_s.resize(0);
    m_d.resize(0);
    m_w.resize(0);
    m_updates   = 0;
    m_state     = DFM_State_None;
    m_randseed  = 0;
    return;
}

//------------------------------------------------------------------------------
/*! \brief DeadFuelMoisture stream insertion operator.

    The DeadFuelMoisture output format a series of "<name> <value>" pairs.

    \param[in] output   Reference to the output stream.
    \param[in] r        Reference to the DeadFuelMoisture instance.

    \relates DeadFuelMoisture

    \return Reference to the output stream.
 */

ostream& operator<<( ostream& output, const DeadFuelMoisture& r )
{
    std::vector<double>::const_iterator it;
    output << "m_JDay "  << r.m_Jday << "\n"
        << "m_density "     << r.m_density << "\n"
        << "m_dSteps "      << r.m_dSteps << "\n"
        << "m_hc "          << r.m_hc << "\n"
        << "m_length "      << r.m_length << "\n"
        << "m_name "        << r.m_name << "\n"
        << "m_nodes "       << r.m_nodes << "\n"
        << "m_radius "      << r.m_radius << "\n"
        << "m_rai0 "        << r.m_rai0 << "\n"
        << "m_rai1 "        << r.m_rai1 << "\n"
        << "m_stca "        << r.m_stca << "\n"
        << "m_stcd "        << r.m_stcd << "\n"
        << "m_mSteps "      << r.m_mSteps << "\n"
        << "m_stv "         << r.m_stv << "\n"
        << "m_wfilmk "      << r.m_wfilmk << "\n"
        << "m_wmx "         << r.m_wmx << "\n"
        << "m_dx "          << r.m_dx << "\n"
        << "m_wmax "        << r.m_wmax << "\n"
        << "m_x (" << r.m_x.size() << ") ";
    for ( it = r.m_x.begin(); it != r.m_x.end(); it++ )
    {
        output << " " << *it ;
    }
    output << "\nm_v (" << r.m_v.size() << ") " ;
    for ( it = r.m_v.begin(); it != r.m_v.end(); it++ )
    {
        output << " " << *it ;
    }
    output<< "\nm_amlf " << r.m_amlf << "\n"
        << "m_capf "    << r.m_capf << "\n"
        << "m_hwf "     << r.m_hwf << "\n"
        << "m_dx_2 "    << r.m_dx_2 << "\n"
        << "m_vf "      << r.m_vf << "\n"
        << "m_bp0 "     << r.m_bp0 << "\n"
        << "m_ha0 "     << r.m_ha0 << "\n"
        << "m_rc0 "     << r.m_rc0 << "\n"
        << "m_sv0 "     << r.m_sv0 << "\n"
        << "m_ta0 "     << r.m_ta0 << "\n"
        << "m_init "    << r.m_init << "\n"
        << "m_bp1 "     << r.m_bp1 << "\n"
        << "m_et "      << r.m_et << "\n"
        << "m_ha1 "     << r.m_ha1 << "\n"
        << "m_rc1 "     << r.m_rc1 << "\n"
        << "m_sv1 "     << r.m_sv1 << "\n"
        << "m_ta1 "     << r.m_ta1 << "\n"
        << "m_ddt "     << r.m_ddt << "\n"
        << "m_mdt "     << r.m_mdt << "\n"
        << "m_mdt_2 "   << r.m_mdt_2 << "\n"
        << "m_pptrate " << r.m_pptrate << "\n"
        << "m_ra0 "     << r.m_ra0 << "\n"
        << "m_ra1 "     << r.m_ra1 << "\n"
        << "m_rdur "    << r.m_rdur << "\n"
        << "m_sf "      << r.m_sf << "\n"
        << "m_hf "      << r.m_hf << "\n"
        << "m_wsa "     << r.m_wsa << "\n"
        << "m_sem "     << r.m_sem << "\n"
        << "m_wfilm "   << r.m_wfilm << "\n"
        << "m_elapsed " << r.m_elapsed << "\n"
        << "m_t " << r.m_t.size() << " ";
    for ( it = r.m_t.begin(); it != r.m_t.end(); it++ )
    {
        output << " " << *it ;
    }
    output << "\nm_s " << r.m_s.size() << " ";
    for ( it = r.m_s.begin(); it != r.m_s.end(); it++ )
    {
        output << " " << *it ;
    }
    output << "\nm_d " << r.m_d.size() << " ";
    for ( it = r.m_d.begin(); it != r.m_d.end(); it++ )
    {
        output << " " << *it ;
    }
    output << "\nm_w " << r.m_w.size() << " ";
    for ( it = r.m_w.begin(); it != r.m_w.end(); it++ )
    {
        output << " " << *it ;
    }
    output << "\nm_updates " << r.m_updates << "\n"
        << "m_state "       << r.m_state << "\n"
        << "m_randseed "    << r.m_randseed << "\n" ;
    return( output );
}

//------------------------------------------------------------------------------
/*! \brief DeadFuelMoisture stream extraction operator.

    The DeadFuelMoisture input format must be "yyyy/mm/dd hh:mm:ss.sss".

    \param[in] input    Reference to the input stream.
    \param[in] r        Reference to the DeadFuelMoisture instance.

    \relates DeadFuelMoisture

    \todo Test this!

    \return Reference to the input stream.
 */

istream& operator>>( istream& input, DeadFuelMoisture& r )
{
    string vname;
    int i, n;
    input >> vname >> r.m_Jday;
    input >> vname >> r.m_density;
    input >> vname >> r.m_dSteps;
    input >> vname >> r.m_hc;
    input >> vname >> r.m_length;
    input >> vname >> r.m_name;
    input >> vname >> r.m_nodes;
    input >> vname >> r.m_radius;
    input >> vname >> r.m_rai0;
    input >> vname >> r.m_rai1;
    input >> vname >> r.m_stca;
    input >> vname >> r.m_stcd;
    input >> vname >> r.m_mSteps;
    input >> vname >> r.m_stv;
    input >> vname >> r.m_wfilmk;
    input >> vname >> r.m_wmx;
    input >> vname >> r.m_dx;
    input >> vname >> r.m_wmax;
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_x[i];
    }
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_v[i];
    }
    input >> vname >> r.m_amlf;
    input >> vname >> r.m_capf;
    input >> vname >> r.m_hwf;
    input >> vname >> r.m_dx_2;
    input >> vname >> r.m_vf;
    input >> vname >> r.m_bp0;
    input >> vname >> r.m_ha0;
    input >> vname >> r.m_rc0;
    input >> vname >> r.m_sv0;
    input >> vname >> r.m_ta0;
    input >> vname >> r.m_init;
    input >> vname >> r.m_bp1;
    input >> vname >> r.m_et;
    input >> vname >> r.m_ha1;
    input >> vname >> r.m_rc1;
    input >> vname >> r.m_sv1;
    input >> vname >> r.m_ta1;
    input >> vname >> r.m_ddt;
    input >> vname >> r.m_mdt;
    input >> vname >> r.m_mdt_2;
    input >> vname >> r.m_pptrate;
    input >> vname >> r.m_ra0;
    input >> vname >> r.m_ra1;
    input >> vname >> r.m_rdur;
    input >> vname >> r.m_sf;
    input >> vname >> r.m_hf;
    input >> vname >> r.m_wsa;
    input >> vname >> r.m_sem;
    input >> vname >> r.m_wfilm;
    input >> vname >> r.m_elapsed;
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_t[i];
    }
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_s[i];
    }
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_d[i];
    }
    input >> vname >> n;
    for ( i=0; i<n; i++ )
    {
        input >> r.m_w[i];
    }
    input >> vname >> r.m_updates;
    input >> vname >> n;
    r.m_state = n;
    input >> vname >> r.m_randseed;
    return( input );
}

DFMCalcState DeadFuelMoisture::GetState()
{
	DFMCalcState ret;
	ret.m_JDay = m_Jday;
	ret.m_obstime = obstime;
	ret.m_Year = m_Year;
	ret.m_Month = m_Month;
	ret.m_Day = m_Day;
	ret.m_Hour = m_Hour;
	ret.m_Min = m_Min;
	ret.m_Sec = m_Sec;
	ret.m_bp1 = m_bp1;
	ret.m_et = m_et;
	ret.m_ha1 = m_ha1;
	ret.m_rc1 = m_rc1;
	ret.m_sv1 = m_sv1;
	ret.m_ta1 = m_ta1;
	ret.m_hf = m_hf;
	ret.m_wsa = m_wsa;
	ret.m_rdur = m_rdur;
	ret.m_ra1 = m_ra1;
	ret.m_nodes = m_nodes;
	for (int i = 0; i < m_nodes; i++)
	{
		//float tVal;
		//tVal = m_t[i];
		ret.m_t.push_back((float)m_t[i]);
		//tVal = m_s[i];
		ret.m_s.push_back((float)m_s[i]);
		//tVal = m_d[i];
		ret.m_d.push_back((float)m_d[i]);
		//tVal = m_w[i];
		ret.m_w.push_back((float)m_w[i]);
	}
	return ret;
}

bool DeadFuelMoisture::SetState(DFMCalcState state)
{
	m_Jday = state.m_JDay;
	obstime = state.m_obstime;
	m_Year = state.m_Year;
	m_Month = state.m_Month;
	m_Day = state.m_Day;
	m_Hour = state.m_Hour;
	m_Min = state.m_Min;
	m_Sec = state.m_Sec;
	m_bp1 = state.m_bp1;
	m_et = state.m_et;
	m_ha1 = state.m_ha1;
	m_rc1 = state.m_rc1;
	m_sv1 = state.m_sv1;
	m_ta1 = state.m_ta1;
	m_hf = state.m_hf;
	m_wsa = state.m_wsa;
	m_rdur = state.m_rdur;
	m_ra1 = state.m_ra1;
	m_nodes = state.m_nodes;
	m_t.clear();
	m_s.clear();
	m_d.clear();
	m_w.clear();
	for (int i = 0; i < m_nodes; i++)
	{
		//float tVal;
		//tVal = state.m_t[i];
		m_t.push_back(state.m_t[i]);
		//tVal = m_s[i];
		m_s.push_back(state.m_s[i]);
		//tVal = m_d[i];
		m_d.push_back(state.m_d[i]);
		//tVal = m_w[i];
		m_w.push_back(state.m_w[i]);
	}
	diffusivity(m_bp1);
	return true;
}




//------------------------------------------------------------------------------
//  End of DeadFuelMoisture.cpp
//------------------------------------------------------------------------------

