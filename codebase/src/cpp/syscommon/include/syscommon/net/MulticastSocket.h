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

#include "syscommon/Exception.h"
#include "syscommon/Platform.h"
#include "syscommon/concurrent/Lock.h"
#include "syscommon/net/DatagramPacket.h"
#include "syscommon/net/InetSocketAddress.h"

namespace syscommon
{
	/**
	 * A MulticastSocket is a UDP Datagram Socket, with capabilities for joining "groups" of other 
	 * multicast hosts on the internet.
	 *
	 * A multicast group is specified by a class D IP address and by a standard UDP port number. 
	 * Class D IP addresses are in the range 224.0.0.0 to 239.255.255.255, inclusive. The address 
	 * 224.0.0.0 is reserved and should not be used.
	 *
	 * To join a multicast group firstly create a MulticastSocket with the desired port, and then 
	 * call the joinGroup(NATIVE_IP_ADDRESS mcastAddress) method.
	 *
	 * When sending a message to a multicast group, all subscribing recipients to that host and 
	 * port receive the message (within the time-to-live range of the packet). The socket does not
	 * need to be a member of the multicast group to send messages to it.
	 *
	 * Multicast sockets are created with the SO_REUSEADDR socket option set
	 */
	class MulticastSocket
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		static const int DEFAULT_TTL = 255;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_SOCKET nativeSocket;
			Lock stateLock;
			bool bound;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * @throws IOException
			 */
			MulticastSocket() noexcept( false );

			/**
			 * Creates a multicast socket and binds it to a specific port.
			 *
			 * @param port the port number to bind to
			 * @throws IOException
			 */
			MulticastSocket( unsigned short port ) noexcept( false );

			/**
			 * Creates a MulticastSocket bound to the specified socket address
			 *
			 * @throws IOException
			 */
			MulticastSocket( const InetSocketAddress& bindAddress ) noexcept( false );

			virtual ~MulticastSocket();

		private:
			/**
			 * Internal constructor helper
			 *
			 * @throws IOException
			 */
			void _MulticastSocket( const InetSocketAddress& bindAddress ) noexcept( false );

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * @throws IOException
			 */
			void setTimeToLive( int ttl ) noexcept( false );

			/**
			 * Joins a multicast group
			 *
			 * @param mcastaddr the multicast address to join
			 * @throws IOException
			 */
			void joinGroup( NATIVE_IP_ADDRESS mcastAddress ) noexcept( false );

			/**
			 * Joins the specified multicast group at the specified interface.
			 * 
			 * @param mcastaddr the multicast address to join
			 * @param netIf the local interface to receive multicast datagram packets
			 * @throws IOException
			 */
			void joinGroup( const InetSocketAddress& mcastAddress, 
							NATIVE_IP_ADDRESS netIf )
				noexcept( false );

			/**
			 * Leaves a multicast group.
			 * 
			 * @param mcastaddr the multicast address to leave
			 * @throws IOException
			 */
			void leaveGroup( NATIVE_IP_ADDRESS mcastAddress ) noexcept( false );

			/**
			 * Leaves a multicast group on a specified local interface.
			 * 
			 * @param mcastaddr is the multicast address to leave
			 * @param netIf the local interface
			 * @throws IOException
			 */
			void leaveGroup( const InetSocketAddress& mcastAddress, 
							 NATIVE_IP_ADDRESS netIf )
				noexcept( false );

			/**
			 * Closes this multicast socket.
			 * 
			 * Any blocking calls to receive() will return a value of false to their threads
			 */
			bool close();
			
			/**
			 * Returns the bind state of the socket.
			 *
			 * @return true if the socket is succesfuly bound to an address
			 */
			bool isBound() const;

			/**
			 * Receives a datagram packet from this socket. When this method returns, the 
			 * DatagramPacket's buffer is filled with the data received. The datagram packet will 
			 * also be populated with the sender's IP address, and the port number on the sender's 
			 * machine.
			 * 
			 * This method blocks until a datagram is received. The length field of the datagram 
			 * packet object will contain the length of the received message. If the message is 
			 * longer than the packet's length, the message is truncated.
			 * 
			 * @param packet the DatagramPacket into which to place the incoming data.
			 *
			 * @throw IOException if the socket was closed before a packet could be received
			 */
			void receive( DatagramPacket& packet ) noexcept( false );

			/**
			 * Sends a datagram packet from this socket. The DatagramPacket includes information 
			 * indicating the data to be sent, its length, the IP address of the remote host,
			 * and the port number on the remote host.
			 * 
			 * @param packet the DatagramPacket to be sent.
			 *
			 * @throw IOException if an I/O error occurs while sending the packet
			 */
			void send( DatagramPacket& packet ) noexcept( false );

		private:
			bool isCreated();
			/**
			 * @throws IOException
			 */
			void create() noexcept( false );
			/**
			 * @throws IOException
			 */
			void bind( const InetSocketAddress& address ) noexcept( false );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}
