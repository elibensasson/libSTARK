OSFLAG 				:=

# In current buildsystem CC is confused with CXX, 
# to have next part of this file works, its needed
# to override CC with CXX environment variable
CC=$(CXX)

ifeq ($(OS),Windows_NT)
	# TODO
else
	LDFLAGS += -L /usr/local/lib
	 CFLAGS += -I /usr/local/include
 
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		# Use default system-wide g++ if CC is not defined
		ifeq ($(CC),)
			CC=/usr/bin/g++
		endif

		AR=/usr/bin/ar crv
	endif
	
	ifeq ($(UNAME_S),Darwin)
		# Use g++ provided by Homebrew
		# brew install gcc-c++ v8
		ifeq ($(CC),)
			CC=/usr/local/bin/g++-8
		endif
		# XCode libtool instead of gnu ar
		AR=libtool -static -a -o
	endif

 	# Currently openmp is part of gcc:
	ifeq ($(shell $(CC) -v 2>&1 | grep -c "gcc version"), 1)
		CFLAGS += -fopenmp
	endif

	# But openmp could be linked while used clang as well
	# Homebrew: brew install libomp 
	ifeq ($(shell $(CC) -v 2>&1 | grep -c "clang version"), 1)
		CFLAGS += "-fopenmp=libomp"
	endif
endif

CPPFLAGS += -std=c++11
CFLAGS += -O3 -g -Wall -fmessage-length=0 -maes -msse4 -mtune=native

