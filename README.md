NFDRS4 - National Fire Danger Rating System 4.0

Multiplatform source for NFDRS4 static library
Also produces FireWxConverter, which converts FW13 fire weather data files to FW21 fire weather data files
NFDRS4_cli produces live and dead fuel moistures as well as NFDRS indexes from FW21 fire weather data files.

Dependencies:

CMAKE
 NFDRS4 requires CMAKE version 3.8 or higher

Config4cpp - see http://www.config4star.org/
 Config4cpp is used by NFDRS4 for defining configuration files for NFDRS4_cli and NFDRS4 initialization (station) parameters. 
 Complete source for config4cpp is included in the 'extern' directory, and must be built via makefiles to produce config4cpp.lib which is neccessary to build NFDRS4_cli executable.

utctime - see http://paulgriffiths.github.io/utctime/documentation/index.html
 UTCTime class is used for handling time in NFDRS4. Complete source is in the lib/utctime directory


Building NFDRS4_cli
 Run CMAKE and provide entries for CONFIG4CPP_DIR (directory containing config4cpp include files) and CONFIG4CPP_LIB (directory containing config4cpp.lib)
 Rerun CMAKE and run make

License - NFDRS4 is public domain software, still under development at this time.

