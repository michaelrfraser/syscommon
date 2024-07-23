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
#include "SocketTest.h"
#include "StringServer.h"
#include "syscommon/Platform.h"
#include "syscommon/net/Socket.h"

#ifdef DEBUG
#include "debug.h"
#endif

CPPUNIT_TEST_SUITE_REGISTRATION( SocketTest );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SocketTest, "SocketTest" );

using namespace std;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
SocketTest::SocketTest()
{
	this->socket = NULL;
	this->server = NULL;
}

SocketTest::~SocketTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void SocketTest::setUp()
{
	this->server = quickCreateServer( INADDR_LOOPBACK, 1234 );
	this->socket = NULL;
}

void SocketTest::tearDown()
{
	if( this->socket && this->socket->isConnected() )
		this->socket->close();

	quickReleaseServer( this->server );
	this->server = NULL;
	
	if( this->socket )
	{
		delete this->socket;
		this->socket = NULL;
	}
}

void SocketTest::testLifeCycle()
{
	this->socket = new Socket();

	// Socket should be in an unconnected state
	CPPUNIT_ASSERT( !this->socket->isConnected() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_NONE );
	CPPUNIT_ASSERT( this->socket->getPort() == 0 );
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isClosed() );

	// Connect the socket and validate it is in the correct startup state
	this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234) );
	CPPUNIT_ASSERT( this->socket->isConnected() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isClosed() );

	// Disconnect the socket and validate it is in the correct closed state
	this->socket->close();
	CPPUNIT_ASSERT( this->socket->isConnected() ); // Has connected previously (That's how Java does it)
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK ); // Should hold references to address and port
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );
	CPPUNIT_ASSERT( this->socket->isClosed() );
}

void SocketTest::testDefaultConstructor()
{
	// Socket should be created in an unconnected state
	this->socket = new Socket();
	CPPUNIT_ASSERT( !this->socket->isConnected() );
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_NONE );
	CPPUNIT_ASSERT( this->socket->getPort() == 0 );
}

void SocketTest::testAddressConstructor()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->isConnected() );
	CPPUNIT_ASSERT( !this->socket->isClosed() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );
}

void SocketTest::testAddressConstructorAddrNone()
{
	try
	{
		this->socket = new Socket( INADDR_NONE, 1234 );

		// FAIL: System should not assign INADDR_NONE
		failTestMissingException( "IOException", "constructing a socket with INADDR_NONE" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", e, "constructing a socket with INADDR_NONE" );
	}
}

void SocketTest::testAddressConstructorDeadEndpoint()
{
	// Stop the server so that its not listening on the specified endpoint
	this->server->stop();

	try
	{
		this->socket = new Socket( INADDR_LOOPBACK, 1234 );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", "constructing a socket with a dead endpoint" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", e, "constructing a socket with a dead endpoint" );
	}
}

void SocketTest::testConnect()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( !socket->isConnected() );

	this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234) );
	CPPUNIT_ASSERT( this->socket->isConnected() );
	CPPUNIT_ASSERT( !this->socket->isClosed() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );
}

void SocketTest::testConnectWithTimeoutSpecified()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( !socket->isConnected() );

	this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234), 5000 );
	CPPUNIT_ASSERT( this->socket->isConnected() );
	CPPUNIT_ASSERT( !this->socket->isClosed() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );
}

void SocketTest::testConnectDeadEndpoint()
{
	// Stop the server so that its not listening on the specified endpoint
	this->server->stop();

	this->socket = new Socket();

	try
	{
		this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", "connecting a socket with a dead endpoint" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", e, "connecting a socket with a dead endpoint" );
	}

	// Socket should not be connected
	CPPUNIT_ASSERT( !this->socket->isConnected() );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_NONE );
	CPPUNIT_ASSERT( this->socket->getPort() == 0 );
}

