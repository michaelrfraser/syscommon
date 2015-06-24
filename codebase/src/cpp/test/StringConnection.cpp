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
#include "StringConnection.h"
#include "syscommon/io/OutputBuffer.h"

using namespace syscommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
StringConnection::StringConnection( IStringConsumer* consumer, const InetSocketAddress& endpoint )
	: endpoint( endpoint )
{
	this->socket = NULL;
	this->consumer = consumer;
	this->receiveThread = NULL;
	this->running = false;
}

StringConnection::StringConnection( IStringConsumer* consumer, Socket* socket )
	: endpoint( socket->getRemoteSocketAddress() )
{
	this->socket = socket;
	this->consumer = consumer;
	this->receiveThread = NULL;
	this->running = false;
}

StringConnection::~StringConnection()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void StringConnection::start() throw ( IOException )
{
	if( !this->receiveThread )
	{
		if( !this->consumer )
			throw IOException( "consumer is NULL" );

		// If the socket isn't already connected, then connect
		if( !this->socket  )
		{
			this->socket = new Socket( this->endpoint.getAddress(), 
			                           this->endpoint.getPort() );
		}

		// Spawn the client thread and start it
		this->running = true;
		this->receiveThread = new Thread( this, "StringConnectionReceive" );
		this->receiveThread->start();
	}
}

void StringConnection::send( const string& data ) throw ( IOException )
{ 
	if( this->running )
	{
		size_t messageLength = data.length();
		const char* message = data.data();
		this->socket->send( (char*)&messageLength, sizeof(size_t) );
		this->socket->send( message, messageLength );
	}
}

#include <stdio.h>
void StringConnection::interrupt()
{
	if( this->receiveThread )
	{
		this->running = false;

		try
		{
			this->socket->close();
		}
		catch( IOException& ioe )
		{
			printf( "Error on socket interrupt: %s\n", ioe.what() );
			// Log an error?
		}
	}
}

void StringConnection::join()
{
	if( this->receiveThread )
	{
		try
		{
			this->receiveThread->join();
		}
		catch( InterruptedException& )
		{
			// Log an error?
		}

		this->receiveThread = NULL;
		delete this->socket;
		this->socket = NULL;
	}
}

void StringConnection::receiveFully( char* buffer, int length ) throw ( IOException )
{
	int totalReceived = 0;
	while( totalReceived < length )
	{
		int leftToReceive = length - totalReceived;
		char* cursor = &buffer[totalReceived];
		int received = this->socket->receive( cursor, leftToReceive );

		totalReceived += received;
	}
}

bool StringConnection::isRunning() const
{
	return this->running;
}

void StringConnection::run()
{
	char* messageData = NULL;

	while( this->running )
	{
		try
		{
			size_t length = 0;
			this->receiveFully( (char*)&length, sizeof(size_t) );
			
			messageData = new char[length];
			this->receiveFully( messageData, length );

			string message( messageData, length );
			this->consumer->consume( message, this );
			delete [] messageData;
			messageData = NULL;
		}
		catch( IOException& )
		{
			if( !this->running )
			{
				// Normal shutdown
			}
			else
			{
				// Badness happened
			}

			if( messageData )
			{
				delete [] messageData;
				messageData = NULL;
			}

			break;
		}
	}

	this->running = false;
}
