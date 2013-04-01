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
	class Exception
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			String name;
			String message;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public:
			Exception( const tchar* message );
			Exception( const Exception& other );
			virtual ~Exception();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			const tchar* getMessage() const;
			const tchar* getName() const;

		protected:
			void setName( const tchar* name );
		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
	};

	#define EXCEPTION_CHILD_DECL(T,S) \
		class T : public S { public: T(const tchar* message) : S(message){ setName(#T); } virtual ~T() {} };

	EXCEPTION_CHILD_DECL( InterruptedException, Exception )
	EXCEPTION_CHILD_DECL( IOException, Exception )
	EXCEPTION_CHILD_DECL( SocketException, IOException )
	EXCEPTION_CHILD_DECL( FileNotFoundException, IOException )
}