void SocketTest::testConnectRemoteTimeout()
{
	this->server->stop();

	this->socket = new Socket();
	try
	{
		// Ideally we wouldn't be hitting google to test the timeout, however under linux
		// non-existant local addresses throw an unroutable error rather than timing out. So
		// we actually need something that actively blocks the connect request
		this->socket->connect( InetSocketAddress(TEXT("google.com"), 81), 1000 );
		failTestMissingException( "IOException", "connecting a remote socket with a timeout" );
	}
	catch( SocketTimeoutException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", e, "connecting a remote socket with a timeout" );
	}
}

void SocketTest::testConnectClosed()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	// Close the socket
	this->socket->close();

	try
	{
		// A connect call on the closed socket should fail
		this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling connect() on a closed socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling connect() on a closed socket" );
	}

	// Socket should still be closed
	CPPUNIT_ASSERT( this->socket->isClosed() );
}

void SocketTest::testConnectAlreadyConnected()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	try
	{
		// A second connect call should fail
		this->socket->connect( InetSocketAddress(INADDR_LOOPBACK, 1234) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling connect() on an already connected socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling connect() on an already connected socket" );
	}

	// Socket should still be connected
	CPPUNIT_ASSERT( this->socket->isConnected() );
}

void SocketTest::testSend()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	
	string data = "Hello World!";
	size_t length = data.length();

	this->server->registerForNextReceive();
	this->socket->send( (char*)&length, sizeof(size_t) );
	this->socket->send( (char*)data.data(), length );

	bool success = this->server->waitForNextReceive( 1000 );
	CPPUNIT_ASSERT( success );
	CPPUNIT_ASSERT( this->server->getReceiveCount() == 1 );

	string message = this->server->getLastReceivedMessage();
	CPPUNIT_ASSERT( data == message );
}

void SocketTest::testSendClosed()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	// Close the socket
	this->socket->close();

	try
	{
		string data = "Hello World!";
		size_t length = data.length();

		// A send call on the closed socket should fail
		this->socket->send( (char*)&length, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling send() on a closed socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling send() on a closed socket" );
	}

	CPPUNIT_ASSERT( this->server->getReceiveCount() == 0 );
}

void SocketTest::testSendNotConnected()
{
	// Construct a disconnected socket
	this->socket = new Socket();

	try
	{
		string data = "Hello World!";
		size_t length = data.length();

		// A send call on the disconnected socket should fail
		this->socket->send( (char*)&length, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling send() on a disconnected socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling send() on a disconnected socket" );
	}

	CPPUNIT_ASSERT( this->server->getReceiveCount() == 0 );
}

void SocketTest::testSendOutputShutdown()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	// Shutdown the socket's output
	this->socket->shutdownOutput();

	try
	{
		string data = "Hello World!";
		size_t length = data.length();

		// A send call on the socket should fail
		this->socket->send( (char*)&length, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling send() on a socket with shutdown output" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling send() on a socket with shutdown output" );
	}

	CPPUNIT_ASSERT( this->server->getReceiveCount() == 0 );
}

void SocketTest::testSendNegativeSize()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	
	try
	{
		string data = "Hello World!";
		
		// A send call with negative length should fail
		this->socket->send( data.c_str(), -1 );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling send() with a negative length" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling send() with a negative length" );
	}

	CPPUNIT_ASSERT( this->server->getReceiveCount() == 0 );
}

void SocketTest::testSendNullBuffer()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	
	try
	{
		// A send call with a null buffer should fail
		this->socket->send( NULL, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling send() with a NULL buffer" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling send() with a NULL buffer" );
	}

	CPPUNIT_ASSERT( this->server->getReceiveCount() == 0 );
}

void SocketTest::testReceive()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	// Send a message so that the DummyServer echoes it back
	string sentMessage = "Hello World!";
	this->quickStringSend( sentMessage );

	// Receive the data length
	size_t length = 0;
	size_t lengthBytes = sizeof( size_t );
	size_t receivedLengthBytes = 0;
	while( receivedLengthBytes < lengthBytes )
	{
		char* cursor = ((char*)&length) + receivedLengthBytes;
		size_t received = this->socket->receive( cursor, lengthBytes - receivedLengthBytes );
		CPPUNIT_ASSERT( received > 0 );

		receivedLengthBytes += received;
	}

	CPPUNIT_ASSERT( length == sentMessage.length() );

	// Receive the message contents
	char* messageData = new char[length];
	size_t receivedMessageBytes = 0;
	while( receivedMessageBytes < length )
	{
		char* cursor = messageData + receivedMessageBytes;
		size_t received = this->socket->receive( cursor, length - receivedMessageBytes );
		CPPUNIT_ASSERT( received > 0 );

		receivedMessageBytes += received;
	}

	string receivedMessage( messageData, length );
	CPPUNIT_ASSERT( sentMessage == receivedMessage );
	delete [] messageData;
}

