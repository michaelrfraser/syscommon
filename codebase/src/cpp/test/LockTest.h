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

#include "Common.h"
#include "syscommon/concurrent/Thread.h"
#include "syscommon/concurrent/Event.h"
#include "syscommon/concurrent/Lock.h"

class LockTest: public CppUnit::TestFixture
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		LockTest();
		virtual ~LockTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testLock();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( LockTest );
		CPPUNIT_TEST( testLock );
	CPPUNIT_TEST_SUITE_END();
};

// LockRunnable runnable helper class
class LockRunnable : public syscommon::IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		syscommon::Event lockEvent;
		syscommon::Event lockedEvent;
		syscommon::Event unlockEvent;
		volatile bool holdingLock;

		syscommon::Lock* lock;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		LockRunnable( syscommon::Lock* lock );
		virtual ~LockRunnable();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		virtual void run();
		void signalLock();
		syscommon::WaitResult waitForLocked( unsigned long timeout );
		void signalUnlock();
		bool isHoldingLock();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

