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

#include "Socket.h"
#include "assert.h"

using namespace SysCommon;

//----------------------------------------------------------
//                    STATIC VARIABLES
//----------------------------------------------------------

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Socket::Socket()
{
	_Socket();
}

Socket::Socket( NATIVE_IP_ADDRESS address, unsigned short port ) throw ( IOException )
{	
	_Socket();
	InetSocketAddress endpoint( address, port );
	this->connect( endpoint );
}

Socket::~Socket()
{
	if( isCreated() )
		this->close();

	Platform::cleanupSocketFramework();
}

void Socket::_Socket()
{
	Platform::initialiseSocketFramework();
	this->nativeSocket = NATIVE_SOCKET_UNINIT;
	this->created = false;
	this->connected = false;
	this->closed = false;
	this->inputShutdown = false;
	this->outputShutdown = false;
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
bool Socket::isCreated()
{
	return this->created;
}

void Socket::create() throw ( SocketException )
{
	if( isCreated() )
		throw SocketException( TEXT("Socket has already been created") );

	this->nativeSocket = ::socket( AF_INET, SOCK_STREAM, 0 );

	if( this->nativeSocket != NATIVE_SOCKET_UNINIT )
		this->created = true;
	else
		throw SocketException( Platform::describeLastSocketError() );

}

bool Socket::isConnected()
{
	return this->connected;
}

void Socket::connect( InetSocketAddress& endpoint ) throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( isConnected() )
		throw SocketException( TEXT("Socket is already connected") );

	if ( !isCreated() )
		this->create();

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	unsigned short networkOrderPort = ::htons( endpoint.getPort() );

	sockaddr_in remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr.s_addr = endpoint.getAddress();
	remoteAddress.sin_port = networkOrderPort;

	int connectResult = ::connect( this->nativeSocket, 
								  (sockaddr*)&remoteAddress, 
								  sizeof(sockaddr_in) );
	if( connectResult != SOCKET_ERROR )
		this->connected = true;
	else
		throw SocketException( Platform::describeLastSocketError() );
}

bool Socket::isClosed()
{
	return this->closed;
}

void Socket::close() throw ( IOException )
{
	if( !isClosed() )
	{
		if( isCreated() )
		{
			int closeResult = ::closesocket( this->nativeSocket );
			if( closeResult != SOCKET_ERROR )
			{
				this->nativeSocket = NATIVE_SOCKET_UNINIT;
				closed = true;
			}
			else
			{
				throw SocketException( Platform::describeLastSocketError() );
			}
		}
	}
}

int Socket::send( const char* buffer, int length ) throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( !isConnected() )
		throw SocketException( TEXT("Socket is not connected") );

	if( isOutputShutdown() )
		throw SocketException( TEXT("Socket output has been shutdown") );	

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int result = ::send( this->nativeSocket, buffer, length, NULL );
	if( result == SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );
	
	return result;
}

int Socket::receive( char* buffer, int length ) throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( !isConnected() )
		throw SocketException( TEXT("Socket is not connected") );

	if( isInputShutdown() )
		throw SocketException( TEXT("Socket input has been shutdown") );	

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int result = ::recv( this->nativeSocket, buffer, length, NULL );
	if( result == SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );

	return result;
}

bool Socket::isInputShutdown()
{
	return this->inputShutdown;
}

void Socket::shutdownInput() throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( !isConnected() )
		throw SocketException( TEXT("Socket is not connected") );

	if( isInputShutdown() )
		throw SocketException( TEXT("Socket input is already shutdown") );

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int shutdownResult = ::shutdown( this->nativeSocket, 0x01 );
	if( shutdownResult == 0 )
		inputShutdown = true;
	else
		throw SocketException( Platform::describeLastSocketError() );
}

bool Socket::isOutputShutdown()
{
	return this->outputShutdown;
}

void Socket::shutdownOutput()
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( !isConnected() )
		throw SocketException( TEXT("Socket is not connected") );

	if( isInputShutdown() )
		throw SocketException( TEXT("Socket output is already shutdown") );

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int shutdownResult = ::shutdown( this->nativeSocket, 0x00 );
	if( shutdownResult == 0 )
		outputShutdown = true;
	else
		throw SocketException( Platform::describeLastSocketError() );
}
