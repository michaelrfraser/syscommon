#!/bin/bash

#
# Compiles/links the SysCommon source code to a 64-bit static library. This script creates both 
# DEBUG and RELEASE versions of the library.
#
# The script has been written to be run from the codebase/ directory. It sources the source code
# from the SRC_DIR variable, builds the compilation units in the BUILD_DIR directory, and finally
# links the library in DIST_DIR.
#

#
# DEFINITIONS
#
INCLUDES=-Isrc/cpp/syscommon/include
CC=clang
SRC_DIR=src/cpp/syscommon/src
DIST_DIR=dist
LIBNAME=syscommon

function main
{
	echo
	echo WELCOME to the SysCommon simple build!
	echo ======================================

	if [ ! -d "$DIST_DIR" ]; then
	  mkdir -p $DIST_DIR
	fi

	echo DEBUG ANSI
	build "-g -fPIC -Wall -Wno-unknown-pragmas" build/ansidebug libsyscommon64d.a
	echo
	
	echo RELEASE ANSI
	build "-O2 -fPIC -Wall -Wno-unknown-pragmas" build/ansirelease libsyscommon64.a
	echo
	
	echo DEBUG UNICODE
	build "-g -fPIC -Wall -Wno-unknown-pragmas -DUNICODE" build/unicodedebug libsyscommon64ud.a
	echo
	
	echo RELEASE UNICODE
	build "-O2 -fPIC -Wall -Wno-unknown-pragmas -DUNICODE" build/unicoderelease libsyscommon64u.a
	echo
	
	echo Done!
}

function build # cflags builddir outname 
{
	CFLAGS=$1
	BUILD_DIR=$2
	OUTNAME=$3

	if [ ! -d "$BUILD_DIR" ]; then
	  mkdir -p $BUILD_DIR
	fi

	echo Compiling to $BUILD_DIR
	LINKOBJECTS=
	
	for SOURCEFILE in $SRC_DIR/*.cpp
	do
		BASENAME=`basename $SOURCEFILE`
		UNITNAME="${BASENAME%%.*}"
		$CC $INCLUDES $CFLAGS -c $SOURCEFILE -o $BUILD_DIR/$UNITNAME.o
		LINKOBJECTS="$LINKOBJECTS $BUILD_DIR/$UNITNAME.o"
	done
	
	echo Linking to $DIST_DIR/$OUTNAME
	ar rcs $DIST_DIR/$OUTNAME $LINKOBJECTS
}

main