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

#include "Platform.h"
#include "InetSocketAddress.h"
#include "DatagramPacket.h"
#include "concurrent/Lock.h"
#include "Exception.h"

namespace SysCommon
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
			MulticastSocket() throw ( IOException );

			/**
			 * Creates a multicast socket and binds it to a specific port.
			 *
			 * @param port the port number to bind to
			 */
			MulticastSocket( unsigned short port ) throw ( IOException );

			/**
			 * Creates a MulticastSocket bound to the specified socket address
			 */
			MulticastSocket( const InetSocketAddress& bindAddress ) throw ( IOException );

			virtual ~MulticastSocket();

		private:
			/**
			 * Internal constructor helper
			 */
			void _MulticastSocket( const InetSocketAddress& bindAddress ) throw ( IOException );

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			void setTimeToLive( int ttl ) throw ( IOException );

			/**
			 * Joins a multicast group
			 *
			 * @param mcastaddr the multicast address to join
			 */
			void joinGroup( NATIVE_IP_ADDRESS mcastAddress ) throw ( IOException );

			/**
			 * Joins the specified multicast group at the specified interface.
			 * 
			 * @param mcastaddr the multicast address to join
			 * @param netIf the local interface to receive multicast datagram packets
			 */
			void joinGroup( const InetSocketAddress& mcastAddress, 
							NATIVE_IP_ADDRESS netIf ) 			
				throw ( IOException );

			/**
			 * Leaves a multicast group.
			 * 
			 * @param mcastaddr the multicast address to leave
			 */
			void leaveGroup( NATIVE_IP_ADDRESS mcastAddress ) throw ( IOException );

			/**
			 * Leaves a multicast group on a specified local interface.
			 * 
			 * @param mcastaddr is the multicast address to leave
			 * @param netIf the local interface
			 */
			void leaveGroup( const InetSocketAddress& mcastAddress, 
							 NATIVE_IP_ADDRESS netIf )
				throw ( IOException );

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
			bool isBound();

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
			void receive( DatagramPacket& packet ) throw ( IOException );

			/**
			 * Sends a datagram packet from this socket. The DatagramPacket includes information 
			 * indicating the data to be sent, its length, the IP address of the remote host,
			 * and the port number on the remote host.
			 * 
			 * @param packet the DatagramPacket to be sent.
			 *
			 * @throw IOException if an I/O error occurs while sending the packet
			 */
			void send( DatagramPacket& packet ) throw ( IOException );

		private:
			bool isCreated();
			void create() throw ( IOException );
			void bind( const InetSocketAddress& address ) throw ( IOException );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}
