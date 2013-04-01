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
#include "concurrent/Semaphore.h"
#include "concurrent/Event.h"

class SemaphoreTest: public CppUnit::TestFixture
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
		SemaphoreTest();
		virtual ~SemaphoreTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testSemaphore();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( SemaphoreTest );
		CPPUNIT_TEST( testSemaphore );
	CPPUNIT_TEST_SUITE_END();
};

// SemaphoreRunnable runnable helper class
class SemaphoreRunnable : public SysCommon::IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		SysCommon::Event acquireEvent;
		SysCommon::Event acquiredEvent;
		SysCommon::Event releaseEvent;
		volatile bool holdingPermit;

		SysCommon::Semaphore* semaphore;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		SemaphoreRunnable( SysCommon::Semaphore* semaphore );
		virtual ~SemaphoreRunnable();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		virtual void run();
		void signalAcquire();
		SysCommon::WaitResult waitForAcquired( unsigned long timeout );
		void signalRelease();
		bool isHoldingPermit();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

