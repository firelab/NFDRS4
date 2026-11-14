# nfdrs4 (Python)

Python bindings for the US National Fire Danger Rating System version 4 (NFDRS4):
the Nelson dead fuel moisture model, the GSI-based live fuel moisture model, and
the NFDRS4 index calculator. The bindings are generated with SWIG from the C++
sources in this repository.

Model documentation: Jolly et al. (2024), *Modernizing the US National Fire Danger
Rating System (version 4)*, Environmental Modelling & Software,
https://doi.org/10.1016/j.envsoft.2024.106181

## Install

From GitHub:

```bash
pip install git+https://github.com/firelab/NFDRS4
```

From a local clone:

```bash
pip install .            # or, for development: pip install -e .
```

The only thing you need on your machine is a C++ compiler (gcc/clang on
Linux/macOS, Visual Studio Build Tools on Windows). SWIG and CMake are
downloaded from PyPI automatically during the build. You don't need conda.

To compile with the OpenMP sections in `nfdrs4.cpp` enabled:

```bash
pip install . -C cmake.define.NFDRS4_OPENMP=ON
```

## Use

```python
import nfdrs4

dfm = nfdrs4.DeadFuelMoisture.createDeadFuelMoisture1()
dfm.update(1, 1, 1, 1, 1)
dfm.medianRadialMoisture()   # 0.5606...

calc = nfdrs4.NFDRS4()
```

## Build wheels

```bash
python -m build              # sdist + wheel for the current Python
```

`.github/workflows/wheels.yml` uses [cibuildwheel](https://cibuildwheel.pypa.io)
to build wheels for Linux, macOS and Windows. Pushing a `v*` tag publishes them to
PyPI (this needs trusted publishing set up for the repo on pypi.org).

## Test

```bash
pip install .[test]
pytest
```
