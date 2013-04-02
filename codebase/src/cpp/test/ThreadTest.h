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

class ThreadTest: public CppUnit::TestFixture
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
		ThreadTest();
		virtual ~ThreadTest();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void setUp();
		void tearDown();

	protected:
		void testMainThread();
		void testIsAlive();
		void testSleep();
		void testInterruptSleep();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
	CPPUNIT_TEST_SUITE( ThreadTest );
		CPPUNIT_TEST( testMainThread );
		CPPUNIT_TEST( testIsAlive );
		//CPPUNIT_TEST( testSleep );
		CPPUNIT_TEST( testInterruptSleep );
	CPPUNIT_TEST_SUITE_END();
};

// SyncPoint runnable helper class
class SyncPointRunnable : public SysCommon::IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		SysCommon::Event* waitForEvent;
		SysCommon::Event isAliveEvent;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		SyncPointRunnable( SysCommon::Event* waitForEvent );
		virtual ~SyncPointRunnable();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void waitForIsAlive();
		virtual void run();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

// IndefiniteWait runnable helper class
class IndefiniteWaitRunnable : public SysCommon::IRunnable
{
	//----------------------------------------------------------
	//                    STATIC VARIABLES
	//----------------------------------------------------------
	public:
		enum FinishResult { FR_NOT_RUN, FR_SUCCESS, FR_WRONG_EXCEPTION };

	//----------------------------------------------------------
	//                   INSTANCE VARIABLES
	//----------------------------------------------------------
	private:
		FinishResult result;
		SysCommon::Event isAliveEvent;

	//----------------------------------------------------------
	//                      CONSTRUCTORS
	//----------------------------------------------------------
	public:
		IndefiniteWaitRunnable();
		virtual ~IndefiniteWaitRunnable();

	//----------------------------------------------------------
	//                    INSTANCE METHODS
	//----------------------------------------------------------
	public:
		void waitForIsAlive();
		virtual void run();
		FinishResult getResult();

	//----------------------------------------------------------
	//                     STATIC METHODS
	//----------------------------------------------------------
};

