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

#include "io/InputBuffer.h"

using namespace SysCommon;

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
	char asBytes[sizeof(unsigned short)];
	this->readAndCopy( sizeof(unsigned short), asBytes );

	return *((unsigned short*)asBytes);
}

short InputBuffer::readInt16() throw ( IOException )
{
	char asBytes[sizeof(short)];
	this->readAndCopy( sizeof(short), asBytes );

	return *((short*)asBytes);
}

unsigned int InputBuffer::readUInt32() throw ( IOException )
{
	char asBytes[sizeof(unsigned int)];
	this->readAndCopy( sizeof(unsigned int), asBytes );

	return *((unsigned int*)asBytes);
}

int InputBuffer::readInt32() throw ( IOException )
{
	char asBytes[sizeof(int)];
	this->readAndCopy( sizeof(int), asBytes );

	return *((int*)asBytes);
}

unsigned long long InputBuffer::readUInt64() throw ( IOException )
{
	char asBytes[sizeof(unsigned long long)];
	this->readAndCopy( sizeof(unsigned long long), asBytes );

	return *((unsigned long long*)asBytes);
}

long long InputBuffer::readInt64() throw ( IOException )
{
	char asBytes[sizeof(long long)];
	this->readAndCopy( sizeof(long long), asBytes );

	return *((long long*)asBytes);
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

size_t InputBuffer::getBytesRemaining()
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
