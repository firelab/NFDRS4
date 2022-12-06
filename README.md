
# NFDRS4 - National Fire Danger Rating System 4.0

## Multiplatform source for NFDRS4 static library
Also produces FireWxConverter, which converts FW13 fire weather data files to FW21 fire weather data files
NFDRS4_cli produces live and dead fuel moistures as well as NFDRS indexes from FW21 fire weather data files.

### Dependencies:

*CMAKE NFDRS4* - requires CMAKE version 3.8 or higher

*Config4cpp* - see http://www.config4star.org/
 Config4cpp is used by NFDRS4 for defining configuration files for NFDRS4_cli and NFDRS4 initialization (station) parameters. 
 Complete source for config4cpp is included in the 'extern' directory, and must be built via makefiles to produce a static config4cpp library which is neccessary to build NFDRS4_cli executable.

*utctime* - see http://paulgriffiths.github.io/utctime/documentation/index.html
 UTCTime class is used for handling time in NFDRS4. Complete source is in the lib/utctime directory

License - NFDRS4 is public domain software, still under development at this time.

Building NFDRS4_cli
 Run CMAKE and provide entries for CONFIG4CPP_DIR (directory containing config4cpp include files) and CONFIG4CPP_LIB (directory containing config4cpp.lib)
 Rerun CMAKE and run make


## Building in MS Windows
Building for MS Windows has been tested with MS Visual Studio 2022
*Steps*<br>
*Build config4cpp*<br>
Build config4cpp.lib is easiest accomplished by use of the x64 Native Tools Command Prompt for VS 2022.<br>
Open the x64 Native Tools Command Prompt for VS 2022, navigate to the NFDRS4/extern/config4cpp directory and enter: ```nmake -f Makefile.win all```<br> 
This should produce config4cpp.lib static library in NFDRS4/extern/config4cpp/lib<br><br>
Navigate back to the root NFDRS4 directory<br>
Run ```cmake -G "NMake Makefiles" .```<br>
*If you haven't already done so, edit the entries for CONFIG4CPP_DIR and CONFIG4CPP_LIB in CMakeCache.txt*<br>
Example:<br>
//Path to a file.<br>
CONFIG4CPP_DIR:PATH=S:/src/NFDRS4/extern/config4cpp/include<br>

//Path to a library.<br>
CONFIG4CPP_LIB:FILEPATH=S:/src/NFDRS4/extern/config4cpp/lib/config4cpp.lib*<br>

Rerun ```cmake -G "NMake Makefiles" .```, there should be no errors<br>
Run ```nmake```<br><br>

## Build NFDRS4 from Visual Studio 2022
In Visual Studio 2022, open the NFDRS4 folder and NFDRS4 will load as a CMake project<br>
Select Project - CMake Settings for NFDRS4<br>
Create a Configuration for x64-Release<br>
Save the settings, Cmake will run
Populate the entry in CMakeSettings.json for CONFIG4CPP_DIR 
	(should be <repo location>/NFDRS4/extern/config4cpp/include where <repo location> is the Drive and folder where the NFDRS4 repository is located)
	e.g. D:/Repos/NFDRS4/extern/config4cpp/include
Populate the entry in CmakeSettings.json for CONFIG4CPP_LIB
	(should be <repo location>/NFDRS4/extern/config4cpp/lib/config4cpp.lib)
	e.g. D:/Repos/NFDRS4/extern/config4cpp/lib/config4cpp.lib
Save CMakeSettings.json, CMake will run and there should be no errors

Select Build - Build All
Select Build - Install NFDRS4
	- this will create an install folder for X64-Release with necessary include and lib files to use NFDRS4 and fw21 with applications, as well as executables
	NFDRS4_cli.exe and FireWxConverter.exe

## Building NFDRS4 for Linux
After extracting the repository, navigate to the NFDRS4/external/config4cpp directory in a Terminal window, and run ```make```<br>
This will create config4cpp.a in NFDRS/external/lib

Navigate back to the NFDRS4 directory. run: ```cmake .```<br>
Fill in or add entries for CONFIG4CPP_DIR and CONFIG4CPP_LIB
e.g.
CONFIG4CPP_DIR:PATH=/home/<user>/src/NFDRS4/extern/config4cpp/include
CONFIG4CPP_LIB:FILEPATH=/home/<user>/src/NFDRS4/extern/config4cpp/lib/libconfig4cpp.a

Build the repository:
```run make```

Install the repository:
```run sudo make install```
