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
#include "concurrent/Event.h"
#include <limits.h>

using namespace SysCommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Event::Event( bool initialState, const String& name )
{
	this->name = name;
	this->event = Platform::createUninitialisedEvent();
	Platform::initialiseEvent( this->event, initialState, name );
}

Event::~Event()
{
	Platform::destroyEvent( this->event );
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
void Event::signal()
{
	Platform::signalEvent( this->event );
}

void Event::clear()
{
	Platform::clearEvent( this->event );
}

WaitResult Event::waitFor()
{
	return Platform::waitOnEvent( this->event, NATIVE_INFINITE_WAIT );
}

WaitResult Event::waitFor( unsigned long timeout )
{
	return Platform::waitOnEvent( this->event, timeout );
}
