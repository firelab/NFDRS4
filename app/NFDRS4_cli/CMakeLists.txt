cmake_minimum_required(VERSION 3.13)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(NFDRS4_cli)

IF(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

find_library(CONFIG4CPP_LIB config4cpp.lib)
find_path(CONFIG4CPP_DIR Configuration.h
)
include_directories(${CONFIG4CPP_DIR})

set(CONFIG4CPP_HEADERS
		${CONFIG4CPP_DIR}/Configuration.h 
		${CONFIG4CPP_DIR}/ConfigurationException.h 
		${CONFIG4CPP_DIR}/namespace.h 
		${CONFIG4CPP_DIR}/SchemaType.h 
		${CONFIG4CPP_DIR}/SchemaValidator.h 
		${CONFIG4CPP_DIR}/StringBuffer.h 
		${CONFIG4CPP_DIR}/StringVector.h
)

add_executable(${PROJECT_NAME} src/CNFDRSParams.cpp src/NFDRSConfiguration.cpp src/NFDRSInitConfig.cpp src/RunNFDRS.cpp src/RunNFDRSConfig.cpp src/RunNFDRSConfiguration.cpp)

add_library(config4cpp STATIC IMPORTED)
set_target_properties(config4cpp PROPERTIES IMPORTED_LOCATION ${CONFIG4CPP_LIB})
#target_link_libraries(${PROJECT_NAME} PRIVATE config4cpp)
target_link_libraries (${PROJECT_NAME} PUBLIC NFDRS4 fw21 PRIVATE config4cpp)
