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
#include "Logger.h"
#include "Exception.h"
#include <map>
#include <set>

namespace SysCommon
{
	/**
	 * The Properties class represents a persistent set of properties. The Properties can be saved 
	 * to a file or loaded from a file. Each key and its corresponding value in the property list 
	 * is a string.
	 */
	class Properties
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		public:
			static const tchar* SEPARATOR_CHARS;
			static const unsigned int MAX_LINE = 4096;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			std::map<String,String> properties;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates an empty property list with no default values.
			 */
			Properties();

			/**
			 * Copy constructor
			 */
			Properties( const Properties& other );
			virtual ~Properties();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Advises whether this Properties instance contains a value for the specified key
			 *
			 * @param key key whose presence in this Properties instance is to be tested
			 *
			 * @return true if this Properties instance contains a value for the specified key
			 */
			bool containsKey( const tchar* key ) const;

			/**
			 * Searches for the property with the specified key in this property list. The method 
			 * returns NULL if the property is not found.
			 *
			 * @param key the property key.
			 * @return the value in this property list with the specified key value.
			 */
			const tchar* getProperty( const tchar* key ) const;

			/**
			 * Searches for the property with the specified key in this property list. The method 
			 * returns the default value argument if the property is not found.
			 *
			 * @param key the property key.
			 * @param defaultValue a default value.
			 *
			 * @return the value in this property list with the specified key value.
			 */
			const tchar* getProperty( const tchar* key, const tchar* defaultValue ) const;

			/**
			 * Sets the property value for the specified key in this property list.
			 *
			 * @param key the property key
			 * @param value the property value
			 */
			void setProperty( const tchar* key, const tchar* value );

			/**
			 * Inserts all of the key/value pairs contained in the other Properties instance into
			 * this Properties instance
			 *
			 * @param other the Properties instance to copy
			 */
			void putAll( const Properties& other );

			/**
			 * Reads a property list (key and element pairs) from the specified file in a simple 
			 * line-oriented format.
			 *
			 * Properties are processed in terms of lines. A line is defined as a line of 
			 * characters that is terminated either by a set of line terminator characters (\n or 
			 * \r or \r\n) or by the end of the file. A line may be either a blank line, a comment 
			 * line, or hold all of a key-element pair.
			 *
			 * A line that contains only white space characters is considered blank and is ignored.  
			 * A comment line has an ASCII '#' or '!' as its first non-white space character; 
			 * comment lines are also ignored and do not encode key-element information.
			 *
			 * The key contains all of the characters in the line starting with the first non-white 
			 * space character and up to, but not including, the first '=', ':', or white space 
			 * character other than a line terminator.
			 *
			 * All remaining characters on the line become part of the associated element string; 
			 * if there are no remaining characters, the element is the empty string
			 *
			 * @param filename The path to the file that should be read
			 *
			 * @throws IOException if the file was not read successfully
			 */
			void loadFromFile( const tchar* filename ) throw ( IOException );

			/**
			 * @see Properties::loadFromFile(const tchar*)
			 *
			 * @param filename The path to the file that should be read
			 * @param logger The logger to log diagnostic information to
			 *
			 * @return true if the file was read successfully, otherwise false.
			 */
			void loadFromFile( const tchar* filename, Logger* logger ) throw ( IOException );

			/**
			 * Populates the provided set with the list of property names contained within this
			 * Properties instance
			 *
			 * @param outNames The set to populate with the list of property names contained within
			 * this Properties instance
			 */
			void getPropertyNames( std::set<const tchar*>& outNames ) const;

			/**
			 * Returns the set of property names contained within this Properties instance
			 *
			 * @return the set of property names contained within this Properties instance
			 */
			std::set<const tchar*> getPropertyNames() const;

			/**
			 * Removes all entries from this Properties object
			 */
			void clear();
		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}