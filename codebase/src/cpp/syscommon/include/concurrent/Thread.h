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

#include <map>
#include "Platform.h"
#include "Exception.h"

namespace SysCommon
{
	/**
	 * An interface representing a unit of execution. IRunnables are usually passed into Threads 
	 * (which themselves are instance of IRunnable) in their constructors, and have their run() 
	 * method called when the Thread is started.
	 *
	 * To create your own thread, simply create a class that realises the IRunnable interface
	 * and overload the run() method. Then construct a Thread object, specifying your IRunnable
	 * class in the constructor, and call start() on the Thread.
	 *
	 * eg:
	 *
	 * IRunnable* myRunnable = new SpecialisedRunnable();
	 * Thread theThread = new Thread( myRunnable );
	 *
	 * // Start the thread
	 * theThread.start();
	 *
	 * // Do things while the thread is running...
	 *
	 * // Interrupt and wait for the thread to finish
	 * theThread.interrupt();
	 * theThread.join();
	 *
	 * // clean up
	 * delete theThread;
	 * delete myRunnable;
	 */
	class IRunnable
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			virtual ~IRunnable() {};

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Called by the containing Thread object when it is started. The run method
			 * may perform any action that is required to fulfil its purpose.
			 */
			virtual void run() = 0;
	};

	/**
	 * The IInterruptable interface forms part of the Thread models interruption framework. This
	 * interface should be implemented by any class that intends to perform a blocking operation
	 * so that it can be interrupted by calls to Thread.interrupt().
	 *
	 * The interruption framework is built upon the Visitor design pattern whereby the object
	 * about to perform the blocking operation notifies the current thread object through a call
	 * to Thread.acceptInterruptable(). The thread then calls back into the IInterruptable class
	 * through IInterruptable.visit() exposing the Thread's interrupt handle to the IInterruptable
	 * instance. The advantage of this architecture is that NATIVE_INTERRUPT handles are only 
	 * exposed to those classes that implement the IInterruptable interface, not all classes in 
	 * general.
	 *
	 * Once the interrupt handle has been obtained, the IInterruptable instance can wait on both
	 * its own blocking handle and the Thread's interrupt handle through the Platform method
	 * Platform.waitOnInterruptableHandle()
	 */
	class IInterruptable
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			virtual ~IInterruptable() {};

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Called by the Thread instance in response to a call to Thread.acceptInterruptable().
			 * This method exposes the Thread's interrupt handle to the IInterruptable instance and
			 * also forwards on the timeout value that acceptInterruptable() was called with.
			 *
			 * This method should perform the blocking operation (through the Platform method
			 * Platform.waitOnInterruptableHandle()) and return the WaitResult value to indicate
			 * the manner in which the operation completed (eg: success, failure, timeout, 
			 * interruption etc).
			 *
			 * @param threadInterrupt The thread's interrupt handle
			 * @param timeout The amount of time to wait on the blocking operation
			 *
			 * @return A WaitResult indicating the manner in which the blocking operation completed
			 */
			virtual WaitResult visit( NATIVE_INTERRUPT& threadInterrupt, unsigned long timeout ) = 0;

	};

	/**
	 * Represents a unit of execution that can be run simultaneously with other threads.
	 *
	 * Threads of execution can be created via two methods:
	 * ----------------------------------------------------------------------------------------------
	 * Method 1:
	 * ----------------------------------------------------------------------------------------------
	 * 1. Create a class that implements the IRunnable interface, override the run() method to
	 * contain the thread's logic
	 * 2. Construct an instance of the default Thread class with the IRunnable as a parameter.
	 * 3. Call the thread's start() method.
	 *
	 * eg:
	 *
	 * IRunnable* myRunnable = new SpecialisedRunnable();
	 * Thread theThread = new Thread( myRunnable );
	 *
	 * // Start the thread
	 * theThread.start();
	 *
	 * // Do things while the thread is running...
	 *
	 * // Interrupt and wait for the thread to finish
	 * theThread.interrupt();
	 * theThread.join();
	 *
	 * // clean up
	 * delete theThread;
	 * delete myRunnable;
	 *
	 * ----------------------------------------------------------------------------------------------
	 * Method 2:
	 * ----------------------------------------------------------------------------------------------
	 * 1. Create a class that extends Thread, override the run() method to contain the thread's
	 * logic.
	 * 2. Construct an instance of the class and call it's start() method.
	 */
	class Thread : public IRunnable
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		private:
			enum ThreadState { TS_STOPPED, TS_ALIVE };

			static unsigned long THREAD_ID_COUNTER;
			static Thread mainThread;
			static NATIVE_THREAD mainThreadHandle;
			static std::map<unsigned long, Thread*> threadMap;
			static std::map<NATIVE_THREAD, Thread*> nativeThreadMap;


		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_THREAD sysThreadHandle;
			unsigned long syntheticHandle;
			volatile ThreadState state;

			bool interruptInitialised;
			NATIVE_INTERRUPT interruptEvent;

			IRunnable* runner;
			String name;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Default constructor for type Thread with anonymous name and NULL IRunnable
			 */
			Thread();

			/**
			 * Constructor for type Thread with provided IRunnable and anonymous name. The runnable
			 * will be executed when the Thread instance is started through the Thread.start() 
			 * method.
			 *
			 * @param runnable This Thread's unit of execution
			 */
			Thread( IRunnable* runnable );

			/**
			 * Constructor for type Thread with provided IRunnable and name. The runnable will be 
			 * executed when the Thread instance is started through the Thread.start() 
			 *
			 * @param runnable This thread's unit of execution
			 * @param name The name of the thread
			 */
			Thread( IRunnable* runnable, const tchar* name );

			/**
			 * Default destructor for type Thread
			 */
			virtual ~Thread();

		private:
			void _Thread( IRunnable* runnable, const String& name );
			
		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Returns this thread's name
			 *
			 * @return The name of this thread
			 */
			const tchar* getName();

			/**
			 * Interrupts this thread. Any operations within this thread's unit of execution that
			 * are blocking will return with a WaitResult of WR_INTERRUPTED and the thread's unit
			 * of execution should begin cleanup with a view to terminating.
			 */
			void interrupt();

			/**
			 * Returns whether this thread is currently executing.
			 *
			 * @return Whether the current thread is executing.
			 */
			bool isAlive();

			/**
			 * Blocks the current thread indefinately until this thread instance has completed 
			 * executing. Calls to join() are usually preceded by a call to interrupt().
			 *
			 * Note: This call itself may be interrupted by other threads.
			 *
			 * @throw InterruptedException if the current thread was interrupted before the join 
			 * operation could complete
			 */
			void join() throw ( InterruptedException );

			/**
			 * Blocks the current thread until either this thread instance has completed executing
			 * or the prescribed timeout period expires. Calls to join() are usually preceded by a 
			 * call to interrupt().
			 *
			 * Note: This call itself may be interrupted by other threads.
			 *
			 * @throw InterruptedException if the current thread was interrupted before the join 
			 * operation could complete
			 */
			void join( unsigned long millis ) throw ( InterruptedException );

			/**
			 * Accepts an IInterruptable visitor, exposing the thread's interrupt handle to the 
			 * visitor's corresponding visit() method.
			 *
			 * @return A WaitResult indicating the manner in which the visitor's blocking operation
			 * completed.
			 */
			WaitResult acceptInterruptable( IInterruptable* interruptable, unsigned long timeout );

			/**
			 * If this thread was constructed with a corresponding IRunnable instance, then that
			 * object's run() method is called. Otherwise if no IRunnable exists, this operation
			 * has no effect.
			 */
			virtual void run();

			/**
			 * Executes this thread's run method in a new thread of execution.
			 */
			virtual void start();

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Returns the current thread of execution.
			 *
			 * @return The currently executing thread.
			 */
			static Thread* currentThread();

			/**
			 * Suspends this thread for the specified time period.
			 *
			 * Note: This call may be interrupted by other threads.
			 *
			 * @return A WaitResult indicating the manner in which the skeep() operation completed.
			 */
			static void sleep( unsigned long millis ) throw ( InterruptedException );
			static void threadEntry( void* threadInstance );
			
		private:
			static String generateAnonymousThreadName();
	};
}
