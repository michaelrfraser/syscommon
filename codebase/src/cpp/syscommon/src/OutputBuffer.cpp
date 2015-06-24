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

#include "syscommon/io/OutputBuffer.h"

#include <assert.h>
#include <cstring>

#ifdef DEBUG
#include "debug.h"
#endif

using namespace syscommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
OutputBuffer::OutputBuffer( size_t dataLength, bool littleEndian )
{
	this->data = new char[dataLength];
	this->dataLength = dataLength;
	this->littleEndian = littleEndian;
	this->writeMarker = 0;
}

OutputBuffer::~OutputBuffer()
{
	assert( this->data );
	if( this->data )
		delete [] this->data;
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void OutputBuffer::writeUInt8( unsigned char value ) throw ( IOException )
{
	this->write( sizeof(unsigned char), (char*)&value );
}

void OutputBuffer::writeInt8( char value ) throw ( IOException )
{
	this->write( sizeof(char), (char*)&value );
}

void OutputBuffer::writeUInt16( unsigned short value ) throw ( IOException )
{
	this->write( sizeof(unsigned short), (char*)&value );
}

void OutputBuffer::writeInt16( short value ) throw ( IOException )
{
	this->write( sizeof(short), (char*)&value );
}

void OutputBuffer::writeUInt32( unsigned int value ) throw ( IOException )
{
	this->write( sizeof(unsigned int), (char*)&value );
}

void OutputBuffer::writeInt32( int value ) throw ( IOException )
{
	this->write( sizeof(int), (char*)&value );
}

void OutputBuffer::writeUInt64( unsigned long long value ) throw ( IOException )
{
	this->write( sizeof(unsigned long long), (char*)&value );
}

void OutputBuffer::writeInt64( long long value ) throw ( IOException )
{
	this->write( sizeof(long long), (char*)&value );
}

void OutputBuffer::writeUTF( const std::string& value ) throw ( IOException )
{
	size_t length = value.length();
	this->writeUInt16( (unsigned short)length );

	if( this->getBytesRemaining() >= length )
	{
		char* writePointer = this->data + this->writeMarker;
		const char* data = value.c_str();
		::memcpy( writePointer, data, length );

		writeMarker += length;
	}
	else
	{
		throw IOException( TEXT("Buffer overflow") );
	}
}

size_t OutputBuffer::getLength()
{
	return this->writeMarker;
}

const char* OutputBuffer::getData()
{
	return this->data;
}

size_t OutputBuffer::getBytesRemaining()
{
	return this->dataLength - writeMarker;
}

void OutputBuffer::write( size_t length, const char* source ) throw ( IOException )
{
	if( this->getBytesRemaining() >= length )
	{
		char* writePointer = this->data + writeMarker;
		if( littleEndian )
		{
			::memcpy( writePointer, source, length );	
		}
		else
		{
			for( size_t i = 0 ; i < length ; ++i )
				writePointer[length - 1 - i] = source[i];
		}
		writeMarker += length;
	}
	else
	{
		throw IOException( TEXT("Buffer overflow") );
	}
}
