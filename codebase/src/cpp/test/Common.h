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

/////////////////////////////////
////////// #includes's //////////
/////////////////////////////////
// Default stuff
#include <stdio.h>
#include <stdarg.h>

// CppUnit includes
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>

#include "StringServer.h"

////////////////////////////////////
////////// useful methods //////////
////////////////////////////////////
/*
 * Fail the current test with the given message
 */
void failTest( const char *format, ... );

/*
 * Test should fail because an exception was expected, but none occurred. The failure message
 * will also include the action that was underway (and should have caused an exception).
 */
void failTestMissingException( const char *expectedException, const char* action );

/*
 * An exception was received, but it wasn't the one we expected. The failure message will
 * include the expected and actual exception types and a message regarding the action that
 * was in progress.
 */
void failTestWrongException( const char* expected, std::exception& actual, const char* action );

StringServer* quickCreateServer( NATIVE_IP_ADDRESS address, unsigned short port );

void quickReleaseServer( StringServer* server );