cmake_minimum_required (VERSION 3.13)

project(utctime VERSION 1.0 LANGUAGES CXX)

IF(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS 
	${HEADER_DIR}/utctime.h)
add_library(${PROJECT_NAME} STATIC
	${HEADERS}
	src/utctime.cpp)

target_include_directories(${PROJECT_NAME}   PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (${PROJECT_NAME} PUBLIC time64)

set(include_dest "include")
install(FILES ${HEADERS} DESTINATION "${include_dest}")