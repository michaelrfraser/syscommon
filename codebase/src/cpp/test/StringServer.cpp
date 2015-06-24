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
#include "StringServer.h"
#include <stdio.h>

#ifdef DEBUG
#include "debug.h"
#endif

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
StringServer::StringServer( const InetSocketAddress& address )
	: address( address ), serverSocket(), receiveEvent( false, "ReceiveEvent" )
{
	this->acceptThread = NULL;
	this->receiveCount = 0;
}

StringServer::~StringServer()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void StringServer::start() throw ( IOException )
{
	if( !this->acceptThread )
	{
		// Create the server socket
		this->serverSocket.bind( this->address );

		// Start the acceptor in a new thread
		this->acceptThread = new Thread( this, TEXT("StringServerAcceptor") );
		this->acceptThread->start();
	}
}

void StringServer::stop()
{
	if( this->acceptThread )
	{
		this->serverSocket.close();

		this->acceptThread->join();
		delete this->acceptThread;
		this->acceptThread = NULL;

		receiveLock.lock();
		set<StringConnection*>::iterator it = this->clients.begin();
		while( it != this->clients.end() )
			(*it++)->interrupt();
		receiveLock.unlock();

		it = this->clients.begin();
		while( it != this->clients.end() )
		{
			StringConnection* connection = *it++;
			connection->join();
			delete connection;
		}

		this->clients.clear();
	}
}

bool StringServer::isStarted() const
{
	return this->acceptThread != NULL;
}

unsigned short StringServer::getServerPort()
{
	return this->serverSocket.getLocalPort();
}

NATIVE_IP_ADDRESS StringServer::getServerAddress() const
{
	return this->serverSocket.getInetAddress();
}

void StringServer::registerForNextReceive()
{
	this->receiveEvent.clear();
}

bool StringServer::waitForNextReceive( unsigned long timeout )
{
	WaitResult result = this->receiveEvent.waitFor( timeout );
	return result == WR_SUCCEEDED;
}

string StringServer::getLastReceivedMessage()
{
	string message;

	this->receiveLock.lock();
	message = this->lastMessage;
	this->receiveLock.unlock();

	return message;
}

size_t StringServer::getReceiveCount()
{
	size_t count = 0;

	this->receiveLock.lock();
	count = this->receiveCount;
	this->receiveLock.unlock();

	return count;
}

void StringServer::run()
{
	while( true )
	{
		try
		{
			Socket* clientSocket = this->serverSocket.accept();
			StringConnection* clientConnection = new StringConnection( this, clientSocket );
			this->clients.insert( clientConnection );

			clientConnection->start();
		}
		catch( IOException& )
		{
			// Shutdown!
			break;
		}
	}
}

void StringServer::consume( const string& data, StringConnection* connection )
{
	receiveLock.lock();
	++this->receiveCount;
	this->lastMessage = data;
	this->receiveEvent.signal();

	if( connection->isRunning() )
		connection->send( data );
	receiveLock.unlock();
}
