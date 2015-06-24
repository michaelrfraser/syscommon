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

#include <vector>
#include "syscommon/Platform.h"

namespace syscommon
{
	/**
	 * A collection of static utility functions
	 */
	class StringUtils
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
			static bool stringStartsWith( const String& string, const String& prefix );

			/**
			 * Tests if the provided string ends with the specified suffix.
			 *
			 * @param string the String to test
			 * @param suffix the suffix.
			 * @return true if the character sequence represented by the suffix argument is a 
			 * suffix of the character sequence represented by the provided string.
			 */
			static bool stringEndsWith( const String& string, const String& suffix );

			/**
		     * Converts all of the characters in this String to upper case
			 */
			static String stringToUpperCase( const String& string );

			/**
			 * Splits the source string into several smaller strings, divided by the delimiter token.
			 *
			 * @param source the string to split
			 * @param delimiter the sequence of characters to use as the delimiter token
			 * @return a list of all substrings that were extracted from the source string
			 */
			static std::vector<String> stringSplit( const String& source, 
			                                        const String& delimiter );

			/**
			 * Formats the specified time value using the provided format string and returns the
			 * result.
			 * <br/><br/>
			 * The format string can contain any combination of regular characters and special 
			 * format specifiers. These format specifiers are replaced by the function to the 
			 * corresponding values to represent the time specified in the <code>time</code> 
			 * parameter. The format specifiers all begin with the percentage (%) sign and are:
			 *
			 * <table>
			 *	<tr>
			 *		<th>Specifier</th><th>Replaced By</th><th>Example</th>
			 *	</tr>
			 *	<tr>
			 *		<td>%a</td><td>Abbreviated weekday name *</td><td>Thu</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%A</td><td>Full weekday name *</td><td>Thursday</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%b</td><td>Abbreviated month name *</td><td>Aug</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%B</td><td>Full month name *</td><td>August</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%c</td><td>Date and time representation *</td><td>Thu Aug 23 14:55:02 2001</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%d</td><td>Day of the month, zero-padded (01-31)</td><td>23</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%H</td><td>Hour in 24h format (00-23)</td><td>14</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%H</td><td>Hour in 12h format (01-22)</td><td>02</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%j</td><td>Day of the year (001-366)</td><td>235</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%m</td><td>Month as a decimal number (01-12)</td><td>08</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%M</td><td>Minute (00-59)</td><td>55</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%p</td><td>AM or PM designation</td><td>PM</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%S</td><td>Second (00-61)</td><td>02</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%w</td><td>Weekday as a decimal number with Sunday as 0 (0-6)</td><td>4</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%W</td><td>Week number with the first Monday as the first day of week one (00-53)</td><td>34</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%x</td><td>Date representation *</td><td>08/23/01</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%X</td><td>Time representation *</td><td>14:55:02</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%y</td><td>Year, last two digits (00-99)</td><td>01</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%Y</td><td>Year</td><td>2001</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%Z</td><td>Timezone name or abbreviation *</td><td>CDT</td>
			 *	</tr>
			 *	<tr>
			 *		<td>%%</td><td>A % sign</td><td>%</td>
			 *	</tr>
			 * </table>
			 *	* The specifiers marked with an asterisk (*) are locale-dependent.
			 *
			 * @param time the time value to format
			 * @param format a format string containing any combination of regular characters and 
			 *               special format specifiers.
			 */
			static String formatTime( const time_t& time, const String& format );
	};
}
