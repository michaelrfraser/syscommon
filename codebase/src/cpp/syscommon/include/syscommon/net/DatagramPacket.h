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

#include "syscommon/Platform.h"
#include "syscommon/net/InetSocketAddress.h"

namespace syscommon
{
	/**
	 * This class represents a datagram packet. 
	 *
	 * Datagram packets are used to implement a connectionless packet delivery service. Each 
	 * message is routed from one machine to another based solely on information contained within 
	 * that packet. Multiple packets sent from one machine to another might be routed differently, 
	 * and might arrive in any order. Packet delivery is not guaranteed.
	 *
	 * Note: buffer pointer passed to the constructor must remain valid for the life of the object
	 */
	class DatagramPacket
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_IP_ADDRESS address;
			unsigned short port;
			char* buffer;
			int offset;
			int length;
			int bufferLength;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Constructs a DatagramPacket for receiving packets of the size length, specifying 
			 * an offset into the buffer.
			 *
			 * @param buffer buffer for holding the incoming datagram.
			 * @param offset the offset for the buffer
			 * @param length the number of bytes to read.
			 */
			DatagramPacket( char* buffer, 
							int offset, 
							int length );

			/**
			 * Constructs a DatagramPacket for receiving packets of size length. 
			 *
			 * @param buffer buffer for holding the incoming datagram.
			 * @param length the number of bytes to read.
			 */
			DatagramPacket( char* buffer, 
							int length );

			/**
			 * Constructs a datagram packet for sending packets of size length with an offset to 
			 * the specified port number on the specified host.
			 *
			 * @param buffer the packet data.
			 * @param offset the packet data offset.
			 * @param length the packet data length.
			 * @param address the destination address.
			 * @param port the destination port number.
			 */
			DatagramPacket( const char* buffer, 
							int offset, 
							int length, 
							NATIVE_IP_ADDRESS address, 
							unsigned short port );

			/**
			 * Constructs a datagram packet for sending packets of size length with an offset to 
			 * the specified port number on the specified host.
			 *
			 * @param buffer the packet data.
			 * @param offset the packet data offset.
			 * @param length the packet data length.
			 * @param address the destination socket address.
			 */
			DatagramPacket( const char* buffer,
							int offset,
							int length,
							InetSocketAddress& address );

			virtual ~DatagramPacket();

		private:
			/**
			 * Internal constructor helper
			 */
			void _DatagramPacket( char* buffer, 
								  int offset, 
								  int length, 
								  NATIVE_IP_ADDRESS address, 
								  unsigned short port );

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/** 
			 * Set the data buffer for this packet. This sets the data pointer, length and offset 
			 * of the packet.
			 *
			 * @param buffer the buffer to set for this packet
			 * @param offset the offset into the data
			 * @param length the length of the data 
			 *       and/or the length of the buffer used to receive data
			 */
			void setData( char* buffer, int offset, int length );

			/**
			 * Sets the IP address of the machine which this datagram is being sent to or has been 
			 * received from
			 *
			 * @param address the IP address of the machine to which this datagram is being sent or 
			 * has been received from
			 */
			void setAddress( NATIVE_IP_ADDRESS address );

			/**
			 * Sets the port number on the remote host which this datagram is being sent or has 
			 * been received from
			 *
			 * @param port the port number
			 */
			void setPort( unsigned short port );

			/**  
			 * Set the length for this packet. The length of the packet is the number of bytes from 
			 * the packet's data buffer that will be sent, or the number of bytes of the packet's 
			 * data buffer that will be used for receiving data. The length must be lesser or
			 * equal to the offset plus the length of the packet's buffer.
			 *
			 * @param length the length to set for this packet.
			 */
			void setLength( int length );

			/**
			 * Returns the data buffer. The data received or the data to be sent
			 * starts from the offset in the buffer, and runs for length long.
			 *
			 * @return the buffer used to receive or send data
			 */
			char* getData();

			/**
			 * Returns the offset of the data to be sent or the offset of the
			 * data received.
			 *
			 * @return the offset of the data to be sent or the offset of the
			 * data received.
			 */
			int getOffset();

			/**
			 * Returns the length of the data to be sent or the length of the
			 * data received.
			 *
			 * @return the length of the data to be sent or the length of the
			 * data received.
			 */
			int getLength();

			/**
			 * Returns the original length of the data buffer of this packet
			 *
			 * @return the original length of the data buffer of this packet
			 */
			int getBufferLength();

			/**
			 * Returns the IP address of the machine to which this datagram is being
			 * sent or from which the datagram was received.
			 *
			 * @return the IP address of the machine to which this datagram is being
			 * sent or from which the datagram was received.
			 */
			NATIVE_IP_ADDRESS getAddress();

			/**
			 * Returns the port number on the remote host to which this datagram is
			 * being sent or from which the datagram was received.
			 *
			 * @return the port number on the remote host to which this datagram is
			 * being sent or from which the datagram was received.
			 */
			unsigned short getPort();

			/**
			 * Gets the InetSocketAddress of the remote host that this packet is being sent to or 
			 * is coming from.
			 */
			InetSocketAddress getSocketAddress();

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------	
	};
}
