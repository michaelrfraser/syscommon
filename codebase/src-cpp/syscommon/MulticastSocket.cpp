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
#include "MulticastSocket.h"

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
	joinRequest.imr_interface.S_un.S_addr = netIf;
	joinRequest.imr_multiaddr.S_un.S_addr = mcastAddress.getAddress();

	// Request membership to the group
	int joinResult = ::setsockopt( this->nativeSocket, 
								   IPPROTO_IP, 
								   IP_ADD_MEMBERSHIP, 
								   (char*)&joinRequest,
								   sizeof(joinRequest) );
	
	if( joinResult == SOCKET_ERROR )
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
	dropRequest.imr_interface.S_un.S_addr = netIf;
	dropRequest.imr_multiaddr.S_un.S_addr = mcastAddress.getAddress();

	// Request removal from the group
	int dropResult = ::setsockopt( this->nativeSocket,
								   IPPROTO_IP,
								   IP_DROP_MEMBERSHIP,
								   (char*)&dropRequest,
								   sizeof(dropRequest) );

	if( dropResult == SOCKET_ERROR )
		throw SocketException( Platform::describeLastSocketError() );
}

bool MulticastSocket::close()
{
	bool result = false;

	stateLock.lock();
	if ( isCreated() )
	{
		// Attempt to close the socket
		int closeResult = ::closesocket( this->nativeSocket );
		if ( closeResult != SOCKET_ERROR )
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
	size_t readLength = packet.getBufferLength();
	char* readPos = &data[offset];

	// Create a struct to receive the sender's information into
	sockaddr_in from;
	int fromSize = sizeof( from );

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
		packet.setAddress( from.sin_addr.S_un.S_addr );
		packet.setPort( ::ntohs(from.sin_port) );
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
	size_t sendLength = packet.getLength();
	char* sendPos = &data[offset];

	// Create an address struct and populate it with the target information
	sockaddr_in to;
	to.sin_family = AF_INET;
	to.sin_addr.S_un.S_addr = packet.getAddress();
	to.sin_port = ::htons( packet.getPort() );
	int toSize = sizeof( to );

	// Perform the send
	int sendResult = ::sendto( nativeSocket,
							   sendPos,
							   sendLength,
							   0,
							   (struct sockaddr*)&to,
							   sizeof(to) );

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
		// Set SO_REUSEADDR so that other processes can bind to it
		int reuseAddress = 1;
		::setsockopt( this->nativeSocket, 
					  SOL_SOCKET, 
					  SO_REUSEADDR, 
					  (char*)&reuseAddress, 
					  sizeof(reuseAddress) );
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

	// Create an address struct to hold the bind address
	in_addr address;
	address.S_un.S_addr = bindAddress.getAddress();

	// Create a socket address struct to hold the interface information
	sockaddr_in ifaceAddress;
	ifaceAddress.sin_family = AF_INET;
	ifaceAddress.sin_addr = address;
	ifaceAddress.sin_port = ::htons( bindAddress.getPort() );

	// Perform the bind
	int bindResult = ::bind( this->nativeSocket, 
							 (sockaddr*)&ifaceAddress, 
							 sizeof(struct sockaddr) );
	this->bound = bindResult != SOCKET_ERROR;
	
	if( !bound )
		throw SocketException( Platform::describeLastSocketError() );
}