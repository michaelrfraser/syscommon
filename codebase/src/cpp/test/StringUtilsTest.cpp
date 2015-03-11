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
#include "StringUtilsTest.h"

#include "syscommon/util/StringUtils.h"

using namespace std;
using namespace syscommon;

CPPUNIT_TEST_SUITE_REGISTRATION( StringUtilsTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
StringUtilsTest::StringUtilsTest()
{

}

StringUtilsTest::~StringUtilsTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void StringUtilsTest::setUp()
{

}

void StringUtilsTest::tearDown()
{

}

void StringUtilsTest::testLongToString()
{
	String result = StringUtils::longToString( 1234L );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result, TEXT("1234")) );
}

void StringUtilsTest::testUnsignedShortToString()
{
	String result = StringUtils::unsignedShortToString( 1234U );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result, TEXT("1234")) );
}

void StringUtilsTest::testTrimLeft()
{
	String source( TEXT("caabCdefgAaabc") );
	String result = StringUtils::stringTrimLeft( source, TEXT("abc") );

	CPPUNIT_ASSERT( StringUtils::stringEquals(result, TEXT("CdefgAaabc")) );
}

void StringUtilsTest::testTrimRight()
{
	String source( TEXT("abcdefgAcab") );
	String result = StringUtils::stringTrimRight( source, TEXT("abc") );

	CPPUNIT_ASSERT( StringUtils::stringEquals(result, TEXT("abcdefgA")) );
}

void StringUtilsTest::testTrim()
{
	String sourceCh( TEXT("caabCdefgAaabc") );
	String resultCh = StringUtils::stringTrim( sourceCh, TEXT("abc") );

	CPPUNIT_ASSERT( StringUtils::stringEquals(resultCh, TEXT("CdefgA")) );

	String sourceWs( TEXT("\tThe quick brown fox \n") );
	String resultWs = StringUtils::stringTrim( sourceWs );

	CPPUNIT_ASSERT( StringUtils::stringEquals(resultWs, TEXT("The quick brown fox")) );
}

void StringUtilsTest::testEquals()
{
	CPPUNIT_ASSERT( StringUtils::stringEquals(TEXT("The quick brown fox"), TEXT("The quick brown fox")) );

	// Case matters
	CPPUNIT_ASSERT( !StringUtils::stringEquals(TEXT("The quick brown fox"), TEXT("THE QUICK BROWN FOX")) );

	// White space matters
	CPPUNIT_ASSERT( !StringUtils::stringEquals(TEXT("The quick brown fox"), TEXT(" The quick brown fox")) );
}

void StringUtilsTest::testStartsWith()
{
	CPPUNIT_ASSERT( StringUtils::stringStartsWith(TEXT("The quick brown fox"), TEXT("The")) );
	CPPUNIT_ASSERT( !StringUtils::stringStartsWith(TEXT("The quick brown fox"), TEXT("fox")) );
}

void StringUtilsTest::testEndsWith()
{
	CPPUNIT_ASSERT( StringUtils::stringEndsWith(TEXT("The quick brown fox"), TEXT("fox")) );
	CPPUNIT_ASSERT( !StringUtils::stringEndsWith(TEXT("The quick brown fox"), TEXT("The")) );
}

void StringUtilsTest::testToUpperCase()
{
	String toUpper = StringUtils::stringToUpperCase( TEXT("The quick brown fox") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(toUpper, TEXT("THE QUICK BROWN FOX")) );
}

void StringUtilsTest::testSplit()
{
	String source = TEXT("one,two, three,four , five ,six");
	vector<String> result = StringUtils::stringSplit( source, "," );

	CPPUNIT_ASSERT( result.size() == 6 );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(0), "one") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(1), "two") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(2), " three") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(3), "four ") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(4), " five ") );
	CPPUNIT_ASSERT( StringUtils::stringEquals(result.at(5), "six") );
}
