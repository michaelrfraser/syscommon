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

class StringUtilsTest : public CppUnit::TestFixture
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
		StringUtilsTest();
		virtual ~StringUtilsTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testLongToString();
		void testUnsignedShortToString();
		void testTrimLeft();
		void testTrimRight();
		void testTrim();
		void testEquals();
		void testStartsWith();
		void testEndsWith();
		void testToUpperCase();
		void testSplit();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( StringUtilsTest );
		CPPUNIT_TEST( testLongToString );
		CPPUNIT_TEST( testUnsignedShortToString );
		CPPUNIT_TEST( testTrimLeft );
		CPPUNIT_TEST( testTrimRight );
		CPPUNIT_TEST( testTrim );
		CPPUNIT_TEST( testEquals );
		CPPUNIT_TEST( testStartsWith );
		CPPUNIT_TEST( testEndsWith );
		CPPUNIT_TEST( testToUpperCase );
		CPPUNIT_TEST( testSplit );
	CPPUNIT_TEST_SUITE_END();
};
