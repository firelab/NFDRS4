cmake_minimum_required(VERSION 3.8)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(FireWxConverter)

IF(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

add_executable(${PROJECT_NAME} src/FireWxConverter.cpp)

target_link_libraries (${PROJECT_NAME} PUBLIC fw21 ${CONFIG4CPP_LIB})
