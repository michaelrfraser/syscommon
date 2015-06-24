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
#include "syscommon/net/ServerSocket.h"

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
ServerSocket::ServerSocket()
{
	_ServerSocket();
}

ServerSocket::ServerSocket( unsigned short port ) throw ( IOException )
{
	_ServerSocket();
	this->bind( InetSocketAddress(port) );
}

ServerSocket::ServerSocket( unsigned short port, int backlog ) throw ( IOException )
{
	_ServerSocket();
	this->bind( InetSocketAddress(port), backlog );
}

ServerSocket::ServerSocket( unsigned short port, 
			                int backlog, 
						    NATIVE_IP_ADDRESS bindAddr) throw ( IOException )
{
	_ServerSocket();
	this->bind( InetSocketAddress(bindAddr, port), backlog );
}

ServerSocket::~ServerSocket()
{
	Platform::cleanupSocketFramework();
}

void ServerSocket::_ServerSocket()
{
	Platform::initialiseSocketFramework();
	this->nativeSocket = NATIVE_SOCKET_UNINIT;
	this->boundTo = INADDR_NONE;
	this->closed = false;
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void ServerSocket::bind( const InetSocketAddress& endpoint ) throw ( IOException )
{
	this->bind( endpoint, DEFAULT_BACKLOG );
}

void ServerSocket::bind( const InetSocketAddress& endpoint, 
			             int backlog ) 
	throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );
	if( isBound() )
		throw SocketException( TEXT("Socket is already bound") );

	if( backlog < 1 )
		backlog = DEFAULT_BACKLOG;

	// Get the port in network order
	unsigned short networkOrderPort = htons( endpoint.getPort() );
	NATIVE_IP_ADDRESS networkOrderAddress = htonl( endpoint.getAddress() );

	// Get the address in network order
	sockaddr_in myAddress;
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.s_addr = networkOrderAddress;
	myAddress.sin_port = networkOrderPort;

	NATIVE_SOCKET impl = getImpl();
	int bindResult = ::bind( impl, (sockaddr*)&myAddress, sizeof(sockaddr_in) );
	if( bindResult != NATIVE_SOCKET_ERROR )
	{
		int listenResult = ::listen( impl, backlog );
		if( listenResult != NATIVE_SOCKET_ERROR )
		{
			this->boundTo = endpoint.getAddress();
		}
		else
		{
			this->boundTo = INADDR_NONE;
			throw SocketException( Platform::describeLastSocketError() );
		}
	}
	else
	{
		this->boundTo = INADDR_NONE;
		throw SocketException( Platform::describeLastSocketError() );
	}
}

NATIVE_IP_ADDRESS ServerSocket::getInetAddress() const
{
	return this->boundTo;
}

unsigned short ServerSocket::getLocalPort()
{
	if( !isBound() )
		return 0;

	NATIVE_SOCKET impl = getImpl();

	sockaddr_in myAddress;
	NATIVE_SOCKET_LEN addrLen = sizeof(myAddress);
	int sockNameResult = ::getsockname( impl, (sockaddr*)&myAddress, &addrLen );
	if( sockNameResult != NATIVE_SOCKET_ERROR )
		return ntohs( myAddress.sin_port );
	else
		return 0;
}

Socket* ServerSocket::accept() throw ( IOException )
{
	if( isClosed() )
		throw SocketException( TEXT("Socket is closed") );
	if( !isBound() )
		throw SocketException( TEXT("Socket is not bound yet") );

	NATIVE_SOCKET impl = getImpl();
	sockaddr_in clientAddress;
	NATIVE_SOCKET_LEN addrLen = sizeof(clientAddress);
	NATIVE_SOCKET acceptResult = ::accept( impl, (sockaddr*)&clientAddress, &addrLen );
	if( acceptResult != NATIVE_SOCKET_ERROR )
	{
		NATIVE_IP_ADDRESS clientIp = ntohl( clientAddress.sin_addr.s_addr );
		unsigned short clientPort = ntohs( clientAddress.sin_port );
		return Socket::createFromAccept( acceptResult, InetSocketAddress(clientIp, clientPort) );
	}
	else
	{
		throw SocketException( Platform::describeLastSocketError() );
	}
}

void ServerSocket::close() throw ( IOException )
{
	NATIVE_SOCKET impl = getImpl();
	this->closeLock.lock();
	if( !isClosed() )
	{
		if( impl != NATIVE_SOCKET_UNINIT )
		{
			// Attempt to shutdown input and output. Required on linux, otherwise
			// calls to accept won't wake up :(
			::shutdown( impl, 0x02 );

			int closeResult = Platform::closeSocket( impl );
			if( closeResult != NATIVE_SOCKET_ERROR )
			{
				this->closed = true;
			}
			else
			{
				// Unable to close, so unlock and throw an exception
				this->closeLock.unlock();
				throw SocketException( Platform::describeLastSocketError() );
			}
		}
	}

	this->closeLock.unlock();
}

bool ServerSocket::isBound() const
{
	return this->boundTo != INADDR_NONE;
}

bool ServerSocket::isClosed()
{
	bool localClosed = false;

	this->closeLock.lock();
	localClosed = this->closed;
	this->closeLock.unlock();

	return localClosed;
}

NATIVE_SOCKET ServerSocket::getImpl() throw ( SocketException )
{
	if( this->nativeSocket == NATIVE_SOCKET_UNINIT )
	{
		this->nativeSocket = ::socket( AF_INET, SOCK_STREAM, 0 );

		// If the native socket failed to create, then throw an exception
		if( this->nativeSocket == NATIVE_SOCKET_UNINIT )
			throw SocketException( Platform::describeLastSocketError() );
	}

	return this->nativeSocket;
}
