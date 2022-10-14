cmake_minimum_required (VERSION 3.13)

project(NFDRS4 VERSION 1.0 LANGUAGES CXX)

IF(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(TOP_LEVEL_HEADERS
        ${HEADER_DIR}/nfdrs4.h
        )
set(INTERNAL_HEADERS
	${HEADER_DIR}/deadfuelmoisture.h
	${HEADER_DIR}/dfmcalcstate.h
	${HEADER_DIR}/lfmcalcstate.h
	${HEADER_DIR}/livefuelmoisture.h
	${HEADER_DIR}/nfdrs4calcstate.h
	${HEADER_DIR}/nfdrs4statesizes.h
)

set(HEADERS 
	${TOP_LEVEL_HEADERS}
	${INTERNAL_HEADERS}
)

add_library(${PROJECT_NAME} STATIC
	${HEADERS}
	src/deadfuelmoisture.cpp
	src/dfmcalcstate.cpp
	src/lfmcalcstate.cpp
	src/livefuelmoisture.cpp
	src/nfdrs4.cpp
	src/nfdrs4calcstate.cpp
)

target_include_directories(${PROJECT_NAME}   PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries (${PROJECT_NAME} PUBLIC utctime)

set(include_dest "include")
install(FILES ${HEADERS} DESTINATION "${include_dest}")