# Builds the nfdrs4 Python extension. Driven by scikit-build-core via the
# top-level pyproject.toml:  pip install .
cmake_minimum_required(VERSION 3.18)
project(nfdrs4_python LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)   # static libs end up in a shared module

set(NFDRS4_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)

# Reuse the upstream library targets (NFDRS4 -> utctime -> time64)
add_subdirectory(${NFDRS4_ROOT}/lib/time64  ${CMAKE_BINARY_DIR}/time64)
add_subdirectory(${NFDRS4_ROOT}/lib/utctime ${CMAKE_BINARY_DIR}/utctime)
add_subdirectory(${NFDRS4_ROOT}/lib/NFDRS4  ${CMAKE_BINARY_DIR}/NFDRS4)

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(SWIG 4.0 REQUIRED COMPONENTS python)
include(UseSWIG)

# Use the upstream interface file unchanged. Its #include lines are written
# relative to swig/ ("../lib/..."), so swig/ goes on both include paths.
set(NFDRS4_I ${NFDRS4_ROOT}/swig/nfdrs4.i)
set_property(SOURCE ${NFDRS4_I} PROPERTY CPLUSPLUS ON)
set_property(SOURCE ${NFDRS4_I} PROPERTY INCLUDE_DIRECTORIES ${NFDRS4_ROOT}/swig)

swig_add_library(nfdrs4_py
    TYPE MODULE
    LANGUAGE python
    OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/py
    OUTFILE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cxx
    SOURCES ${NFDRS4_I})

# Module file must be _nfdrs4.<ext> to match "%module nfdrs4" in the .i file
set_target_properties(nfdrs4_py PROPERTIES PREFIX "" OUTPUT_NAME _nfdrs4)
target_include_directories(nfdrs4_py PRIVATE ${NFDRS4_ROOT}/swig)
target_link_libraries(nfdrs4_py PRIVATE NFDRS4 Python::Module)
if(MSVC)
    target_compile_options(nfdrs4_py PRIVATE /bigobj)
endif()

# Optional: honour the "#pragma omp" sections in nfdrs4.cpp.
# Off by default so wheels don't need an OpenMP runtime.
option(NFDRS4_OPENMP "Build with OpenMP" OFF)
if(NFDRS4_OPENMP)
    find_package(OpenMP REQUIRED COMPONENTS CXX)
    target_link_libraries(NFDRS4 PUBLIC OpenMP::OpenMP_CXX)
endif()

# Install the compiled module and the SWIG-generated proxy into the package.
# Uses its own component so the upstream header install() rules in lib/ are
# not dumped into site-packages (see install.components in pyproject.toml).
install(TARGETS nfdrs4_py
        LIBRARY DESTINATION nfdrs4 COMPONENT python
        RUNTIME DESTINATION nfdrs4 COMPONENT python)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/py/nfdrs4.py DESTINATION nfdrs4 COMPONENT python)
