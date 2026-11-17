import math

import nfdrs4


def test_version():
    assert nfdrs4.__version__ not in ("", "unknown")


def test_dead_fuel_moisture_readme_example():
    # Values from swig/Readme.md
    dfm = nfdrs4.DeadFuelMoisture.createDeadFuelMoisture1()
    assert dfm.update(1, 1, 1, 1, 1)
    assert math.isclose(dfm.medianRadialMoisture(), 0.5606049792805294, rel_tol=1e-9)
    dfm.setAdsorptionRate(0.4)
    assert dfm.diffusivitySteps() == 25
    assert dfm.deriveStickNodes(0.3) == 11


def test_core_classes_exposed():
    for name in ("NFDRS4", "DeadFuelMoisture", "LiveFuelMoisture",
                 "NFDRS4State", "DFMCalcState", "LFMCalcState"):
        assert hasattr(nfdrs4, name), name
    nfdrs4.NFDRS4()
