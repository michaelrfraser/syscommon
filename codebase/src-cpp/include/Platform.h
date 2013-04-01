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

// WIN32
#ifdef _WIN32
	#include <windows.h>
	#include <winsock.h>

	// Semaphores
	#define NATIVE_SEMAPHORE			HANDLE
	#define NATIVE_SEMAPHORE_UNINIT		INVALID_HANDLE_VALUE

	// Threads
	#define NATIVE_THREAD				HANDLE
	#define NATIVE_THREAD_UNINIT		INVALID_HANDLE_VALUE
	#define NATIVE_THREAD_ID			DWORD
	#define NATIVE_INTERRUPT			HANDLE
	#define NATIVE_INTERRUPT_UNINIT		INVALID_HANDLE_VALUE
	#define NATIVE_THREAD_PROC			LPTHREAD_START_ROUTINE

	// Sockets
	#define NATIVE_SOCKET				SOCKET
	#define NATIVE_SOCKET_UNINIT		INVALID_SOCKET
	#define NATIVE_IP_ADDRESS			u_long

	// Critical Sections
	#define NATIVE_CRITICALSECTION		CRITICAL_SECTION
#else	
	#include <semaphore.h>

	// Semaphores
	#define NATIVE_SEMAPHORE			sem_t
#endif

#include <string>

namespace SysCommon
{		
#ifndef UNICODE  
	typedef std::string String;
	typedef char tchar;
	#define totupper ::toupper
	#define tcslen ::strlen
	#define _ltot_s _ltoa_s
	#define _ttol atol
 #else
	typedef std::wstring String;
	typedef wchar_t tchar;
	#define totupper ::towupper
	#define tcslen ::wcslen
	#define _ltot_s _ltow_s
	#define _ttol wtol
#endif

	enum WaitResult
	{
		WR_FAILED, 
		WR_SUCCEEDED, 
		WR_TIMEOUT, 
		WR_INTERRUPTED,
		WR_ABANDONED
	};

	/**
	 * The Platform class encapsulates all system call functionality that is implemented 
	 * differently across the platforms that SysCommon supports.
	 *
	 * The services are purposefully provided under one single class to alleviate the spread of
	 * platform specific hash defines throughout the codebase. If you find yourself writing a
	 * platform based hash define in a class method implementation, it probably should be delegated
	 * to this class to keep things neat.
	 */
	class Platform
	{
		//----------------------------------------------------------
		//                    STATIC VARIABLES
		//----------------------------------------------------------
		private:
			static Platform lifecycleSingleton;

		//----------------------------------------------------------
		//                   INSTANCE VARIABLES
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                      CONSTRUCTORS
		//----------------------------------------------------------
		private:
			Platform();
			virtual ~Platform();

		//----------------------------------------------------------
		//                    INSTANCE METHODS
		//----------------------------------------------------------

		//----------------------------------------------------------
		//                     STATIC METHODS
		//----------------------------------------------------------
		public:
			// Semaphores
			static bool initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
											 unsigned int maximumCount, 
											 const String& name );
			static bool destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore );
			static WaitResult waitOnSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
											   NATIVE_INTERRUPT& threadInterrupt, 
											   DWORD timeout );
			static bool releaseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore );
	
			// Critical Sections
			static void initialiseCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void destroyCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void enterCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void leaveCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );

			// Threads
			static bool initialiseThread( NATIVE_THREAD& nativeThread, 
										  NATIVE_THREAD_ID& nativeThreadID, 
										  NATIVE_THREAD_PROC startRoutine, 
										  void* argument );
			static bool destroyThread( NATIVE_THREAD& nativeThread );
			static bool resumeThread( NATIVE_THREAD& nativeThread );
			static WaitResult joinThread( NATIVE_THREAD& nativeThread, 
										  NATIVE_INTERRUPT& threadInterrupt, 
										  DWORD timeout );
			static NATIVE_THREAD_ID getCurrentThreadID();

			static bool initialiseThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt, 
												   const String& name );
			static bool destroyThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt );
			static WaitResult performInterruptableSleep( NATIVE_INTERRUPT& threadInterrupt, 
														 DWORD timeout );
			static bool signalInterrupt( NATIVE_INTERRUPT& nativeInterrupt );

			// Sockets
			static bool initialiseSocketFramework();
			static void cleanupSocketFramework();
			static NATIVE_IP_ADDRESS lookupHost( const tchar* hostName );
			static int lookupHostName( NATIVE_IP_ADDRESS address, 
									   int addressType, 
									   tchar* outBuffer, 
									   int outBufferSize );
			static int getHostAddress( NATIVE_IP_ADDRESS address, 
									   int addressType, 
									   tchar* outBuffer, 
									   int outBufferSize );
			static const tchar* describeLastSocketError();

			// String helpers
			static int toAnsiChars( const wchar_t* string, 
									int stringLength, 
									char* outBuffer, 
									int outBufferSize );
			static std::string toAnsiString( const char* string );
			static std::string toAnsiString( const char* string, int length );
			static std::string toAnsiString( const wchar_t* string );
			static std::string toAnsiString( const wchar_t* string, int length );

			static int toUnicodeChars( const char* string, 
									   int stringLength, 
									   wchar_t* outBuffer, 
									   int outBufferSize );
			static std::wstring toUnicodeString( const char* string );
			static std::wstring toUnicodeString( const char* string, int length );
			static std::wstring toUnicodeString( const wchar_t* string );
			static std::wstring toUnicodeString( const wchar_t* string, int length );

			static int toPlatformChars( const char* string, 
										int stringLength, 
										tchar* outBuffer, 
										int outBufferSize );
			static String toPlatformString( const char* string,
											int length );
			static String toPlatformString( const char* string );
			static String toPlatformString( const wchar_t* string );

			// Files
			static bool fileExists( const tchar* fileName );

#ifdef _WIN32
		// Win32 Specific
		private:
			static WaitResult translateWaitResult( DWORD nativeWaitResult );
			static WaitResult waitOnInterruptableHandle( HANDLE interruptableHandle, 
														 HANDLE interruptHandle, 
														 DWORD timeout );
#endif
	};
}