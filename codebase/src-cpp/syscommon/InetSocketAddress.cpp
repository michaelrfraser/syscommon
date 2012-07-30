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
#include "InetSocketAddress.h"

using namespace SysCommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
InetSocketAddress::InetSocketAddress( const unsigned short port )
{
	this->address = INADDR_ANY;
	this->port = port;
}

InetSocketAddress::InetSocketAddress( const NATIVE_IP_ADDRESS address, const unsigned short port )
{
	this->port = port;
	if( address == INADDR_NONE )
		this->address = INADDR_ANY;
	else
		this->address = address;
}

InetSocketAddress::InetSocketAddress( const tchar* hostname, const unsigned short port )
{
	this->hostname = hostname;
	this->port = port;

	// Resolve the ip address from the host name (will default to INADDR_NONE if the host
	// could not be resolved
	this->address = Platform::lookupHost( hostname );
}

InetSocketAddress::InetSocketAddress( const InetSocketAddress& other )
{
	this->hostname = other.hostname;
	this->address = other.address;
	this->port = other.port;
}

InetSocketAddress::~InetSocketAddress()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
bool InetSocketAddress::isUnresolved() const
{
	return this->address == INADDR_NONE;
}

unsigned short InetSocketAddress::getPort() const
{
	return this->port;
}

NATIVE_IP_ADDRESS InetSocketAddress::getAddress() const
{
	return this->address;
}

const tchar* InetSocketAddress::getHostName( bool lookup )
{
	if( this->hostname.empty() )
	{
		// No cached hostname, so we must perform a reverse lookup 
		if( !isUnresolved() && lookup )
		{
			// We have an address, so attempt to resolve the host name
			int nameSize = Platform::lookupHostName( this->address, AF_INET, NULL, 0 );
			if( nameSize > 0 )
			{
				// Name found, so cache it to a class member
				tchar* nameBuffer = new tchar[nameSize];
				Platform::lookupHostName( this->address, AF_INET, nameBuffer, nameSize );
				this->hostname = String( nameBuffer, nameSize );
				delete[] nameBuffer;
			}
		}

		// Still no name, so use the string representation of the address
		if( this->hostname.empty() )
			this->hostname = getHostAddress();
	}
	
	return hostname.c_str();
}

const tchar* InetSocketAddress::getHostAddress()
{
	if( this->addressString.empty() )
	{
		// No cached address string, so we must perform a reverse lookup 
		if( !isUnresolved() )
		{
			// The actual address is ok, so get it in string form
			int addressSize = Platform::getHostAddress(address, AF_INET, NULL, 0 );
			if( addressSize > 0 )
			{
				// Address can be converted to a name, so cache it to a class member
				tchar* addressBuffer = new tchar[addressSize];
				Platform::getHostAddress( address, AF_INET, addressBuffer, addressSize );
				addressString = String( addressBuffer, addressSize );
				delete[] addressBuffer;
			}
		}
		
		// Still no address string, so mark it as unresolved
		if( this->addressString.empty() )
			this->addressString = TEXT("Unresolved");
		
	}

	return this->addressString.c_str();
}