cmake_minimum_required (VERSION 3.13)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project (
	NFDRS4
	VERSION 1.0
	DESCRIPTION "National Fire Danger Rating System 4"
	LANGUAGES CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

  # Let's ensure -std=c++xx instead of -std=g++xx
  set(CMAKE_CXX_EXTENSIONS OFF)

  # Let's nicely support folders in IDEs
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)

endif()

add_subdirectory(lib)

add_subdirectory(app)

#install locations for apps, headers, and libs
set(app_dest "bin")
set(include_dest "include")
set(lib_dest "lib")

