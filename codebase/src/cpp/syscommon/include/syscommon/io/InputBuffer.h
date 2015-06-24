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

#include <string>

#include "syscommon/Exception.h"

namespace syscommon
{
	class InputBuffer
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			const char* data;
			size_t dataLength;
			size_t readMarker;
			bool littleEndian;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			InputBuffer( const char* data, size_t dataLength, bool littleEndian );

			virtual ~InputBuffer();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			unsigned char readUInt8() throw ( IOException );
			char readInt8() throw ( IOException );

			unsigned short readUInt16() throw ( IOException );
			short readInt16() throw ( IOException );

			unsigned int readUInt32() throw ( IOException );
			int readInt32() throw ( IOException );

			unsigned long long readUInt64() throw ( IOException );
			long long readInt64() throw ( IOException );

			std::string readUTF() throw ( IOException );

		private:
			size_t getBytesRemaining() const;
			void readAndCopy( size_t length, char* dest ) throw ( IOException );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}
