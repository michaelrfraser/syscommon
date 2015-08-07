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
#include "syscommon/net/Socket.h"

#include <assert.h>

#ifdef DEBUG
#include "debug.h"
#endif

using namespace syscommon;

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
	this->closed = false;
	this->inputShutdown = true;
	this->outputShutdown = true;

	this->remoteAddress = INADDR_NONE;
	this->remotePort = 0;
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
bool Socket::isCreated() const
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

bool Socket::isConnected() const
{
	return this->remoteAddress != INADDR_NONE;
}

void Socket::connect( const InetSocketAddress& endpoint ) throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( isConnected() )
		throw SocketException( TEXT("Socket is already connected") );

	if( endpoint.getAddress() == INADDR_ANY )
		throw SocketException( TEXT("Cannot connect to INADDR_NONE/INADDR_ANY") );

	if ( !isCreated() )
		this->create();

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	unsigned short networkOrderPort = htons( endpoint.getPort() );
	NATIVE_IP_ADDRESS networkOrderAddress = htonl( endpoint.getAddress() );

	sockaddr_in remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr.s_addr = networkOrderAddress;
	remoteAddress.sin_port = networkOrderPort;

	int connectResult = ::connect( this->nativeSocket, 
								  (sockaddr*)&remoteAddress, 
								  sizeof(sockaddr_in) );
	if( connectResult != NATIVE_SOCKET_ERROR )
	{
		this->remoteAddress = endpoint.getAddress();
		this->remotePort = endpoint.getPort();
		this->inputShutdown = false;
		this->outputShutdown = false;
	}
	else
	{
		throw SocketException( Platform::describeLastSocketError() );
	}
}

void Socket::connect( const InetSocketAddress& endpoint, int timeout ) throw ( IOException )
{
	// If no timeout was specified, then call the non-timeout version of connect
	if( timeout <= 0 )
		return connect( endpoint );

	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( isConnected() )
		throw SocketException( TEXT("Socket is already connected") );

	if( endpoint.getAddress() == INADDR_ANY )
		throw SocketException( TEXT("Cannot connect to INADDR_NONE/INADDR_ANY") );

	if ( !isCreated() )
		this->create();

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	unsigned short networkOrderPort = htons( endpoint.getPort() );
	NATIVE_IP_ADDRESS networkOrderAddress = htonl( endpoint.getAddress() );

	sockaddr_in remoteAddress;
	remoteAddress.sin_family = AF_INET;
	remoteAddress.sin_addr.s_addr = networkOrderAddress;
	remoteAddress.sin_port = networkOrderPort;

	// Set non-blocking 
	Platform::setNonBlockingMode( this->nativeSocket, true );
	int connectResult = ::connect( this->nativeSocket,
	                               (sockaddr*)&remoteAddress,
								   sizeof(sockaddr_in) );
	if( connectResult == NATIVE_SOCKET_ERROR )
	{
		if( Platform::isLastSocketErrorSocketConnecting() )
		{
			// The connection is currently in progress, so wait for it to become available
			// by using select with an appropriate timeout
			timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = timeout * 1000;
			fd_set fdset;
			FD_ZERO( &fdset );
			FD_SET( this->nativeSocket, &fdset );
			
			if( ::select(this->nativeSocket + 1, NULL, &fdset, NULL, &tv) > 0 )
			{
				// Do a final check for any error flags in the socket's option field
				int errorFlag = 0;
				NATIVE_SOCKET_LEN errorLength = sizeof( errorFlag );
				::getsockopt( this->nativeSocket, SOL_SOCKET, SO_ERROR, (char*)(&errorFlag), &errorLength );
				if( errorFlag )
					throw SocketException( Platform::describeLastSocketError() );
			}
			else
			{
				// Select failed, so the connection timed out
				throw SocketTimeoutException( TEXT("Call to connect timed out") );
			}
		}
		else
		{
			// Some other connect error
			throw SocketException( Platform::describeLastSocketError() );
		}
	}

	// If we get to here we are connected! Set the socket back to non-blocking
	Platform::setNonBlockingMode( this->nativeSocket, false );
}

bool Socket::isClosed() const
{
	return this->closed;
}

void Socket::close() throw ( IOException )
{
	if( !isClosed() )
	{
		if( isCreated() )
		{
			// Explicit shutdown on input/output. Calling our own shutdown methods was
			// causing problems in the unit test with exceptions being thrown.
			int shutdownResult = ::shutdown( this->nativeSocket, 0x02 );
			if( shutdownResult != NATIVE_SOCKET_ERROR )
			{
				this->inputShutdown = true;
				this->outputShutdown = true;
			}

			int closeResult = Platform::closeSocket( this->nativeSocket );
			if( closeResult != NATIVE_SOCKET_ERROR )
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

	if( length < 0 )
		throw SocketException( TEXT("Negative length provided to send") );

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int result = ::send( this->nativeSocket, buffer, length, 0 );
	if( result == NATIVE_SOCKET_ERROR )
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

	int result = ::recv( this->nativeSocket, buffer, length, 0 );
	if( result == NATIVE_SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );
	
	return result;
}

NATIVE_IP_ADDRESS Socket::getInetAddress() const
{
	return this->remoteAddress;
}

unsigned short Socket::getPort() const
{
	return this->remotePort;
}

InetSocketAddress Socket::getRemoteSocketAddress() const
{
	return InetSocketAddress( this->remoteAddress, this->remotePort );
}

bool Socket::isInputShutdown() const
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

bool Socket::isOutputShutdown() const
{
	return this->outputShutdown;
}

void Socket::shutdownOutput() throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );

	if( !isConnected() )
		throw SocketException( TEXT("Socket is not connected") );

	if( isOutputShutdown() )
		throw SocketException( TEXT("Socket output is already shutdown") );

	assert( this->nativeSocket != NATIVE_SOCKET_UNINIT );

	int shutdownResult = ::shutdown( this->nativeSocket, 0x00 );
	if( shutdownResult == 0 )
		outputShutdown = true;
	else
		throw SocketException( Platform::describeLastSocketError() );
}

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
Socket* Socket::createFromAccept( NATIVE_SOCKET client, const InetSocketAddress& clientAddress )
{
	// Create a new, connected Socket based around the client's socket descriptor
	Socket* clientSocket = new Socket();
	clientSocket->nativeSocket = client;
	clientSocket->closed = false;
	clientSocket->created = true;
	clientSocket->inputShutdown = false;
	clientSocket->outputShutdown = false;

	clientSocket->remoteAddress = clientAddress.getAddress();
	clientSocket->remotePort = clientAddress.getPort();

	return clientSocket;
}
