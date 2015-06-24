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

class InetSocketAddressTest: public CppUnit::TestFixture
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		InetSocketAddressTest();
		virtual ~InetSocketAddressTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testLookupHostDottedOctets();
		void testLookupHostName();
		void testPortConstructor();
		void testAddressPortConstructor();
		void testHostPortConstructor();
		void testGetHostAddress();
		void testGetHostName();
		void testLessThan();
		void testEquality();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( InetSocketAddressTest );
		CPPUNIT_TEST( testLookupHostDottedOctets );
		CPPUNIT_TEST( testLookupHostName );
		CPPUNIT_TEST( testPortConstructor );
		CPPUNIT_TEST( testAddressPortConstructor );
		CPPUNIT_TEST( testHostPortConstructor );
		CPPUNIT_TEST( testGetHostAddress );
		CPPUNIT_TEST( testLessThan );
		CPPUNIT_TEST( testEquality );
	CPPUNIT_TEST_SUITE_END();
};

