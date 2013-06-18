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
#include "syscommon/concurrent/Thread.h"

#include <limits.h>
#include "syscommon/util/StringUtils.h"

using namespace syscommon;

//----------------------------------------------------------
//                    STATIC VARIABLES
//----------------------------------------------------------
unsigned long Thread::THREAD_ID_COUNTER = 0;
std::map<unsigned long, Thread*> Thread::threadMap;
std::map<NATIVE_THREAD, Thread*> Thread::nativeThreadMap;
Thread Thread::mainThread( NULL, TEXT("Main") );
NATIVE_THREAD Thread::mainThreadHandle = Platform::getCurrentThreadHandle();

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
/**
 * Default constructor for type Thread with anonymous name and NULL IRunnable
 */
Thread::Thread() : joinEvent( false, TEXT("ThreadJoin") )
{
	// Generate anonymous name and call initialiser
	String anonymousName = this->generateAnonymousThreadName();
	this->_Thread( NULL, anonymousName );
}

/**
 * Constructor for type Thread with provided IRunnable and anonymous name. The runnable
 * will be executed when the Thread instance is started through the Thread.start() 
 * method.
 *
 * @param runnable This Thread's unit of execution
 */
Thread::Thread( IRunnable* runnable ) : joinEvent( false, TEXT("ThreadJoin") )
{
	// Generate anonymous name and call initialiser
	String anonymousName = this->generateAnonymousThreadName();
	this->_Thread( runnable, anonymousName );
}

/**
 * Constructor for type Thread with provided IRunnable and name. The runnable will be 
 * executed when the Thread instance is started through the Thread.start() 
 *
 * @param runnable This thread's unit of execution
 * @param name The name of the thread
 */
Thread::Thread( IRunnable* runnable, const tchar* name ) : joinEvent( false, TEXT("ThreadJoin") )
{
	this->_Thread( runnable, String(name) );
}

/**
 * Common thread initialiser method
 */
void Thread::_Thread( IRunnable* runnable, const String& name )
{
	this->state = TS_STOPPED;
	this->runner = runnable;
	this->name = name;
	this->syntheticHandle = THREAD_ID_COUNTER++;
	this->threadMap[this->syntheticHandle] = this;

	this->sysThreadHandle = Platform::createUninitialisedThread();
	this->interruptEvent = Platform::createUninitialisedInterrupt();
	this->interruptInitialised = Platform::initialiseThreadInterrupt( this->interruptEvent, TEXT("ThreadInterrupt") );
}

/**
 * Default destructor for type Thread
 */
