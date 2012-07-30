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

#include "Lock.h"

using namespace SysCommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Lock::Lock()
{
	Platform::initialiseCriticalSection( mutex );
}

Lock::~Lock()
{
	Platform::destroyCriticalSection( mutex );
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void Lock::lock()
{
	Platform::enterCriticalSection( mutex );
}

void Lock::unlock()
{
	Platform::leaveCriticalSection( mutex );
}