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
	 * This class represents an IP Socket Address (IP address + port number)
	 */
	class InetSocketAddress
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			String hostname;
			String addressString;
			NATIVE_IP_ADDRESS address;
			unsigned short port;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates a socket address where the IP address is the wildcard address and the port 
			 * number a specified value.
			 *
			 * A port number of zero will let the system pick up an ephemeral port in a bind 
			 * operation.
			 *
			 * @param port The port number
			 */
			InetSocketAddress( const unsigned short port );

			/**
			 * Creates a socket address from an IP address and a port number.
			 *
			 * A port number of zero will let the system pick up an ephemeral port in a bind 
			 * operation.
			 *
			 * @param addr The IP address
			 * @param port The port number
			 */
			InetSocketAddress( const NATIVE_IP_ADDRESS address, const unsigned short port );

			/**
			 * Creates a socket address from a hostname and a port number.
			 *
			 * An attempt will be made to resolve the hostname into an ip address.
			 * If that attempt fails, the address will be flagged as unresolved.
			 *
			 * A port number of <code>zero</code> will let the system pick up an
			 * ephemeral port in a <code>bind</code> operation.
			 * 
			 * @param hostname the Host name
			 * @param port the port number
			 */
			InetSocketAddress( const tchar* hostname, const unsigned short port );

			/**
			 * Copy constructor
			 */
			InetSocketAddress( const InetSocketAddress& other );

			virtual ~InetSocketAddress();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Checks whether the address has been resolved or not.
			 *
			 * @return true if the hostname couldn't be resolved into an ip address
			 */
			bool isUnresolved() const;

			/**
			 * Gets the port number
			 *
			 * @return the port number
			 */
			unsigned short getPort() const;

			/**
			 * Gets the ip address
			 *
			 * @return the ip address or INADDR_NONE if it is unresolved.
			 */
			NATIVE_IP_ADDRESS getAddress() const;

			/**
			 * Returns the hostname, or the String form of the address if it doesn't have a 
			 * hostname. The lookup parameter controls whether the function will perform a reverse
			 * lookup for the name (triggered if the instance was not constructed with a string
			 * literal).
			 *
			 * @param whether a reverse name lookup should be performed if the hostname has not
			 * already been resolved.
			 *
			 * @return the hostname, or String representation of the address.
			 */
			const tchar* getHostName( bool lookup );

			/**
			 * Returns the address part of this InetSocketAddress in String form
			 *
			 * @return the address part of this InetSocketAddress in String form
			 */
			const tchar* getHostAddress();

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------	
	};
}