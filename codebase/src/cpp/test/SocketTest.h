#pragma once

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

class SocketTest: public CppUnit::TestFixture
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		Socket* socket;
		StringServer* server;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		SocketTest();
		virtual ~SocketTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testLifeCycle();
		void testDefaultConstructor();
		void testAddressConstructor();
		void testAddressConstructorDeadEndpoint();
		void testAddressConstructorAddrNone();
		void testConnect();
		void testConnectDeadEndpoint();
		void testConnectClosed();
		void testConnectAlreadyConnected();
		void testSend();
		void testSendClosed();
		void testSendNotConnected();
		void testSendOutputShutdown();
		void testSendNullBuffer();
		void testSendNegativeSize();
		void testReceive();
		void testReceiveClosed();
		void testReceiveNotConnected();
		void testReceiveInputShutdown();
		void testReceiveNullBuffer();
		void testReceiveNegativeSize();
		void testGetInetAddress();
		void testGetInetAddressNotConnected();
		void testGetInetAddressDisconnected();
		void testGetPort();
		void testGetPortNotConnected();
		void testGetPortDisconnected();
		void testGetRemoteSocketAddress();
		void testGetRemoteSocketAddressNotConnected();
		void testGetRemoteSocketAddressDisconnected();
		void testShutdownInput();
		void testShutdownInputNotConnected();
		void testShutdownInputDisconnected();
		void testShutdownInputAlreadyShutdown();
		void testShutdownOutput();
		void testShutdownOutputNotConnected();
		void testShutdownOutputDisconnected();
		void testShutdownOutputAlreadyShutdown();

	private:
		void quickStringSend( const string& message ) throw ( IOException );
		string quickStringReceive() throw ( IOException );

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( SocketTest );
		CPPUNIT_TEST( testLifeCycle );
		CPPUNIT_TEST( testDefaultConstructor );
		CPPUNIT_TEST( testAddressConstructor );
		CPPUNIT_TEST( testAddressConstructorDeadEndpoint );
		CPPUNIT_TEST( testAddressConstructorAddrNone );
		CPPUNIT_TEST( testConnectDeadEndpoint );
		CPPUNIT_TEST( testConnectClosed );
		CPPUNIT_TEST( testConnectAlreadyConnected );
		CPPUNIT_TEST( testSend );
		CPPUNIT_TEST( testSendClosed );
		CPPUNIT_TEST( testSendNotConnected );
		CPPUNIT_TEST( testSendOutputShutdown );
		CPPUNIT_TEST( testSendNullBuffer );
		CPPUNIT_TEST( testSendNegativeSize );
		CPPUNIT_TEST( testReceive );
		CPPUNIT_TEST( testReceiveClosed );
		CPPUNIT_TEST( testReceiveNotConnected );
		CPPUNIT_TEST( testReceiveInputShutdown );
		CPPUNIT_TEST( testReceiveNullBuffer );
		CPPUNIT_TEST( testReceiveNegativeSize );
		CPPUNIT_TEST( testGetInetAddress );
		CPPUNIT_TEST( testGetInetAddressNotConnected );
		CPPUNIT_TEST( testGetInetAddressDisconnected );
		CPPUNIT_TEST( testGetPort );
		CPPUNIT_TEST( testGetPortNotConnected );
		CPPUNIT_TEST( testGetPortDisconnected );
		CPPUNIT_TEST( testGetRemoteSocketAddress );
		CPPUNIT_TEST( testGetRemoteSocketAddressNotConnected );
		CPPUNIT_TEST( testGetRemoteSocketAddressDisconnected );
		CPPUNIT_TEST( testShutdownInput );
		CPPUNIT_TEST( testShutdownInputNotConnected );
		CPPUNIT_TEST( testShutdownInputDisconnected );
		CPPUNIT_TEST( testShutdownInputAlreadyShutdown );
		CPPUNIT_TEST( testShutdownOutput );
		CPPUNIT_TEST( testShutdownOutputNotConnected );
		CPPUNIT_TEST( testShutdownOutputDisconnected );
		CPPUNIT_TEST( testShutdownOutputAlreadyShutdown );
	CPPUNIT_TEST_SUITE_END();
};

