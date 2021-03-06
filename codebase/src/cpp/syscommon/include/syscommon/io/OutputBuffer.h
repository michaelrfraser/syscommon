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
	class OutputBuffer
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		
		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			char* data;
			size_t dataLength;
			size_t writeMarker;
			bool littleEndian;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			OutputBuffer( size_t dataLength, bool littleEndian );

			virtual ~OutputBuffer();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			void writeUInt8( unsigned char value ) throw ( IOException );
			void writeInt8( char value ) throw ( IOException );

			void writeUInt16( unsigned short value ) throw ( IOException );
			void writeInt16( short value ) throw ( IOException );

			void writeUInt32( unsigned int value ) throw ( IOException );
			void writeInt32( int value ) throw ( IOException );

			void writeUInt64( unsigned long long value ) throw ( IOException );
			void writeInt64( long long value ) throw ( IOException );

			void writeUTF( const std::string& value ) throw ( IOException );

			size_t getLength();
			const char* getData();

		private:
			size_t getBytesRemaining();
			void write( size_t length, const char* source ) throw ( IOException );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}
