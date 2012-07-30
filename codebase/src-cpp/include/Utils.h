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

#include "Platform.h"

namespace SysCommon
{
	/**
	 * A collection of static utility functions
	 */
	class Utils
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		public:
			static const int MAX_LTOT_SIZE = 33;

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
		public:
			//
			// String utilities
			//
			/**
			 * Returns the string representation of the long argument.
			 *
			 * @param value a long.
			 *
			 * @return a string representation of the long argument.
			 */
			static String longToString( const long value );

			/**
			 * Returns the string representation of the unsigned short argument.
			 *
			 * @param value an unsigned short.
			 *
			 * @return a string representation of the unsigned short argument.
			 */
			static String unsignedShortToString( const unsigned short value );

			/**
			 * Returns a copy of the string parameter, trimmed of any leading characters that are
			 * contained in the characters parameter.
			 *
			 * @param string the string to trim leading characters from
			 * @param characters a string of characters that should be trimmed from the input 
			 * string
			 *
			 * @return a copy of the input string, trimmed of any leading characters that are
			 * contained in the characters parameter.
			 */
			static String stringTrimLeft( const String& string, const String& characters );

			/**
			 * Returns a copy of the string parameter, trimmed of any trailing characters that are
			 * contained in the characters parameter.
			 *
			 * @param string the string to trim trailing characters from
			 * @param characters a string of characters that should be trimmed from the input 
			 * string
			 *
			 * @return a copy of the input string, trimmed of any trailing characters that are
			 * contained in the characters parameter.
			 */
			static String stringTrimRight( const String& string, const String& characters );

			/**
			 * Returns a copy of the string parameter, trimmed of any leading and trailing
			 * whitespace characters.
			 *
			 * @param string the string to trim leading and trailing characters from
			 *
			 * @return a copy of the input string, trimmed of any leading and trailing whitespace
			 * characters.
			 */
			static String stringTrim( const String& string );

			/**
			 * Returns a copy of the string parameter, trimmed of any leading and trailing
			 * that are contained in the characters parameter.
			 *
			 * @param string the string to trim leading and trailing characters from
			 * @param characters a string of characters that should be trimmed from the input 
			 * string
			 *
			 * @return a copy of the input string, trimmed of any leading and trailing characters
			 * that are contained in the characters parameter
			 */
			static String stringTrim( const String& string, const String& characters );

			/**
			 * Compares one string to another. The result is true if and only if the character
			 * sequence contained in the strings are identical.
			 *
			 * @param string1 The first string to compare
			 * @param string2 The second string to compare
			 *
			 * @return  true if the two provided strings are equivalent
			 */
			static bool stringEquals( const String& string1, const String& string2 );

			/**
			 * Tests if the provided string starts with the specified prefix.
			 *
			 * @param string the String to test
			 * @param prefix the prefix.
			 * @return true if the character sequence represented by the prefix argument is a 
			 * prefix of the character sequence represented by the provided string.
			 */
			static bool stringStartsWith( const String& prefix, const String& test );

			/**
			 * Tests if the provided string ends with the specified suffix.
			 *
			 * @param string the String to test
			 * @param suffix the suffix.
			 * @return true if the character sequence represented by the suffix argument is a 
			 * suffix of the character sequence represented by the provided string.
			 */
			static bool stringEndsWith( const String& suffix, const String& test );

			/**
		     * Converts all of the characters in this String to upper case
			 */
			static String stringToUpperCase( const String& string );
	};
}