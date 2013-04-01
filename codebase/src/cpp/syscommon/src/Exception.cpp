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

#include "Exception.h"

using namespace SysCommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Exception::Exception( const tchar* message )
{
	this->message = message;
	this->name = TEXT("Exception");
}

Exception::Exception( const Exception& other )
{
	this->message = other.message;
	this->name = other.name;
}

Exception::~Exception()
{

}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
const tchar* Exception::getMessage() const
{
	return message.c_str();
}

const tchar* Exception::getName() const
{
	return name.c_str();
}

void Exception::setName( const tchar* name )
{
	this->name = name;
}
