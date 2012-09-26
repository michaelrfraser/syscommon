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
#include "Properties.h"
#include "Utils.h"

using namespace SysCommon;

#pragma warning( disable : 4996 )

//----------------------------------------------------------
//                    STATIC VARIABLES
//----------------------------------------------------------
const tchar* Properties::SEPARATOR_CHARS = TEXT("=: \t");

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Properties::Properties()
{

}

Properties::Properties( const Properties& other )
{
	putAll( other );
}

Properties::~Properties()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
bool Properties::containsKey( const tchar* key )
{
	std::map<String,String>::iterator found = properties.find( key );
	return found != properties.end();
}

const tchar* Properties::getProperty( const tchar* key )
{
	return getProperty( key, NULL );
}

const tchar* Properties::getProperty( const tchar* key, const tchar* defaultValue )
{
	const tchar* result = defaultValue;

	std::map<String,String>::iterator found = properties.find( key );
	if( found != properties.end() )
		result = (*found).second.c_str();

	return result;
}

void Properties::setProperty( const tchar* key, const tchar* value )
{
	properties[key] = value;
}

void Properties::putAll( const Properties& other )
{
	// Iterate over all key/values in other and copy them into our properties
	std::map<String,String>::const_iterator otherIt = other.properties.begin();
	while( otherIt != other.properties.end() )
	{
		const String& key = (*otherIt).first;
		const String& value = (*otherIt).second;

		this->properties[key] = value;
		++otherIt;
	}
}

void Properties::loadFromFile( const tchar* filename ) throw ( IOException )
{
	return loadFromFile( filename, NULL );
}

void Properties::loadFromFile( const tchar* filename, Logger* logger ) throw ( IOException )
{
	if( !Platform::fileExists(filename) )
		throw FileNotFoundException( TEXT("File does not exist") );

	std::string narrowName = Platform::toAnsiString( filename );
	if( logger )
		logger->debug( TEXT("ATTEMPT: Load properties from file [%s]"), narrowName.c_str() );
	
	// Attempt to open the specified file
	FILE* file = ::fopen( narrowName.c_str(), "r" );
	if( file )
	{
		// Read each line
		char line[MAX_LINE];
		while( ::fgets(line, MAX_LINE, file) != NULL )
		{
			// Convert to String and trim whitespace
			String asString = Platform::toPlatformString( line );
			asString = Utils::stringTrim( asString );

			// Parse property statements (eg not comments or empty lines)
			if( !asString.empty() && 
				!Utils::stringStartsWith(asString, TEXT("#")) && 
				!Utils::stringStartsWith(asString, TEXT("!")) )
			{
				// Find the end of the key
				size_t keyEndPos = asString.find_first_of( SEPARATOR_CHARS );
				if( keyEndPos != String::npos )
				{
					String key = asString.substr( 0, keyEndPos );
					key = Utils::stringTrim( key );

					// Find the start fo the value
					size_t valueStart = asString.find_first_not_of( SEPARATOR_CHARS, keyEndPos );
					String value = asString.substr( valueStart, asString.length() );
					value = Utils::stringTrim( value );

					if( logger )
						logger->trace( TEXT("Parsed property KEY=[%s], VALUE=[%s]"), key.c_str(), value.c_str() );

					// Set the property
					properties[key] = value;
				}
				else
				{
					if( logger )
						logger->warn( TEXT("Ignoring malformed property statement [%s]"), asString.c_str() );
				}
			}
		}

		::fclose( file );
	}
	else
	{
		throw IOException( TEXT("File could not be opened for reading") );
	}
}

std::set<const tchar*> Properties::getPropertyNames()
{
	std::set<const tchar*> names;
	getPropertyNames( names );

	return names;
}

void Properties::getPropertyNames( std::set<const tchar*>& outNames )
{
	// Iterate over all entries and populate the provided set with the names
	std::map<String,String>::const_iterator propertiesIt = properties.begin();
	while( propertiesIt != properties.end() )
	{
		const String& key = (*propertiesIt).first;
		outNames.insert( key.c_str() );

		++propertiesIt;
	}
}