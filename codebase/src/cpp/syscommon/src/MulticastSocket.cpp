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
#include "net/MulticastSocket.h"

#include <cstring>
#include "assert.h"

using namespace SysCommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
MulticastSocket::MulticastSocket() throw ( IOException )
{
	InetSocketAddress bindAddress( 0 );
	_MulticastSocket( bindAddress );
}

MulticastSocket::MulticastSocket( unsigned short port ) throw ( IOException )
{
	InetSocketAddress bindAddress( port );
	_MulticastSocket( bindAddress );
}

MulticastSocket::MulticastSocket( const InetSocketAddress& bindAddress ) throw ( IOException )
{
	_MulticastSocket( bindAddress );
}

MulticastSocket::~MulticastSocket()
{
	// Initialised in _MulticastSocket
	Platform::cleanupSocketFramework();
}

void MulticastSocket::_MulticastSocket( const InetSocketAddress& bindAddress )  
	throw ( IOException )
{
	// Uninitialised in ~MulticastSocket
	Platform::initialiseSocketFramework();
	bound = false;
	this->nativeSocket = NATIVE_SOCKET_UNINIT;

	// Create the socket and bind it to the appropriate address
	create();
	bind( bindAddress );
}

void MulticastSocket::setTimeToLive( int ttl ) throw ( IOException )
{
	if( !isCreated() )
		throw SocketException( TEXT("Socket is closed") );

	// Set a default multicast TTL
	::setsockopt( this->nativeSocket,
				  IPPROTO_IP,
				  IP_MULTICAST_TTL,
				  (char*)&ttl,
				  sizeof(int) );
}

void MulticastSocket::joinGroup( NATIVE_IP_ADDRESS mcastAddress )  throw ( IOException )
{
	InetSocketAddress address( mcastAddress, 0 );
	joinGroup( address, INADDR_ANY );
}

void MulticastSocket::joinGroup( const InetSocketAddress& mcastAddress, 
								 NATIVE_IP_ADDRESS netIf )  
	throw ( IOException )
{
	if( !isCreated() )
		throw SocketException( TEXT("Socket is closed") );

	// TODO: Check validity of mcastAddress

	// Construct the join request
	ip_mreq joinRequest;
	::memset( &joinRequest, 0, sizeof(ip_mreq) );
	joinRequest.imr_interface.s_addr = htonl(netIf);
	joinRequest.imr_multiaddr.s_addr = htonl(mcastAddress.getAddress());

	// Request membership to the group
	int joinResult = ::setsockopt( this->nativeSocket, 
								   IPPROTO_IP, 
								   IP_ADD_MEMBERSHIP, 
								   (char*)&joinRequest,
								   sizeof(joinRequest) );
	
	if( joinResult == NATIVE_SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );
}

void MulticastSocket::leaveGroup( NATIVE_IP_ADDRESS mcastAddress ) throw ( IOException )
{
	InetSocketAddress address( mcastAddress, 0 );
	leaveGroup( address, INADDR_ANY );
}

void MulticastSocket::leaveGroup( const InetSocketAddress& mcastAddress, 
								  NATIVE_IP_ADDRESS netIf )
	throw ( IOException )
{
	if( !isCreated() )
		throw SocketException( TEXT("Socket is closed") );

	// Construct the drop request
	ip_mreq dropRequest;
	::memset( &dropRequest, 0, sizeof(ip_mreq) );
	dropRequest.imr_interface.s_addr = htonl(netIf);
	dropRequest.imr_multiaddr.s_addr = htonl(mcastAddress.getAddress());

	// Request removal from the group
	int dropResult = ::setsockopt( this->nativeSocket,
								   IPPROTO_IP,
								   IP_DROP_MEMBERSHIP,
								   (char*)&dropRequest,
								   sizeof(dropRequest) );

	if( dropResult == NATIVE_SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );
}

bool MulticastSocket::close()
{
	bool result = false;

	stateLock.lock();
	if ( isCreated() )
	{
		// Shutdown input and output
		::shutdown( this->nativeSocket, 0x00 );
		::shutdown( this->nativeSocket, 0x01 );

		// Attempt to close the socket
		int closeResult = Platform::closeSocket( this->nativeSocket );
		if ( closeResult != NATIVE_SOCKET_ERROR )
		{
			this->nativeSocket = NATIVE_SOCKET_UNINIT;
			bound = false;
			result = true;
		}
	}
	stateLock.unlock();

	return result;
}

