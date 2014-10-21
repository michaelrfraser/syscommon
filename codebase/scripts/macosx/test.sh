#!/bin/bash

BUILD_DIR=build/test64
SRC_DIR=src/cpp/test
CC=g++
INCLUDES="-Isrc/cpp/syscommon/include -Ilib/cppunit/cppunit-1.12.1/include"
CFLAGS="-g -Wall"
LDFLAGS="-lpthread -Llib/cppunit/cppunit-1.12.1/macosx -lcppunit -Ldist -lsyscommon64d"
DIST_DIR=dist
OUTNAME=test64

function fail
{
	echo
	echo BUILD FAILED
	exit
}

if [ ! -d "$BUILD_DIR" ]; then
	mkdir -p $BUILD_DIR
fi

echo Compiling to $BUILD_DIR
LINKOBJECTS=

for SOURCEFILE in $SRC_DIR/*.cpp
do
	BASENAME=`basename $SOURCEFILE`
	UNITNAME="${BASENAME%%.*}"

	echo [CC] $SOURCEFILE
	RESULT=`$CC $INCLUDES $CFLAGS -c $SOURCEFILE -o $BUILD_DIR/$UNITNAME.o && echo OK`
	if [ "$RESULT" != "OK" ]; then
		fail
	fi

	LINKOBJECTS="$LINKOBJECTS $BUILD_DIR/$UNITNAME.o"
done

echo [LD] $DIST_DIR/$OUTNAME
RESULT=`$CC $LINKOBJECTS -o $DIST_DIR/$OUTNAME $LDFLAGS && echo OK`
if [ "$RESULT" != "OK" ]; then
	fail
fi
