This directory contains an interface file ```nfdrs.i``` for generating SWIG bindings for NFDRS4

**How to compile the SWIG module for Python:**

Swig ver 4 or newer is needed. To generate the bindings and compile the module for Python, execute the following from within this directory in the NFDRS source tree:
```/bin/bash
swig -python -py3 -c++ nfdrs4.i
g++ -fPIC -I /usr/include/python3.10  -I ../lib/NFDRS4/include/ \
   -I ../lib/time64/include/ -I ../lib/utctime/include/ \
   -c ../lib/NFDRS4/src/deadfuelmoisture.cpp \
      ../lib/NFDRS4/src/livefuelmoisture.cpp \
      ../lib/NFDRS4/src/dfmcalcstate.cpp \
      ../lib/NFDRS4/src/lfmcalcstate.cpp \
      ../lib/NFDRS4/src/nfdrs4calcstate.cpp \
      ../lib/NFDRS4/src/nfdrs4.cpp  \
      ../lib/utctime/src/utctime.cpp \
      ../lib/time64/src/time64.c nfdrs_wrap.cxx
g++ -shared *.o -o _nfdrs.so -lgomp
```

Notes: The include path to Python.h might need to be changed. Other than that, all other includes should be from the NFDRS source tree.

**Example usage to test functions:**

```python
python3
>>> import nfdrs
>>> nf= nfdrs.DeadFuelMoisture.createDeadFuelMoisture1()
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
