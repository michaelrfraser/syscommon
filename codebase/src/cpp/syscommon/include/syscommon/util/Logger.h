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

#include <stdarg.h>

#include "syscommon/Platform.h"
#include "syscommon/concurrent/Lock.h"

namespace syscommon
{
	class Logger
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		public:
			enum Level { LL_OFF,
						 LL_FATAL,
						 LL_ERROR,
						 LL_WARN,
						 LL_INFO,
						 LL_DEBUG, 
						 LL_TRACE };

			static const int MAX_MSG_LENGTH = 4096;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------
		private:
			FILE* file;
			Level level;
			Lock lock;

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		public: 
			Logger();
			virtual ~Logger();
				
		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------
		public:
			bool start( const tchar* filePath, bool append );
			void stop();
			bool isStarted() const;

			Level getLevel() const;
			void setLevel( Level newLevel );

			// logging methods
			void fatal( const tchar* format, ... );
			void error( const tchar* format, ... );
			void warn ( const tchar* format, ... );
			void info ( const tchar* format, ... );
			void debug( const tchar* format, ... );
			void trace( const tchar* format, ... );

		private:
			void log( const tchar* level, const tchar* message );
			void log( const tchar* level, const tchar* format, va_list args );

			// this method will return true if messages for the given
			// level should be printed, false otherwise
			bool checkLevel( Level messageLevel ) const;

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
		public:
			static Level getLevelFromString( const tchar* level );
	};
}