void SocketTest::testReceiveClosed()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );

	// TODO Commented this out as it gives an unavoidable SIGPIPE under 
	// linux it's something we should look at in the future
	//this->quickStringSend( "Hello World!" );


	// Close the socket
	this->socket->close();

	try
	{
		size_t dummy = 0;

		// A receive call on the closed socket should fail
		this->socket->receive( (char*)&dummy, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling receive() on a closed socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling receive() on a closed socket" );
	}
}

void SocketTest::testReceiveNotConnected()
{
	// Construct a disconnected socket
	this->socket = new Socket();

	try
	{
		size_t dummy = 0;

		// A receive call on the disconnected socket should fail
		this->socket->receive( (char*)&dummy, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling receive() on a disconnected socket" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling receive() on a disconnected socket" );
	}
}

void SocketTest::testReceiveInputShutdown()
{
	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	
	// TODO Commented this out as it gives an unavoidable SIGPIPE under 
	// linux it's something we should look at in the future
	//this->quickStringSend( "Hello World!" );

	// Shutdown the socket's input
	this->socket->shutdownInput();

	try
	{
		size_t dummy = 0;

		// A receive call on the closed socket should fail
		this->socket->receive( (char*)&dummy, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling receive() on a socket with shutdown input" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling receive() on a socket with shutdown input" );
	}
}

void SocketTest::testReceiveNullBuffer()
{
	const string sentMessage = "Hello World!";

	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	this->quickStringSend( sentMessage );

	try
	{
		// A receive call with a NULL buffer should fail
		this->socket->receive( NULL, sizeof(size_t) );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling receive() with a NULL buffer" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling receive() with a NULL buffer" );
	}

	// Make sure it hasn't affected the state of the buffer
	string receivedMessage = this->quickStringReceive();
	CPPUNIT_ASSERT( receivedMessage == sentMessage );
}

void SocketTest::testReceiveNegativeSize()
{
	const string sentMessage = "Hello World!";

	// Construct a connected socket
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	this->quickStringSend( sentMessage );

	try
	{
		size_t dummy = 0;

		// A receive call with a negative size should fail
		this->socket->receive( (char*)&dummy, -1 );

		// FAIL: Expected an exception
		failTestMissingException( "IOException", 
		                          "calling receive() with a negative size" );
	}
	catch( IOException& )
	{
		// PASS: We expected this exception!
	}
	catch( exception& e )
	{
		// FAIL: Wrong exception
		failTestWrongException( "IOException", 
		                        e, 
								"calling receive() with a negative size" );
	}

	// Make sure it hasn't affected the state of the buffer
	string receivedMessage = this->quickStringReceive();
	CPPUNIT_ASSERT( receivedMessage == sentMessage );
}

void SocketTest::testGetInetAddress()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
}

void SocketTest::testGetInetAddressNotConnected()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_NONE );
}

void SocketTest::testGetInetAddressDisconnected()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );

	this->socket->close();

	// Contract of getInetAddress() states that the socket should still return the address it was
	// initially connected to after it has been closed
	CPPUNIT_ASSERT( this->socket->getInetAddress() == INADDR_LOOPBACK );
}

void SocketTest::testGetPort()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
}

void SocketTest::testGetPortNotConnected()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( this->socket->getPort() == 0 );
}

void SocketTest::testGetPortDisconnected()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );

	this->socket->close();

	// Contract of getPort() states that the socket should still return the port it was
	// initially connected to after it has been closed
	CPPUNIT_ASSERT( this->socket->getPort() == 1234 );
}

