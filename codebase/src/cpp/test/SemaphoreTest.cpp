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
#include "SemaphoreTest.h"
#include "concurrent/Thread.h"
#include "Platform.h"

//CPPUNIT_TEST_SUITE_REGISTRATION( SemaphoreTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
SemaphoreTest::SemaphoreTest()
{

}

SemaphoreTest::~SemaphoreTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void SemaphoreTest::setUp()
{

}

void SemaphoreTest::tearDown()
{

}

void SemaphoreTest::testSemaphore()
{
	// Create a semaphore with 2 permits
	SysCommon::Semaphore semaphore( 2 );

	// Create 3 runnables/threads to ask for permits
	SemaphoreRunnable runnableOne( &semaphore );
	SemaphoreRunnable runnableTwo( &semaphore );
	SemaphoreRunnable runnableThree( &semaphore );

	SysCommon::Thread threadOne( &runnableOne );
	SysCommon::Thread threadTwo( &runnableTwo );
	SysCommon::Thread threadThree( &runnableThree );

	// Start the threads, they will block until we signal the acquire event on them
	threadOne.start();
	threadTwo.start();
	threadThree.start();

	// Have two runnables acquire the two available permits
	runnableOne.signalAcquire();
	runnableOne.waitForAcquired( NATIVE_INFINITE_WAIT );
	runnableTwo.signalAcquire();
	runnableTwo.waitForAcquired( NATIVE_INFINITE_WAIT );

	// Both runnables should have the permits
	CPPUNIT_ASSERT( runnableOne.isHoldingPermit() );
	CPPUNIT_ASSERT( runnableTwo.isHoldingPermit() );

	// Have another runnable attempt to acquire the permit, it should timeout as both permits are
	// taken
	runnableThree.signalAcquire();
	SysCommon::WaitResult blockedAcquire = runnableThree.waitForAcquired( 100L );
	CPPUNIT_ASSERT( blockedAcquire == SysCommon::WR_TIMEOUT );
	CPPUNIT_ASSERT( !runnableThree.isHoldingPermit() );

	// Have one of the original runnables release its permit, the waiting runnable should wake up
	// and acquire the released permit
	runnableOne.signalRelease();
	runnableThree.waitForAcquired( NATIVE_INFINITE_WAIT );
	CPPUNIT_ASSERT( runnableThree.isHoldingPermit() );

	runnableTwo.signalRelease();
	runnableThree.signalRelease();

	threadOne.join();
	threadTwo.join();
	threadThree.join();
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////  SyncPointRunnable  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
SemaphoreRunnable::SemaphoreRunnable( SysCommon::Semaphore* semaphore ) :
		acquireEvent(false, TEXT("acquire")), acquiredEvent(false, TEXT("acquired")), releaseEvent(false, TEXT("release"))
{
	this->holdingPermit = false;
	this->semaphore = semaphore;
}

SemaphoreRunnable::~SemaphoreRunnable()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void SemaphoreRunnable::run()
{
	acquireEvent.waitFor();
	this->semaphore->acquire();
	this->holdingPermit = true;
	acquiredEvent.signal();
	releaseEvent.waitFor();
	this->semaphore->release();
	this->holdingPermit = false;
}

void SemaphoreRunnable::signalAcquire()
{
	acquireEvent.signal();
}

SysCommon::WaitResult SemaphoreRunnable::waitForAcquired( unsigned long timeout )
{
	return acquiredEvent.waitFor( timeout );
}

void SemaphoreRunnable::signalRelease()
{
	releaseEvent.signal();
}

bool SemaphoreRunnable::isHoldingPermit()
{
	return this->holdingPermit;
}
