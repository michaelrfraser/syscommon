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

#include "Platform.h"
#include "Thread.h"
#include "Exception.h"

namespace SysCommon
{
	/** 
	 * A counting semaphore. Conceptually, a semaphore maintains a set of permits. Each acquire() 
	 * blocks if necessary until a permit is available, and then takes it. Each release() adds a 
	 * permit, potentially releasing a blocking acquirer. However, no actual permit objects are 
	 * used; the Semaphore just keeps a count of the number available and acts accordingly.
	 *
	 * Semaphores are often used to restrict the number of threads than can access some (physical 
	 * or logical) resource.
	 * 
	 * A semaphore initialized to one, and which is used such that it only has at most one permit 
	 * available, can serve as a mutual exclusion lock. This is more commonly known as a binary 
	 * semaphore, because it only has two states: one permit available, or zero permits available. 
	 * When used in this way, the binary semaphore has the property (unlike many Lock 
	 * implementations), that the "lock" can be released by a thread other than the owner (as 
	 * semaphores have no notion of ownership).
	 */
	class Semaphore : public IInterruptable
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		private:
			static unsigned long SEMAPHORE_ID_COUNTER;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:	
			NATIVE_SEMAPHORE nativeSemaphore;
			unsigned long syntheticID;
			bool initialised;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			/**
			 * Creates a Semaphore with the given number of permits
			 */
			Semaphore( unsigned int permits );

			/**
			 * Creates a Semaphore with the given number of permits and the specified name.
			 */
			Semaphore( unsigned int permits, const tchar* name );
			virtual ~Semaphore();

		private:
			/**
			 * Internal constructor helper
			 */
			void _Semaphore( unsigned int permits, const String& name );

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Acquires a permit from this semaphore, blocking until one is available, or the 
			 * thread is interrupted.
			 *
			 * @throws InterruptedException if the thread was interrupted before the permit could 
			 * be acquired.
			 */
			void acquire() throw ( InterruptedException );

			/**
			 * Acquires a permit from this semaphore, if one becomes available within the given 
			 * waiting time and the current thread has not been interrupted.
			 *
			 * @return true if a permit was acquired and false if the waiting time elapsed before a 
			 * permit was acquired
			 *
			 * @throws InterruptedException if the thread was interrupted before the permit could 
			 * be acquired.
			 */
			bool tryAcquire( DWORD timeoutMillis ) throw ( InterruptedException );

			/**
			 * Releases a permit, returning it to the semaphore.
			 */
			bool release();

		private:
			/**
			 * Interupt visitor
			 */
			virtual WaitResult visit( NATIVE_INTERRUPT& threadInterrupt, DWORD timeout );

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
		private:
			String generateAnonymousSemaphoreName( unsigned long syntheticID );
	};
}