void SocketTest::testGetRemoteSocketAddress()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getRemoteSocketAddress() == 
	                InetSocketAddress(INADDR_LOOPBACK, 1234) );
}

void SocketTest::testGetRemoteSocketAddressNotConnected()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( this->socket->getRemoteSocketAddress() == 
	                InetSocketAddress(INADDR_NONE, 0) );
}

void SocketTest::testGetRemoteSocketAddressDisconnected()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( this->socket->getRemoteSocketAddress() == 
	                InetSocketAddress(INADDR_LOOPBACK, 1234) );

	this->socket->close();

	// Contract of getRemoteSocketAddress() states that the socket should still return the address
	// it was initially connected to after it has been closed
	CPPUNIT_ASSERT( this->socket->getRemoteSocketAddress() == 
	                InetSocketAddress(INADDR_LOOPBACK, 1234) );
}

void SocketTest::testShutdownInput()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );

	this->socket->shutdownInput();

	CPPUNIT_ASSERT( this->socket->isInputShutdown() );
}

void SocketTest::testShutdownInputNotConnected()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );

	try
	{
		this->socket->shutdownInput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the input of an unconnected socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the input of an unconnected socket" );
	}
}

void SocketTest::testShutdownInputDisconnected()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );

	this->socket->close();
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );

	try
	{
		this->socket->shutdownInput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the input of closed socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the input of a closed socket" );
	}
}

void SocketTest::testShutdownInputAlreadyShutdown()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isInputShutdown() );

	this->socket->shutdownInput();
	CPPUNIT_ASSERT( this->socket->isInputShutdown() );

	try
	{
		this->socket->shutdownInput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the input of closed socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the input of a closed socket" );
	}
}

void SocketTest::testShutdownOutput()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );

	this->socket->shutdownOutput();

	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );
}

void SocketTest::testShutdownOutputNotConnected()
{
	this->socket = new Socket();
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );

	try
	{
		this->socket->shutdownOutput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the output of an unconnected socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the output of an unconnected socket" );
	}
}

void SocketTest::testShutdownOutputDisconnected()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );

	this->socket->close();
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );

	try
	{
		this->socket->shutdownOutput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the output of closed socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the output of a closed socket" );
	}
}

void SocketTest::testShutdownOutputAlreadyShutdown()
{
	this->socket = new Socket( INADDR_LOOPBACK, 1234 );
	CPPUNIT_ASSERT( !this->socket->isOutputShutdown() );

	this->socket->shutdownOutput();
	CPPUNIT_ASSERT( this->socket->isOutputShutdown() );

	try
	{
		this->socket->shutdownOutput();

		// FAIL: Expecting an exception
		failTestMissingException( "IOException", "shutting down the output of closed socket" );
	}
	catch( IOException& )
	{
		// PASS: Excepected this exception
	}
	catch( exception& e )
	{
		// FAIL: Wrong type of exception
		failTestWrongException( "IOException", e, "shutting down the output of a closed socket" );
	}
}

void SocketTest::quickStringSend( const string& message )
{
	size_t length = message.length();
	
	this->socket->send( (char*)&length, sizeof(size_t) );
	this->socket->send( (char*)message.data(), length );
}

string SocketTest::quickStringReceive()
{
	// Receive the data length
	size_t length = 0;
	size_t lengthSize = sizeof( size_t );
	size_t receivedLengthSize = 0;
	while( receivedLengthSize < lengthSize )
	{
		char* cursor = ((char*)&length) + receivedLengthSize;
		size_t receivedSize = this->socket->receive( cursor, lengthSize - receivedLengthSize );

		receivedLengthSize += receivedSize;
	}

	// Receive the message contents
	char* messageData = new char[length];
	size_t receivedMessageSize = 0;
	while( receivedMessageSize < length )
	{
		char* cursor = messageData + receivedMessageSize;
		size_t receivedSize = this->socket->receive( cursor, length - receivedMessageSize );

		receivedMessageSize += receivedSize;
	}

	string receivedMessage( messageData, length );
	delete [] messageData;

	return receivedMessage;
}
