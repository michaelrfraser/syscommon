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
#include "Common.h"
#ifdef WIN32
	#pragma warning(disable: 4996)
#endif

#ifdef DEBUG
#include "debug.h"
#endif

/*
 * Fail the current test with the given message
 */
void failTest( const char *format, ... )
{
	// start the var-arg stuff
	va_list args;
	va_start( args, format );

	// turn the args into a single string
	// http://www.cplusplus.com/reference/clibrary/cstdio/vsprintf.html
	char buffer[4096];
	vsprintf( buffer, format, args );

	// clean up the varargs
	va_end( args );

	// kill the test
	CPPUNIT_FAIL( buffer );
}

/*
 * Test should fail because an exception was expected, but none occurred. The failure message
 * will also include the action that was underway (and should have caused an exception).
 */
void failTestMissingException( const char *expectedException, const char* action )
{
	char buffer[4096];
	sprintf( buffer,
	         "(missingException) Expected an exception (%s) while %s",
	         expectedException,
	         action );
	CPPUNIT_FAIL( buffer );
}

/*
 * An exception was received, but it wasn't the one we expected. The failure message will
 * include the expected and actual exception types and a message regarding the action that
 * was in progress.
 */
void failTestWrongException( const char* expected, std::exception& actual, const char* action )
{
	char buffer[4096];
	sprintf( buffer,
	         "(wrongException) Wrong exception received while %s: expected [%s], received a different exception with this message: [%s]\n",
	         action,
	         expected,
	         actual.what() );
	CPPUNIT_FAIL( buffer );
}

StringServer* quickCreateServer( NATIVE_IP_ADDRESS iface, unsigned short port )
{
	StringServer* server = new StringServer( InetSocketAddress(iface, port) );
	try
	{
		server->start();
	}
	catch( IOException& ioe )
	{
		delete server;
		server = NULL;
		failTest( "Unexpected exception while starting server [%s]", ioe.what() );
	}

	return server;
}

void quickReleaseServer( StringServer* server )
{
	if( server )
	{
		if( server->isStarted() )
			server->stop();

		delete server;
	}
}