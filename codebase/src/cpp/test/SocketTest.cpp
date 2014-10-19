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
#include "SocketTest.h"
#include "syscommon/Platform.h"
#include "syscommon/net/Socket.h"


CPPUNIT_TEST_SUITE_REGISTRATION( SocketTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
SocketTest::SocketTest()
{

}

SocketTest::~SocketTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void SocketTest::setUp()
{

}

void SocketTest::tearDown()
{

}

void SocketTest::testDefaultConstructor()
{
	try
	{
		// Socket should be created in an unconnected state
		syscommon::Socket testSocket;
		CPPUNIT_ASSERT( !testSocket.isConnected() );
		CPPUNIT_ASSERT( testSocket.isInputShutdown() );
		CPPUNIT_ASSERT( testSocket.isOutputShutdown() );
	}
	catch( std::exception& e )
	{
		failTest( "Unexpected exception while creating Socket with default constructor. Error [%s]",
				  e.what() );
	}
}

void SocketTest::testAddressConstructor()
{

}
