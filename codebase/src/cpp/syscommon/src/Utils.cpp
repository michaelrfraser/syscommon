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

#include "Utils.h"

using namespace SysCommon;

String Utils::longToString( const long value )
{
	// stdc _ltow will return up to 33 bytes
	tchar buffer[MAX_LTOT_SIZE];
	Platform::longToChars( value, buffer, MAX_LTOT_SIZE );
	return String( buffer );
}

String Utils::unsignedShortToString( const unsigned short value )
{
	// stdc _ltow will return up to 33 bytes
	tchar buffer[MAX_LTOT_SIZE];
	Platform::longToChars( value, buffer, MAX_LTOT_SIZE );
	return String( buffer );
}

String Utils::stringTrimLeft( const String& string, const String& characters )
{
	String result = string;
	size_t startPos = string.find_first_not_of( characters );
	if( String::npos != startPos )
		result = string.substr( startPos, result.length() );
	else if( string.length() > 0 )
		result.clear();

	return result;
}

String Utils::stringTrimRight( const String& string, const String& characters )
{
	String result = string;
	size_t endpos = string.find_last_not_of( characters );
	if( String::npos != endpos )
		result = string.substr( 0, endpos + 1 );
	else if( string.length() > 0 )
		result.clear();

	return result;
}

String Utils::stringTrim( const String& string, const String& characters )
{
	String result = Utils::stringTrimLeft( string, characters );
	result = Utils::stringTrimRight( result, characters );

	return result;
}

String Utils::stringTrim( const String& string )
{
	return Utils::stringTrim( string, TEXT("\n\r\t ") );
}


bool Utils::stringEquals( const String& string1, const String& string2 )
{
	return string1.compare(string2) == 0;
}

bool Utils::stringStartsWith( const String& string, const String& prefix )
{
	bool result = false;

	size_t stringLength = string.length();
	size_t testLength = prefix.length();

	if( testLength == stringLength )
	{
		result = Utils::stringEquals(prefix, string);
	}
	else if( testLength < stringLength )
	{
		String substring = string.substr( 0, testLength );
		result = Utils::stringEquals( prefix, substring );
	}

	return result;
}

bool Utils::stringEndsWith( const String& string, const String& suffix )
{
	bool result = false;

	size_t stringLength = string.length();
	size_t testLength = suffix.length();

	if( testLength == stringLength )
	{
		result = Utils::stringEquals( suffix, string );
	}
	else if( testLength < stringLength )
	{
		String substring = string.substr( stringLength - testLength );
		result = Utils::stringEquals( suffix, substring );
	}

	return result;
}

String Utils::stringToUpperCase( const String& string )
{
	size_t length = string.length();
	const tchar* data = string.data();
	tchar* copy = new tchar[length + 1];
	for( size_t i = 0 ; i < length ; ++i )
		copy[i] = totupper( data[i] );

	copy[length] = 0;
	String result = copy;
	delete[] copy;

	return result;
}
