/*
 * The contents of this file are subject to the terms of the Common Development
 * and Distribution License (the "License"). You may not use this file except in
 * compliance with the License. You can obtain a copy of the license at
 * SysCommon/license.html or http://www.sun.com/cddl/cddl.html. See the License
 * for the specific language governing permissions and limitations under the
 * License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each file and
 * include the License file at SysCommon/license.html.
 * If applicable, add the following below this CDDL HEADER, with the fields
 * enclosed by brackets "[]" replaced with your own identifying information:
 * Portions Copyright [yyyy] [name of copyright owner]
 */
// CPP Unit includes
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

// System Includes
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

#include "Common.h"

#ifdef WIN32
	#pragma warning(disable: 4996)
#endif

using namespace CPPUNIT_NS;
using namespace std;

int main( int argc, char* argv[] )
{
	// check the command line arguments
	if( argc < 2 )
	{
		// display usage and exit
		cout << "Incorrect arguments: you must supply the output XML file name" << endl;
		exit( 1 );
	}
	
	// check the output file, making sure we can open and write to it
	char* outputFile = argv[1];
	ofstream output( outputFile ); // try and open it for writing
	if( output.fail() )
	{
		cout << "Error: couldn't open output file [" << outputFile << "] for writing" << endl;
		exit( 1 );
	}

	////////////////////////
	// run the unit tests //
	////////////////////////
	// create the event manager and test controller
	TestResult controller;

	// add a listener that collects the results
	TestResultCollector result;
	controller.addListener( &result );

	// add a listener that prints a brief progress display while running
	BriefTestProgressListener progress;
	controller.addListener( &progress );

	// add suites from the registry
	// if the TEST.GROUP environment variable is set, use that as the class to run
	TestRunner runner;
	char *value = getenv( "TEST.GROUP" );
	if( value == NULL || strcmp(value,"") == 0 || strcmp(value,"${test.group}") == 0 )
	{
		runner.addTest( TestFactoryRegistry::getRegistry().makeTest() );
	}
	else
	{
		cout << "TEST.GROUP=" << value << endl;
		runner.addTest( TestFactoryRegistry::getRegistry(value).makeTest() );
	}

	// Run the tests
	runner.run( controller );

	// Output results
	XmlOutputter outputter( &result, output );
	outputter.write();
	cout << "Results written to [" << outputFile << "]" << endl;

	// Pass back the return code
	return result.wasSuccessful() ? 0 : 1;

}
