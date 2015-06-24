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
#include "syscommon/net/InetSocketAddress.h"
#include "syscommon/net/Socket.h"

namespace syscommon
{
	/**
	 * This class implements server sockets. A server socket waits for requests to come in over the 
	 * network. It performs some operation based on that request, and then possibly returns a 
	 * result to the requester.
	 */
	class ServerSocket
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		public:
			static const int DEFAULT_BACKLOG = 50;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_SOCKET nativeSocket;
			bool closed;
			NATIVE_IP_ADDRESS boundTo;

			Lock closeLock;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates an unbound server socket.
			 */
			ServerSocket();

			/**
			 * Creates a server socket, bound to the specified port. A port number of 
			 * <code>0</code> means that the port number is automatically allocated, typically 
			 * from an ephemeral port range. This port number can then be retrieved by calling 
			 * {@link #getLocalPort getLocalPort}.
			 * <p>
			 * The maximum queue length for incoming connection indications (a request to connect) 
			 * is set to <code>50</code>. If a connection indication arrives when the queue is 
			 * full, the connection is refused.
			 *
			 * @param      port  the port number, or <code>0</code> to use a port
			 *                   number that is automatically allocated.
			 *
			 * @exception  IOException  if an I/O error occurs when opening the socket.
			 */
			ServerSocket( unsigned short port ) throw ( IOException );

			/**
			 * Creates a server socket and binds it to the specified local port number, with the 
			 * specified backlog. A port number of <code>0</code> means that the port number is
			 * automatically allocated, typically from an ephemeral port range. This port number 
			 * can then be retrieved by calling {@link #getLocalPort getLocalPort}.
			 * <p>
			 * The maximum queue length for incoming connection indications (a request to connect) 
			 * is set to the <code>backlog</code> parameter. If a connection indication arrives 
			 * when the queue is full, the connection is refused.
			 * <p>
			 * The <code>backlog</code> argument is the requested maximum number of pending 
			 * connections on the socket. The operating system usually places a cap on this value.
			 *
			 * @param      port     the port number, or <code>0</code> to use a port
			 *                      number that is automatically allocated.
			 * @param      backlog  requested maximum length of the queue of incoming
			 *                      connections.
			 *
			 * @exception  IOException  if an I/O error occurs when opening the socket.
			 */
			ServerSocket( unsigned short port, int backlog ) throw ( IOException );

			/**
			 * Create a server with the specified port, listen backlog, and local IP address to 
			 * bind to. The <i>bindAddr</i> argument can be used on a multi-homed host for a 
			 * ServerSocket that will only accept connect requests to one of its addresses. If 
			 * <i>bindAddr</i> is null, it will default accepting connections on any/all local 
			 * addresses.
			 * <p> 
			 * The port must be between 0 and 65535, inclusive. A port number of <code>0</code> 
			 * means that the port number is automatically allocated, typically from an ephemeral 
			 * port range. This port number can then be retrieved by calling 
			 * {@link #getLocalPort getLocalPort}.
			 * <p>
			 * The <code>backlog</code> argument is the requested maximum number of pending 
			 * connections on the socket. The operating system usually places a cap on this value.
			 *
			 * @param port  the port number, or <code>0</code> to use a port
			 *              number that is automatically allocated.
			 * @param backlog requested maximum length of the queue of incoming
			 *                connections.
			 * @param bindAddr the local InetAddress the server will bind to
			 *
			 * @throws  IOException if an I/O error occurs when opening the socket.
			 */
			ServerSocket( unsigned short port, 
			              int backlog, 
						  NATIVE_IP_ADDRESS bindAddr ) throw ( IOException );

			virtual ~ServerSocket();

		private:
			/**
			 * Internal constructor helper
			 */
			void _ServerSocket();


		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 *
			 * Binds the <code>ServerSocket</code> to a specific address (IP address and port 
			 * number).
			 *
			 * @param   endpoint        The IP address & port number to bind to.
			 * @throws  IOException if the bind operation fails, or if the socket is already bound.
			 */
			void bind( const InetSocketAddress& endpoint ) throw ( IOException );

			/**
			 * Binds the <code>ServerSocket</code> to a specific address (IP address and port 
			 * number).
			 * <p>
			 * The <code>backlog</code> argument is the requested maximum number of pending 
			 * connections on the socket. The operating system usually places a cap on this value. 
			 * If it is less than, or equal to <code>0</code>, then an implementation 
			 * specific default will be used.
			 * @param   endpoint        The IP address & port number to bind to.
			 * @param   backlog         requested maximum length of the queue of
			 *                          incoming connections.
			 * @throws  IOException if the bind operation fails, or if the socket
			 *                     is already bound.
			 */
			void bind( const InetSocketAddress& endpoint, 
			           int backlog ) 
				throw ( IOException );

			/**
			 * Returns the local address of this server socket.
			 * <p>
			 * If the socket was bound prior to being {@link #close closed}, then this method will 
			 * continue to return the local address after the socket is closed.
			 *
			 * @return  the address to which this socket is bound,
			 *          or <code>INADDR_NONE</code> if the socket is unbound.
			 */
			NATIVE_IP_ADDRESS getInetAddress() const;

			/**
			 * Returns the port number on which this socket is listening.
			 * <p>
			 * If the socket was bound prior to being {@link #close closed}, then this method will 
			 * continue to return the port number after the socket is closed.
			 *
			 * @return  the port number to which this socket is listening or 0 if the socket is not 
			 * bound yet.
			 */
			unsigned short getLocalPort();

			/**
			 * Listens for a connection to be made to this socket and accepts it. The method blocks 
			 * until a connection is made.
			 * <p>
			 * <b>Memory Management:</b> The caller is responsible for freeing the Socket allocated
			 * by this function when it is no longer required
			 *
			 * @exception IOException  if an I/O error occurs when waiting for a connection.
			 *
			 * @return the new Socket
			 */
			Socket* accept() throw ( IOException );

			/**
			 * Closes this socket.
			 *
			 * Any thread currently blocked in {@link #accept()} will throw a 
			 * {@link SocketException}.
			 *
			 * @exception  IOException  if an I/O error occurs when closing the socket.
			 */
			void close() throw ( IOException );		

			/**
			 * Returns the binding state of the ServerSocket.
			 *
			 * @return true if the ServerSocket succesfuly bound to an address
			 */
			bool isBound() const;

			/**
			 * Returns the closed state of the ServerSocket.
			 *
			 * @return true if the socket has been closed
			 */
			bool isClosed();

		private:
			NATIVE_SOCKET getImpl() throw ( SocketException );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------

	};
}
