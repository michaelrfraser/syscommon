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

#include <set>
#include <string>

#include "StringConnection.h"
#include "IStringConsumer.h"
#include "syscommon/concurrent/Event.h"
#include "syscommon/concurrent/Thread.h"
#include "syscommon/net/InetSocketAddress.h"
#include "syscommon/net/ServerSocket.h"

using namespace std;
using namespace syscommon;

class StringServer : IRunnable, IStringConsumer
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		InetSocketAddress address;
		ServerSocket serverSocket;
		Thread* acceptThread;
		Lock receiveLock;
		Event receiveEvent;
		string lastMessage;
		volatile size_t receiveCount;

		set<StringConnection*> clients;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		StringServer( const InetSocketAddress& address );
		virtual ~StringServer();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		/**
		 * @throws IOException
		 */
		void start();
		void stop();

		bool isStarted() const;
		unsigned short getServerPort();
		NATIVE_IP_ADDRESS getServerAddress() const;

		void registerForNextReceive();
		bool waitForNextReceive( unsigned long timeout );
		string getLastReceivedMessage();
		size_t getReceiveCount();

	public:
		virtual void run();

	public:
		virtual void consume( const string& data, StringConnection* connection );

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