Thread::~Thread()
{
	if ( this->interruptInitialised )
		Platform::destroyThreadInterrupt( this->interruptEvent );
	this->threadMap.erase( this->syntheticHandle );
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
/**
 * Returns this thread's name
 *
 * @return The name of this thread
 */
const tchar* Thread::getName()
{
	return this->name.c_str();
}

/**
 * Interrupts this thread. Any operations within this thread's unit of execution that
 * are blocking will return with a WaitResult of WR_INTERRUPTED and the thread's unit
 * of execution should begin cleanup with a view to terminating.
 */
void Thread::interrupt()
{
	// Signal the interrupt event
	if ( Platform::isThreadInterruptInitialised( this->interruptEvent ) )
		Platform::signalInterrupt( this->interruptEvent );
}

/**
 * Returns whether this thread is currently executing.
 *
 * @return Whether the current thread is executing.
 */
bool Thread::isAlive()
{
	return this->state == TS_ALIVE;
}

/**
 * Blocks the current thread indefinitely until this thread instance has completed
 * executing. Calls to join() are usually preceded by a call to interrupt().
 *
 * Note: This call itself may be interrupted by other threads.
 * 
 * @throw InterruptedException if the current thread was interrupted before the join 
 * operation could complete
 */
void Thread::join() throw ( InterruptedException )
{
	// Join the thread with INFINITE timeout
	this->join( NATIVE_INFINITE_WAIT );
}

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
bool Thread::join( unsigned long millis ) throw ( InterruptedException )
{	
	// The commented code below was how the join was originally done, however it was creating a 
	// buttload of race conditions, so I've just used an event to signal the join instead

	//if ( this->state == TS_ALIVE )
	//{
	//	// Get the currently executing thread for its interrupt handle
	//	Thread* thisThread = Thread::currentThread();
	//	if ( thisThread && thisThread->interruptInitialised )
	//	{
	//		NATIVE_THREAD threadHandle = this->sysThreadHandle;
	//		
	//		// Join this thread with the provided timeout
	//		WaitResult result = Platform::joinThread( threadHandle, 
	//												  thisThread->interruptEvent, 
	//												  millis );

	//		if( result == WR_INTERRUPTED )
	//		{
	//			throw InterruptedException( TEXT("Thread interrupted") );
	//		}
	//	}
	//}

	WaitResult result = this->joinEvent.waitFor( millis );
	if( result == WR_INTERRUPTED )
		throw InterruptedException( TEXT("Thread interrupted") );

	bool completed = result == WR_SUCCEEDED;
	return completed;
}

/**
 * Accepts an IInterruptable visitor, exposing the thread's interrupt handle to the 
 * visitor's corresponding visit() method.
 *
 * @return A WaitResult indicating the manner in which the visitor's blocking operation
 * completed.
 */
WaitResult Thread::acceptInterruptable( IInterruptable* interruptable, unsigned long timeout )
{
	WaitResult result = WR_FAILED;

	// Call the IInterruptable's visit() method with this thread's interrupt handle
	if ( interruptable && this->interruptInitialised )
		result = interruptable->visit( this->interruptEvent, timeout );
	
	return result;
}

/**
 * If this thread was constructed with a corresponding IRunnable instance, then that
 * object's run() method is called. Otherwise if no IRunnable exists, this operation
 * has no effect.
 */
void Thread::run()
{
	if ( this->runner )
		this->runner->run();
}

/**
 * Executes this thread's run method in a new thread of execution.
 */
void Thread::start()
{
	if ( this->state == TS_STOPPED )
	{
		// Create the thread in suspended mode
		bool threadInitialised = Platform::initialiseThread( this->sysThreadHandle, 
															 (void*)this );

		if ( threadInitialised )
		{
			// Resume the thread
			Platform::resumeThread( this->sysThreadHandle );
		}
	}
}

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
/**
 * Returns the current thread of execution.
 *
 * @return The currently executing thread.
 */ 
Thread* Thread::currentThread()
{
	Thread* asThread = NULL;

	// Get the current thread's ID from the OS
	NATIVE_THREAD sysHandle = Platform::getCurrentThreadHandle();

	// If the ID was not the main thread's ID
	if( !Platform::threadHandlesEqual(sysHandle, Thread::mainThreadHandle) )
	{
		// Lookup the ID in the map
		std::map<NATIVE_THREAD, Thread*>::iterator threadPos = Thread::nativeThreadMap.find( sysHandle );
		if ( threadPos != Thread::nativeThreadMap.end() )
		{
			// Found the thread, so return it
			asThread = (*threadPos).second;
		}
	}
	else
	{
		// This thread is the main thread, so return it
		asThread = &Thread::mainThread;
	}
	
	return asThread;
}

/**
 * Suspends this thread for the specified time period.
 *
 * Note: This call may be interrupted by other threads.
 *
 * @throw InterruptedException if the current thread was interrupted before the sleep
 * operation could complete
 */
void Thread::sleep( unsigned long millis ) throw ( InterruptedException )
{
	Thread* thisThread = Thread::currentThread();
	if ( thisThread && thisThread->interruptInitialised )
	{
		WaitResult result = Platform::performInterruptableSleep( thisThread->interruptEvent, 
																 millis );

		if( result == WR_INTERRUPTED )
			throw InterruptedException( TEXT("Thread interrupted") );
	}
}

/**
 * Generates an anonymous thread name
 *
 * @return A string representing a unique thread name
 */
String Thread::generateAnonymousThreadName()
{
	String result = TEXT("Thread-");
	result.append( StringUtils::longToString(THREAD_ID_COUNTER) );
	return result;
}

/**
 * Entry point for platform threads.
 *
 * @param threadInstance The instance of the Thread class that is being started.
 */
void Thread::threadEntry( void* threadInstance )
{
	if( threadInstance )
	{
		Thread* asThread = reinterpret_cast<Thread*>( threadInstance );
		if ( asThread )
		{
			// Put the thread in the native thread map
			NATIVE_THREAD sysHandle = asThread->sysThreadHandle;
			Thread::nativeThreadMap[sysHandle] = asThread;

			// Switch into TS_ALIVE state
			asThread->state = TS_ALIVE;

			// Run the thread
			asThread->run();

			// Switch back to TS_STOPPED state
			asThread->state = TS_STOPPED;

			// Clean up and close handles
			Thread::nativeThreadMap.erase( sysHandle );
			Platform::destroyThread( sysHandle );

			asThread->joinEvent.signal();
		}
	}
}
