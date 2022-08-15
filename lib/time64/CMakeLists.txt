cmake_minimum_required (VERSION 3.8)

project(time64 VERSION 1.0 LANGUAGES CXX C)

IF(MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
ENDIF(MSVC)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(HEADERS 
	${HEADER_DIR}/time64.h
	${HEADER_DIR}/time64_config.h
	${HEADER_DIR}/time64_limits.h)
add_library(${PROJECT_NAME} STATIC
	${HEADERS}
	src/time64.c)

target_include_directories(${PROJECT_NAME}   PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
set(include_dest "include")
install(FILES ${HEADERS} DESTINATION "${include_dest}")