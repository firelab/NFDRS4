#--------
# Default BUILD_TYPE is "release" (rather than "debug").
# This default value can be overridden by running "make BUILD_TYPE=debug"
#--------
BUILD_TYPE = release

OS= $(shell uname)

ifeq ($(patsubst Cygwin%,Cygwin,$(OS)),Cygwin)
OS=Cygwin
else ifeq ($(patsubst CYGWIN%,Cygwin,$(OS)),Cygwin)
OS=Cygwin
endif

ifeq ($(OS),Gnu/Linux)
OS=Linux
endif
ifeq ($(OS),GNU/Linux)
OS=Linux
endif


LIB_DIR=$(PROJECT_ROOT)/lib
BIN_DIR=$(PROJECT_ROOT)/bin
INC_DIR=$(PROJECT_ROOT)/include


#--------
# EXE_EXT is ".exe" on Windows, and "" on UNIX-like operating systems.
#--------
ifeq ($(OS),Cygwin)
EXE_EXT=.exe
else
EXE_EXT=
endif

LINK_WITH_CONFIG4CPP= -L$(LIB_DIR) -lconfig4cpp

#--------
# Update CXX_FLAGS based on BUILD_TYPE (debug or release)
#--------
ifeq ($(BUILD_TYPE),debug)
override CXX_FLAGS+= -I$(PROJECT_ROOT)/include -g -fno-common
else
override CXX_FLAGS+= -I$(PROJECT_ROOT)/include -g -fno-common -O3 -DNDEBUG
endif

ifneq ($(OS),Cygwin)
override CXX_FLAGS+= -fPIC
endif


LINK_FLAGS=	


#-----
# Determine:
#	- The commands used to build a library
#	- The C++ compiler
#-----
ifeq ($(OS),Linux)
#-----
# Linux
#-----
AR			= ar
AR_FLAGS	= -r
RANLIB		= /bin/true
RANLIB_FLAGS= 
CXX			= g++
CREATE_SHARED_LIB_FLAGS = -shared
SHARED_LIB_EXT = .so
else ifeq ($(OS),Cygwin)
#-----
# Cygwin: same as Linux, but shared libraries/DLLs are not yet supported on Windows
#-----
AR			= ar
AR_FLAGS	= -r
RANLIB		= /bin/true
RANLIB_FLAGS= 
CXX			= g++
else ifeq ($(OS),SunOS)
#-----
# Solaris
#-----
AR			= ar
AR_FLAGS	= cr
RANLIB		= /bin/true
RANLIB_FLAGS= 
CXX			= g++
CREATE_SHARED_LIB_FLAGS = -shared
SHARED_LIB_EXT = .so
else ifeq ($(OS),HPUX)
#-----
# HPUX
#-----
AR			= ar
AR_FLAGS	= cr
RANLIB		= /bin/true
RANLIB_FLAGS= 
CXX			= g++
CREATE_SHARED_LIB_FLAGS = -shared
SHARED_LIB_EXT = .so
else ifeq ($(OS),Aix)
#-----
# AIX
#-----
AR			= ar
AR_FLAGS	= -r
RANLIB		= ranlib
RANLIB_FLAGS= 
CXX			= g++
CREATE_SHARED_LIB_FLAGS = -shared
SHARED_LIB_EXT = .so
else ifeq ($(OS),Darwin)
#-----
# Darwin, that is, Apple OS X
#-----
AR			= ar
AR_FLAGS	= cr
RANLIB		= ranlib
RANLIB_FLAGS= 
CXX			= g++
CREATE_SHARED_LIB_FLAGS = -dynamiclib
SHARED_LIB_EXT = .dylib
endif





#--------
# Generic build rules
#--------
.SUFFIXES:		.cpp

.cpp.o:
	$(CXX) -c $(CXX_FLAGS) $<
