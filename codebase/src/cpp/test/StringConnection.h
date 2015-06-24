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

#include <string>
#include "IStringConsumer.h"
#include "syscommon/concurrent/Thread.h"
#include "syscommon/net/InetSocketAddress.h"
#include "syscommon/net/Socket.h"

using namespace std;
using namespace syscommon;

class StringConnection : IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		IStringConsumer* consumer;
		InetSocketAddress endpoint;
		Socket* socket;
		Thread* receiveThread;
		volatile bool running;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		StringConnection( IStringConsumer* consumer, const InetSocketAddress& endpoint );
		StringConnection( IStringConsumer* consumer, Socket* socket );
		virtual ~StringConnection();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void start() throw ( IOException );
		void send( const string& data ) throw ( IOException );
		void interrupt();
		void join();
		bool isRunning() const;

	private:
		void receiveFully( char* buffer, int length ) throw ( IOException );

	public:
		virtual void run();
	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};
