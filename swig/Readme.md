This directory contains an interface file ```nfdrs.i``` for generating SWIG bindings for NFDRS4

**How to compile the SWIG module for Python:**

Swig ver 4 or newer is needed. To generate the bindings and compile the module for Python, execute the following from within this directory in the NFDRS source tree:
```/bin/bash
swig -python -py3 -c++ nfdrs4.i
g++ -fPIC -I ~/anaconda3/include/python3.12/ -I ../lib/NFDRS4/include/
      -I ../lib/time64/include/ -I ../lib/utctime/include/
      -c ../lib/NFDRS4/src/deadfuelmoisture.cpp  ../lib/NFDRS4/src/livefuelmoisture.cpp ../lib/NFDRS4/src/dfmcalcstate.cpp
      ../lib/NFDRS4/src/lfmcalcstate.cpp       ../lib/NFDRS4/src/nfdrs4calcstate.cpp       ../lib/NFDRS4/src/nfdrs4.cpp
      ../lib/utctime/src/utctime.cpp ../app/NFDRS4_cli/src/CNFDRSParams.cpp      ../lib/time64/src/time64.c nfdrs4_wrap.cxx
g++ -shared *.o -o _nfdrs4.so -lgomp
```

Notes: The include path to Python.h might need to be changed. Other than that, all other includes should be from the NFDRS source tree.


**How to compile and build SWIG module for Python using Anaconda:**
***This example is for Python 3.13 but can be adapted to any Python version***

To build the Python library, you'll need to download the NFDRS4 repo and change into the 'swig' directory of repo root. 

```
conda create --name nfdrs4 swig gxx m2-base python=3.13
conda activate nfdrs4
swig -python -c++ nfdrs4.i  
g++ -I %CONDA_PREFIX%\include -I ../lib/NFDRS4/include/ -I ../lib/time64/include/ -I ../lib/utctime/include/ -c ../lib/NFDRS4/src/deadfuelmoisture.cpp  ../lib/NFDRS4/src/livefuelmoisture.cpp ../lib/NFDRS4/src/dfmcalcstate.cpp ../lib/NFDRS4/src/lfmcalcstate.cpp ../lib/NFDRS4/src/nfdrs4calcstate.cpp ../lib/NFDRS4/src/nfdrs4.cpp ../lib/utctime/src/utctime.cpp ../app/NFDRS4_cli/src/CNFDRSParams.cpp ../lib/time64/src/time64.c nfdrs4_wrap.cxx
g++ -shared *.o -o _nfdrs4.pyd -lgomp -L %CONDA_PREFIX%\libs -l python313
```
Note: %CONDA_PREFIX% may be different if using a Powershell prompt. If you are having trouble, trying replacing %CONDA_PREFIX% with $env:CONDA_PREFIX.

**Example usage to test functions:**

```python
python
>>> import nfdrs4
>>> nf= nfdrs4.DeadFuelMoisture.createDeadFuelMoisture1()
>>> nf.update(1,1,1,1,1)
True
>>> nf.medianRadialMoisture()
0.5606049792805294
>>> nf.setAdsorptionRate(0.4)
>>> nf.diffusivitySteps()
25
>>> nf.deriveStickNodes(0.3)
11
```
