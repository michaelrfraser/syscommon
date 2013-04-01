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
		void testDefaultConstructor();
		void testAddressConstructor();
		/*
		void testPortConstructor();
		void testIfaceConstructor();
		void testIfaceConstructorInvalid();
		void testJoinLeaveGroup();
		void testJoinOnClosedSocket();
		void testLeaveGroupWithoutJoin();
		void testAddressReusage();
		void testSendReceive();
		void testSendWhileClosed();
		void testSendNoAddress();
		void testReceiveWhileClosed();
		*/

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( SocketTest );
		CPPUNIT_TEST( testDefaultConstructor );
		CPPUNIT_TEST( testAddressConstructor );
	CPPUNIT_TEST_SUITE_END();
};

