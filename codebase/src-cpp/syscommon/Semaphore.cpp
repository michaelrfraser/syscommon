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

#include "Semaphore.h"
#include "Thread.h"
#include "Utils.h"

#include "assert.h"

using namespace SysCommon;

//----------------------------------------------------------
//                    STATIC VARIABLES
//----------------------------------------------------------
unsigned long Semaphore::SEMAPHORE_ID_COUNTER = 0;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Semaphore::Semaphore( unsigned int permits )
{
	_Semaphore( permits, this->generateAnonymousSemaphoreName(SEMAPHORE_ID_COUNTER) );
}

Semaphore::Semaphore( unsigned int permits, const tchar* name )
{
	_Semaphore( permits, String(name) );
	this->syntheticID = SEMAPHORE_ID_COUNTER++;
}

Semaphore::~Semaphore()
{
	if ( this->initialised )
	{
		Platform::destroySemaphore( this->nativeSemaphore );
		this->initialised = false;
	}
}

void Semaphore::_Semaphore( unsigned int permits, const String& name )
{
	this->nativeSemaphore = NATIVE_SEMAPHORE_UNINIT;
	this->initialised = Platform::initialiseSemaphore( this->nativeSemaphore, permits, name );
}

void Semaphore::acquire() throw ( InterruptedException )
{
	this->tryAcquire( INFINITE );
}

bool Semaphore::tryAcquire( DWORD timeoutMillis ) throw ( InterruptedException )
{
	bool acquired = false;
	if ( this->initialised )
	{
		Thread* pCurrentThread = Thread::currentThread();

		assert( pCurrentThread );
		if ( pCurrentThread )
		{
			WaitResult result = pCurrentThread->acceptInterruptable( this, timeoutMillis );
			if( result == WR_INTERRUPTED )
				throw InterruptedException( TEXT("Thread Interrupted") );
			else if( result == WR_SUCCEEDED )
				acquired = true;
		}
	}

	return acquired;
}

bool Semaphore::release()
{
	bool result = false;

	if ( this->initialised )
		result = Platform::releaseSemaphore( this->nativeSemaphore );

	return result;
}

WaitResult Semaphore::visit( NATIVE_INTERRUPT& threadInterrupt, DWORD timeoutMillis )
{
	return Platform::waitOnSemaphore( this->nativeSemaphore, threadInterrupt, timeoutMillis );
}

String Semaphore::generateAnonymousSemaphoreName( unsigned long syntheticID )
{
	String result = TEXT("Semaphore-");
	result.append( Utils::longToString(syntheticID) );
	return result;
}