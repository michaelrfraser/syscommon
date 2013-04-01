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
#include "concurrent/Thread.h"
#include "concurrent/Event.h"
#include "concurrent/Lock.h"

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
class LockRunnable : public SysCommon::IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		SysCommon::Event lockEvent;
		SysCommon::Event lockedEvent;
		SysCommon::Event unlockEvent;
		volatile bool holdingLock;

		SysCommon::Lock* lock;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		LockRunnable( SysCommon::Lock* lock );
		virtual ~LockRunnable();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		virtual void run();
		void signalLock();
		SysCommon::WaitResult waitForLocked( unsigned long timeout );
		void signalUnlock();
		bool isHoldingLock();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

