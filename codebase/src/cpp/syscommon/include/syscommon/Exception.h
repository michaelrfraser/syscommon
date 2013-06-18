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

#include <exception>
#include <stdexcept>

#include "syscommon/Platform.h"

namespace syscommon
{
	class Exception : public std::runtime_error
	{
		public:
			Exception( const tchar* message ) :
				std::runtime_error( Platform::toAnsiString(message) )
			{

			}

			virtual ~Exception() throw ()
			{

			}
	};

	class IllegalArgumentException : public Exception
	{
		public:
			IllegalArgumentException( const tchar* message ) : Exception( message ) {}
			virtual ~IllegalArgumentException() throw () {}
	};

	class InterruptedException : public Exception
	{
		public:
			InterruptedException( const tchar* message ) : Exception( message ) {}
			virtual ~InterruptedException() throw () {}
	};

	class IOException : public Exception
	{
		public:
			IOException( const tchar* message ) : Exception( message ) {}
			virtual ~IOException() throw () {}
	};

	class SocketException : public IOException
	{
		public:
			SocketException( const tchar* message ) : IOException( message ) {}
			virtual ~SocketException() throw () {}
	};

	class FileNotFoundException : public IOException
	{
		public:
			FileNotFoundException( const tchar* message ) : IOException( message ) {}
			virtual ~FileNotFoundException() throw () {}
	};
}

