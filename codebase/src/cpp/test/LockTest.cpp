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
#include "LockTest.h"
#include "concurrent/Thread.h"
#include "Platform.h"

CPPUNIT_TEST_SUITE_REGISTRATION( LockTest );

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
LockTest::LockTest()
{

}

LockTest::~LockTest()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void LockTest::setUp()
{

}

void LockTest::tearDown()
{

}

void LockTest::testLock()
{
	// Create a semaphore with 2 permits
	SysCommon::Lock lock;

	// Create 3 runnables/threads to ask for permits
	LockRunnable runnableOne( &lock );
	LockRunnable runnableThree( &lock );

	SysCommon::Thread threadOne( &runnableOne, "ThreadOne" );
	SysCommon::Thread threadThree( &runnableThree, "ThreadThree" );

	// Start the threads, they will block until we signal the acquire event on them
	threadOne.start();
	threadThree.start();

	// Have two runnables acquire the two available permits
	runnableOne.signalLock();
	runnableOne.waitForLocked( NATIVE_INFINITE_WAIT );

	// Both runnables should have the permits
	CPPUNIT_ASSERT( runnableOne.isHoldingLock() );

	// Have another runnable attempt to acquire the permit, it should timeout as both permits are
	// taken
	runnableThree.signalLock();
	SysCommon::WaitResult blockedLock = runnableThree.waitForLocked( 100L );
	CPPUNIT_ASSERT( blockedLock == SysCommon::WR_TIMEOUT );
	CPPUNIT_ASSERT( !runnableThree.isHoldingLock() );

	// Have one of the original runnables release its permit, the waiting runnable should wake up
	// and acquire the released permit
	runnableOne.signalUnlock();
	runnableThree.waitForLocked( NATIVE_INFINITE_WAIT );
	CPPUNIT_ASSERT( runnableThree.isHoldingLock() );

	runnableThree.signalUnlock();

	threadOne.join();
	threadThree.join();
}

///////////////////////////////////////////////////////////////////////////////
/////////////////////////////  SyncPointRunnable  /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
LockRunnable::LockRunnable( SysCommon::Lock* lock ) :
		lockEvent(false, TEXT("lock")),
		lockedEvent(false, TEXT("locked")),
		unlockEvent(false, TEXT("unlock"))
{
	this->holdingLock = false;
	this->lock = lock;
}

LockRunnable::~LockRunnable()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void LockRunnable::run()
{
	lockEvent.waitFor();
	this->lock->lock();
	this->holdingLock = true;
	lockedEvent.signal();
	unlockEvent.waitFor();
	this->lock->unlock();
	this->holdingLock = false;
}

void LockRunnable::signalLock()
{
	lockEvent.signal();
}

SysCommon::WaitResult LockRunnable::waitForLocked( unsigned long timeout )
{
	return lockedEvent.waitFor( timeout );
}

void LockRunnable::signalUnlock()
{
	unlockEvent.signal();
}

bool LockRunnable::isHoldingLock()
{
	return this->holdingLock;
}
