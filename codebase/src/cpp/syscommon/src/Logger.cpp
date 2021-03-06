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
#include "syscommon/util/Logger.h"

#include <cstdio>
#include <ctime>
#include "syscommon/util/StringUtils.h"

#ifdef DEBUG
#include "debug.h"
#endif

#pragma warning( disable : 4996 )

using namespace syscommon;

//----------------------------------------------------------
//                      CONSTRUCTORS
//----------------------------------------------------------
Logger::Logger()
{
	this->file = NULL;
	this->level = LL_INFO;
}

Logger::~Logger()
{
	if( isStarted() )
		stop();
}

//----------------------------------------------------------
//                    INSTANCE METHODS
//----------------------------------------------------------
bool Logger::start( const tchar* filePath, bool append )
{
	bool started = false;
	if( !isStarted() )
	{
		// File name must be converted to ansi, as that's what fopen() expects
		std::string ansiName = Platform::toAnsiString( filePath );
		if( !ansiName.empty() )
		{
			const char* mode = append ? "a" : "w";
			this->file = fopen( ansiName.c_str(), mode );

			started = this->file != NULL;
		}
	}

	return started;
}

void Logger::stop()
{
	if( isStarted() )
	{
		// Flush and clean up
		fflush( file );
		fclose( file );
		file = NULL;
	}
}

bool Logger::isStarted() const
{
	return this->file != NULL;
}

Logger::Level Logger::getLevel() const
{
	return this->level;
}

void Logger::setLevel( Level newLevel )
{
	this->level = newLevel;
}

void Logger::fatal( const tchar* format, ... )
{
	if( checkLevel(LL_FATAL) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT("[FATAL]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::error( const tchar* format, ... )
{
	if( checkLevel(LL_ERROR) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT("[ERROR]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::warn( const tchar* format, ... )
{
	if( checkLevel(LL_WARN) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT(" [WARN]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::info( const tchar* format, ... )
{
	if( checkLevel(LL_INFO) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT(" [INFO]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::debug( const tchar* format, ... )
{
	if( checkLevel(LL_DEBUG) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT("[DEBUG]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::trace( const tchar* format, ... )
{
	if( checkLevel(LL_TRACE) )
	{
		// start the var-arg stuff
		va_list args;
		va_start( args, format );
		log( TEXT("[TRACE]"), format, args );
		// do the varargs cleanup
		va_end( args );
	}
}

void Logger::log( const tchar* level, const tchar* message )
{
	lock.lock();
	if( isStarted() )
	{
		String time = StringUtils::formatTime( ::time(NULL), TEXT("%Y-%m-%d %H:%M:%S") );

#ifndef UNICODE
		fprintf( file, TEXT("%s %s %s\n"), time.c_str(), level, message );
#else
		fwprintf( file, TEXT("%ls %ls %ls\n"), time.c_str(), level, message );
#endif
		fflush( file );
	}
	lock.unlock();
}

void Logger::log( const tchar* level, const tchar* format, va_list args )
{
	lock.lock();
	if( isStarted() )
	{
		String time = StringUtils::formatTime( ::time(NULL), TEXT("%Y-%m-%d %H:%M:%S") );

#ifndef UNICODE
		char buffer[MAX_MSG_LENGTH];
		vsprintf( buffer, format, args );

		fprintf( file, TEXT("%s %s %s\n"), time.c_str(), level, buffer );
#else
		
#endif

		fflush( file );
	}
	lock.unlock();
}

bool Logger::checkLevel( Level messageLevel ) const
{
	bool levelEnabled = messageLevel <= this->level;
	return levelEnabled;
}

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
Logger::Level Logger::getLevelFromString( const tchar* string )
{
	Logger::Level level = Logger::LL_INFO;
	String asStdString( string );
	if( StringUtils::stringEquals(asStdString, TEXT("OFF")) )
		level = Logger::LL_OFF;
	else if( StringUtils::stringEquals(asStdString, TEXT("FATAL")) )
		level = Logger::LL_FATAL;
	else if( StringUtils::stringEquals(asStdString, TEXT("ERROR")) )
		level = Logger::LL_ERROR;
	else if( StringUtils::stringEquals(asStdString, TEXT("WARN")) )
		level = Logger::LL_WARN;
	else if( StringUtils::stringEquals(asStdString, TEXT("INFO")) )
		level = Logger::LL_INFO;
	else if( StringUtils::stringEquals(asStdString, TEXT("DEBUG")) )
		level = Logger::LL_DEBUG;
	else if( StringUtils::stringEquals(asStdString, TEXT("TRACE")) )
		level = Logger::LL_TRACE;

	return level;
}
