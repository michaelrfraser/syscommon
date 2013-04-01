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
#include "Exception.h"

namespace SysCommon
{
	/**
	 * This class implements client sockets (also called just "sockets"). A socket is an endpoint 
	 * for communication between two machines.
	 */
	class Socket
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_SOCKET nativeSocket;
			bool created;
			bool connected;
			bool closed;
			bool inputShutdown;
			bool outputShutdown;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates an unconnected socket
			 */
			Socket();

			/**
			 * Creates a stream socket and connects it to the specified port number at the 
			 * specified IP address.
			 * 
			 * @param address the IP address.
			 * @param port the port number.
			 *
			 * @throws IOException if an I/O error occurs when creating the socket.
			 */
			Socket( NATIVE_IP_ADDRESS address, unsigned short port ) throw ( IOException );

			virtual ~Socket();

		private:
			/**
			 * Internal constructor helper
			 */
			void _Socket();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Returns the connection state of the socket.
			 *
			 * @return true if the socket successfuly connected to a server
			 */
			bool isConnected();

			/**
			 * Connects this socket to the specified server.
			 *
			 * @param	endpoint the InetSocketAddress of the server to connect to
			 * @throws	IOException if an error occurs during the connection
			 */
			void connect( InetSocketAddress& endpoint ) throw ( IOException );

			/**
			 * Returns the closed state of the socket.
			 *
			 * @return true if the socket has been closed
			 */
			bool isClosed();

			/**
			 * Closes this socket.
			 *
			 * Any thread currently blocked in an I/O operation upon this socket will throw a 
			 * SocketException.
			 * 
			 * Once a socket has been closed, it is not available for further networking use (i.e. 
			 * can't be reconnected or rebound). A new socket needs to be created.
			 *
			 * @exception IOException if an I/O error occurs when closing this socket.
			 */
			void close() throw ( IOException );
			
			/**
			 * Sends the bytes contained in the specified buffer to the server this socket is
			 * connected to.
			 *
			 * @param buffer The buffer of bytes to send through the socket
			 * @param length The amount of bytes to send
			 *
			 * @return the number of bytes sent by this operation
			 *
			 * @throws IOException if an I/O error occurs while attempting to send the data
			 * through the socket
			 */
			int send( const char* buffer, int length ) throw ( IOException );

			/**
			 * Receives data from the socket into the provided buffer
			 *
			 * @param buffer The buffer of bytes to receive into
			 * @param length The maximum amount of bytes to receive
			 *
			 * @return the number of bytes received by this receive operation
			 *
			 * @throws IOException if an I/O error occurs while attempting to send the data through
			 * the socket
			 */
			int receive( char* buffer, int length ) throw ( IOException );

			/**
			 * Returns whether the read-half of the socket connection is closed.
			 *
			 * @return true if the input of the socket has been shutdown
			 */
			bool isInputShutdown();

			/**
			 * Shuts down this socket's input handle
			 *
			 * Any pending sent by the remove server to the input of this socket is acknowledged 
			 * and then silently discarded.
			 *
			 * Any subsequent calls to receive() after the input handle has been closed will result
			 * in an IOException being thrown.
			 *
			 * @exception IOException if an I/O error occurs when shutting down this socket.
			 */
			void shutdownInput() throw ( IOException );

			/**
			 * Returns whether the write-half of the socket connection is closed.
			 *
			 * @return true if the output of the socket has been shutdown
			 */
			bool isOutputShutdown();

			/**
			 * Disables the output stream for this socket. For a TCP socket, any previously written 
			 * data will be sent followed by TCP's normal connection termination sequence.
			 *
			 * Subsequent calls to send() after shutdownOutput() has been called will result in
			 * an IOException being thrown
			 *
			 * @exception IOException if an I/O error occurs when shutting down this
			 * socket.
			 */
			void shutdownOutput() throw ( IOException );

		private:
			bool isCreated();
			void create() throw ( SocketException );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}