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
#include "ThreadTest.h"

#include <limits.h>
#include "syscommon/Platform.h"
#include "syscommon/concurrent/Thread.h"

CPPUNIT_TEST_SUITE_REGISTRATION( ThreadTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
ThreadTest::ThreadTest()
{

}

ThreadTest::~ThreadTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void ThreadTest::setUp()
{

}

void ThreadTest::tearDown()
{

}

void ThreadTest::testMainThread()
{
	syscommon::Thread* mainThread = syscommon::Thread::currentThread();
	CPPUNIT_ASSERT( mainThread != NULL );
}

void ThreadTest::testIsAlive()
{
	// Create a test event and a runner to wait on it
	syscommon::Event testEvent( false, TEXT("SyncEvent") );
	SyncPointRunnable testRunner( &testEvent );

	// Create the test thread, it should initially not be alive
	syscommon::Thread testThread( &testRunner );
	CPPUNIT_ASSERT( !testThread.isAlive() );

	// Start the thread
	testThread.start();
	testRunner.waitForIsAlive();

	// Should now be alive
	CPPUNIT_ASSERT( testThread.isAlive() );

	// Signal the event and
	testEvent.signal();
	testThread.join();

	// Should no longer be alive
	CPPUNIT_ASSERT( !testThread.isAlive() );
}

void ThreadTest::testSleep()
{
	syscommon::Thread* currentThread = syscommon::Thread::currentThread();

	// Clock time before sleep
	unsigned long before = syscommon::Platform::getCurrentTimeMilliseconds();
	currentThread->sleep( 300L );

	// Clock time after sleep
	unsigned long after = syscommon::Platform::getCurrentTimeMilliseconds();

	// Difference should not be less than the sleep period
	CPPUNIT_ASSERT( after >= (before + 300L) );
}

void ThreadTest::testInterruptSleep()
{
	IndefiniteWaitRunnable testRunner;
	syscommon::Thread testThread( &testRunner );

	testThread.start();
	testRunner.waitForIsAlive();

	// Now the thread has got past the alive syncpoint, it will be sleeping indefinitely, so
	// wake it up by interrupting it!
	testThread.interrupt();
	testThread.join();

	IndefiniteWaitRunnable::FinishResult result = testRunner.getResult();
	CPPUNIT_ASSERT( result == IndefiniteWaitRunnable::FR_SUCCESS );
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////  SyncPointRunnable  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
SyncPointRunnable::SyncPointRunnable( syscommon::Event* waitForEvent ) :
		isAliveEvent(false, TEXT("IsAlive"))
{
	this->waitForEvent = waitForEvent;
}

SyncPointRunnable::~SyncPointRunnable()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void SyncPointRunnable::waitForIsAlive()
{
	this->isAliveEvent.waitFor();
}

void SyncPointRunnable::run()
{
	this->isAliveEvent.signal();
	this->waitForEvent->waitFor();
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////////  IndefiniteWaitRunnable  ///////////////////////////
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
IndefiniteWaitRunnable::IndefiniteWaitRunnable() :
	isAliveEvent(false, TEXT("IsAlive"))
{
	this->result = FR_NOT_RUN;
}

IndefiniteWaitRunnable::~IndefiniteWaitRunnable()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void IndefiniteWaitRunnable::waitForIsAlive()
{
	this->isAliveEvent.waitFor();
}

void IndefiniteWaitRunnable::run()
{
	this->isAliveEvent.signal();

	try
	{
		// Sleep pretty much forever
		syscommon::Thread* currentThread = syscommon::Thread::currentThread();
		currentThread->sleep( NATIVE_INFINITE_WAIT );
	}
	catch( syscommon::InterruptedException& )
	{
		// We are expecting this exception
		this->result = FR_SUCCESS;
	}
	catch( ... )
	{
		this->result = FR_WRONG_EXCEPTION;
	}
}

IndefiniteWaitRunnable::FinishResult IndefiniteWaitRunnable::getResult()
{
	return this->result;
}

