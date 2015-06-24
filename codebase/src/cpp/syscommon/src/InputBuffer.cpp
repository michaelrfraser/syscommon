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
#include "syscommon/io/InputBuffer.h"

#include <cstring>

#ifdef DEBUG
#include "debug.h"
#endif

using namespace syscommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
InputBuffer::InputBuffer( const char* data, size_t dataLength, bool littleEndian )
{
	this->data = data;
	this->dataLength = dataLength;
	this->readMarker = 0;
	this->littleEndian = littleEndian;
}

InputBuffer::~InputBuffer()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
unsigned char InputBuffer::readUInt8() throw ( IOException )
{
	char asBytes[sizeof(unsigned char)];
	this->readAndCopy( sizeof(unsigned char), asBytes );

	return *((unsigned char*)asBytes);
}

char InputBuffer::readInt8() throw ( IOException )
{
	char asBytes[sizeof(char)];
	this->readAndCopy( sizeof(char), asBytes );

	return *((char*)asBytes);
}

unsigned short InputBuffer::readUInt16() throw ( IOException )
{
	union 
	{ 
		char asBytes[sizeof(unsigned short)]; 
		unsigned short value; 
	} unionUInt16;

	this->readAndCopy( sizeof(unsigned short), unionUInt16.asBytes );

	return unionUInt16.value;
}

short InputBuffer::readInt16() throw ( IOException )
{
	union
	{
		char asBytes[sizeof(short)];
		short value;
	} unionInt16;

	this->readAndCopy( sizeof(short), unionInt16.asBytes );

	return unionInt16.value;
}

unsigned int InputBuffer::readUInt32() throw ( IOException )
{
	union
	{
		char asBytes[sizeof(unsigned int)];
		unsigned int value;
	} unionUInt32;

	this->readAndCopy( sizeof(unsigned int), unionUInt32.asBytes );

	return unionUInt32.value;
}

int InputBuffer::readInt32() throw ( IOException )
{
	union
	{
		char asBytes[sizeof(int)];
		int value;
	} unionInt32;
	this->readAndCopy( sizeof(int), unionInt32.asBytes );

	return unionInt32.value;
}

unsigned long long InputBuffer::readUInt64() throw ( IOException )
{
	union
	{
		char asBytes[sizeof(unsigned long long)];
		unsigned long long value;
	} unionUInt64;
	this->readAndCopy( sizeof(unsigned long long), unionUInt64.asBytes );

	return unionUInt64.value;
}

long long InputBuffer::readInt64() throw ( IOException )
{
	union
	{
		char asBytes[sizeof(long long)];
		long long value;
	} unionInt64;
	this->readAndCopy( sizeof(long long), unionInt64.asBytes );

	return unionInt64.value;
}

std::string InputBuffer::readUTF() throw ( IOException )
{
	size_t size = (size_t)readUInt16();
	if( this->getBytesRemaining() >= size )
	{
		std::string result( this->data + readMarker, size );
		readMarker += size;
		
		return result;
	}
	else
	{
		throw IOException( TEXT("Buffer overflow") );
	}
}

size_t InputBuffer::getBytesRemaining() const
{
	return this->dataLength - this->readMarker;
}

void InputBuffer::readAndCopy( size_t length, char* dest ) throw ( IOException )
{
	if( this->getBytesRemaining() >= length )
	{
		if( littleEndian )
		{
			::memcpy( dest, this->data + readMarker, length );
		}
		else
		{
			for( size_t i = 0 ; i < length ; ++i )
				dest[length - 1 - i] = this->data[readMarker + i];
		}

		readMarker += length;
	}
	else
	{
		throw IOException( TEXT("Buffer overflow") );
	}
}