bool MulticastSocket::isBound()
{
	return this->bound;
}

void MulticastSocket::receive( DatagramPacket& packet ) throw ( IOException )
{
	if( !isCreated() )
		throw SocketException( TEXT("Socket is closed") );

	char* data = packet.getData();
	assert( data );

	// Calculate the pointer within the data buffer that we should receive into
	size_t offset = packet.getOffset();
	int readLength = packet.getBufferLength();
	char* readPos = &data[offset];

	// Create a struct to receive the sender's information into
	sockaddr_in from;
	NATIVE_SOCKET_LEN fromSize = sizeof( from );

	// Do the receive
	int recvResult = ::recvfrom( nativeSocket, 
								 readPos, 
								 readLength, 
								 0, 
								 (struct sockaddr*)&from, 
								 &fromSize );
	if( recvResult >= 0 )
	{
		// Update the packet's length member
		packet.setLength( recvResult );

		// Update the packet with the sender's information
		packet.setAddress( ntohl(from.sin_addr.s_addr) );
		packet.setPort( ntohs(from.sin_port) );
	}
	else
	{
		throw SocketException( Platform::describeLastSocketError() );
	}
}

void MulticastSocket::send( DatagramPacket& packet ) throw ( IOException )
{
	if( !isBound() )
		throw SocketException( TEXT("Socket is not bound") );

	if( packet.getAddress() == INADDR_NONE )
		throw SocketException( TEXT("Destination address in datagram packet is empty") );

	char* data = packet.getData();
	assert( data );

	// Calculate the pointer within the data buffer that we should send from
	size_t offset = packet.getOffset();
	int sendLength = packet.getLength();
	char* sendPos = &data[offset];

	// Create an address struct and populate it with the target information
	sockaddr_in to;
	NATIVE_IP_ADDRESS toAddress = packet.getAddress();
	unsigned short toPort = packet.getPort();

	::memset( &to, 0, sizeof(sockaddr_in) );
	to.sin_family = AF_INET;
	to.sin_addr.s_addr = htonl( toAddress );
	to.sin_port = htons( toPort );

	// Perform the send
	int sendResult = ::sendto( nativeSocket,
							   sendPos,
							   sendLength,
							   0,
							   (struct sockaddr*)&to,
							   (int)sizeof(to) );

	//int sendResult = ::send( nativeSocket, sendPos, sendLength, 0 );

	if( sendResult < 0 )
		throw SocketException( Platform::describeLastSocketError() );
}

bool MulticastSocket::isCreated()
{
	bool created = false;

	stateLock.lock();
	created = this->nativeSocket != NATIVE_SOCKET_UNINIT;
	stateLock.unlock();

	return created;
}

void MulticastSocket::create() throw ( IOException )
{
	if( isCreated() )
		throw SocketException( TEXT("Socket already created") );

	// Create the socket
	this->nativeSocket = ::socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
	if( this->nativeSocket != NATIVE_SOCKET_UNINIT )
	{
		Platform::setMulticastSocketOptions( this->nativeSocket );

		setTimeToLive( DEFAULT_TTL );
	}
	else
	{
		throw SocketException( Platform::describeLastSocketError() );
	}
}


void MulticastSocket::bind( const InetSocketAddress& bindAddress ) throw ( IOException )
{
	if( !isCreated() )
		throw SocketException( TEXT("Socket is closed") );

	if( isBound() )
		throw SocketException( TEXT("Socket is already bound") );

	// Create a socket address struct to hold the interface information
	sockaddr_in ifaceAddress;
	::memset( &ifaceAddress, 0, sizeof(sockaddr_in) );
	ifaceAddress.sin_family = AF_INET;
	ifaceAddress.sin_addr.s_addr = htonl( bindAddress.getAddress() );
	ifaceAddress.sin_port = htons( bindAddress.getPort() );

	// Perform the bind
	int bindResult = ::bind( this->nativeSocket, 
							 (sockaddr*)&ifaceAddress, 
							 sizeof(struct sockaddr) );
	this->bound = bindResult != NATIVE_SOCKET_ERROR;
	
	if( !bound )
		throw SocketException( Platform::describeLastSocketError() );
}
