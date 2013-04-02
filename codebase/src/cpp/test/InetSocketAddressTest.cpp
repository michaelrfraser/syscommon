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
#include "InetSocketAddressTest.h"
#include "net/InetSocketAddress.h"
#include "Platform.h"

CPPUNIT_TEST_SUITE_REGISTRATION( InetSocketAddressTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
InetSocketAddressTest::InetSocketAddressTest()
{

}

InetSocketAddressTest::~InetSocketAddressTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void InetSocketAddressTest::setUp()
{

}

void InetSocketAddressTest::tearDown()
{

}

void InetSocketAddressTest::testLookupHostDottedOctets()
{
	// Host address
	NATIVE_IP_ADDRESS ipAddress = SysCommon::Platform::lookupHost( TEXT("127.0.0.1") );
	CPPUNIT_ASSERT( ipAddress != INADDR_NONE );
	CPPUNIT_ASSERT( ipAddress == INADDR_LOOPBACK );

	// Multicast address
	NATIVE_IP_ADDRESS multicastAddress = SysCommon::Platform::lookupHost( TEXT("226.0.1.1") );
	CPPUNIT_ASSERT( multicastAddress != INADDR_NONE );
}

void InetSocketAddressTest::testLookupHostName()
{
	NATIVE_IP_ADDRESS ipAddress = SysCommon::Platform::lookupHost( TEXT("localhost") );
	CPPUNIT_ASSERT( ipAddress != INADDR_NONE );
}

void InetSocketAddressTest::testPortConstructor()
{
	SysCommon::InetSocketAddress sockAddress( 3001 );
	CPPUNIT_ASSERT( !sockAddress.isUnresolved() );
	CPPUNIT_ASSERT( sockAddress.getAddress() == INADDR_ANY );
	CPPUNIT_ASSERT( sockAddress.isAnyAddress() );
	CPPUNIT_ASSERT( !sockAddress.isLoopbackAddress() );
	CPPUNIT_ASSERT( ::strcmp(sockAddress.getHostAddress(), TEXT("0.0.0.0")) == 0 );
	CPPUNIT_ASSERT( sockAddress.getPort() == 3001 );
}

void InetSocketAddressTest::testAddressPortConstructor()
{
	NATIVE_IP_ADDRESS multicastAddress = SysCommon::Platform::lookupHost( TEXT("226.0.1.1") );
	SysCommon::InetSocketAddress sockAddress( multicastAddress, 3001 );
	CPPUNIT_ASSERT( !sockAddress.isUnresolved() );
	CPPUNIT_ASSERT( !sockAddress.isAnyAddress() );
	CPPUNIT_ASSERT( !sockAddress.isLoopbackAddress() );
	CPPUNIT_ASSERT( sockAddress.getAddress() == multicastAddress );
	CPPUNIT_ASSERT( ::strcmp(sockAddress.getHostAddress(), TEXT("226.0.1.1")) == 0 );
	CPPUNIT_ASSERT( sockAddress.getPort() == 3001 );
}

void InetSocketAddressTest::testHostPortConstructor()
{
	NATIVE_IP_ADDRESS loopbackAddress = SysCommon::Platform::lookupHost( TEXT("127.0.0.1") );
	SysCommon::InetSocketAddress sockAddress( TEXT("localhost"), 3030 );

	CPPUNIT_ASSERT( !sockAddress.isUnresolved() );
	CPPUNIT_ASSERT( sockAddress.getAddress() == INADDR_LOOPBACK );
	CPPUNIT_ASSERT( !sockAddress.isAnyAddress() );
	CPPUNIT_ASSERT( sockAddress.isLoopbackAddress() );
	CPPUNIT_ASSERT( loopbackAddress == sockAddress.getAddress() );
	CPPUNIT_ASSERT( ::strcmp(sockAddress.getHostAddress(), TEXT("127.0.0.1")) == 0 );

	CPPUNIT_ASSERT( sockAddress.getPort() == 3030 );
}

void InetSocketAddressTest::testGetHostAddress()
{
	SysCommon::InetSocketAddress sockAddress( INADDR_LOOPBACK, 2099 );
	CPPUNIT_ASSERT( ::strcmp(sockAddress.getHostAddress(), TEXT("127.0.0.1")) == 0 );
}

void InetSocketAddressTest::testGetHostName()
{
	SysCommon::InetSocketAddress initiallySpecified( TEXT("localhost"), 2099 );
	CPPUNIT_ASSERT( ::strcmp(initiallySpecified.getHostName(true), TEXT("localhost")) == 0 );

	SysCommon::InetSocketAddress localLookup( INADDR_LOOPBACK, 2099 );
	CPPUNIT_ASSERT( ::strcmp(localLookup.getHostName(true), TEXT("localhost")) == 0 );
}
