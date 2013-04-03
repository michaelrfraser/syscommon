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

namespace SysCommon
{
	/**
	 * This class represents a reentrant mutual exclusion Lock.
	 *
	 * A ReentrantLock is owned by the thread last successfully locking, but not yet unlocking it. 
	 * A thread invoking lock will return, successfully acquiring the lock, when the lock is not 
	 * owned by another thread. The method will return immediately if the current thread already 
	 * owns the lock.
	 */
	class Lock
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			NATIVE_CRITICALSECTION mutex;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			Lock();
			virtual ~Lock();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			/**
			 * Acquires the lock if it is not held by another thread and returns immediately
			 *
			 * If the lock is held by another thread then the current thread becomes disabled for 
			 * thread scheduling purposes and lies dormant until the lock has been acquired
			 */
			void lock();

			/**
			 * Attempts to release this lock.
			 */
			void unlock();

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};
}
