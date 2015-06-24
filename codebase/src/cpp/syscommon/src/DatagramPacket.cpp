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
#include "syscommon/net/DatagramPacket.h"

#include <assert.h>

#ifdef DEBUG
#include "debug.h"
#endif

using namespace syscommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
DatagramPacket::DatagramPacket( char* buffer, 
								int offset, 
								int length )
{
	_DatagramPacket( buffer, offset, length, INADDR_NONE, 0 );	
}

DatagramPacket::DatagramPacket( char* buffer, 
								int length )
{
	_DatagramPacket( buffer, 0, length, INADDR_NONE, 0 );
}

DatagramPacket::DatagramPacket( const char* buffer, 
								int offset, 
								int length, 
								NATIVE_IP_ADDRESS address, 
								unsigned short port )
{
	_DatagramPacket( (char*)buffer, offset, length, address, port );
}

DatagramPacket::DatagramPacket( const char* buffer,
								int offset,
								int length,
								InetSocketAddress& address )
{
	_DatagramPacket( (char*)buffer, 
					 offset, 
					 length, 
					 address.getAddress(), 
					 address.getPort() );
}

DatagramPacket::~DatagramPacket()
{

}

void DatagramPacket::_DatagramPacket( char* buffer, 
									  int offset, 
									  int length, 
									  NATIVE_IP_ADDRESS address, 
									  unsigned short port )
{
	setData( buffer, offset, length );
	this->address = address;
	this->port = port;
	this->bufferLength = length;
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void DatagramPacket::setData( char* buffer, int offset, int length )
{
	assert( buffer );

	this->buffer = buffer;
	this->offset = offset;
	this->length = length;
	this->bufferLength = length;
}

void DatagramPacket::setAddress( NATIVE_IP_ADDRESS address )
{
	this->address = address;
}

void DatagramPacket::setPort( unsigned short port )
{
	this->port = port;
}

void DatagramPacket::setLength( int length )
{
	this->length = length;
}

char* DatagramPacket::getData() const
{
	return this->buffer;
}

int DatagramPacket::getOffset() const
{
	return this->offset;
}

int DatagramPacket::getLength() const
{
	return this->length;
}

int DatagramPacket::getBufferLength() const
{
	return this->bufferLength;
}

NATIVE_IP_ADDRESS DatagramPacket::getAddress() const
{
	return this->address;
}

unsigned short DatagramPacket::getPort() const
{
	return this->port;
}

InetSocketAddress DatagramPacket::getSocketAddress() const
{
	return InetSocketAddress( address, port );
}
