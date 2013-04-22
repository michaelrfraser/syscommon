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
	#define WIN32_LEAN_AND_MEAN 
	#define NOGDI
	#define NOUSER
	#define NOKERNEL
	#define NOSOUND

	#include <windows.h>
	#include <winsock.h>
	#include <map>

	#pragma warning( disable : 4290 )

	// Semaphores
	#define NATIVE_SEMAPHORE			HANDLE

	// Threads
	#define NATIVE_THREAD				HANDLE
	#define NATIVE_INTERRUPT			HANDLE
	#define NATIVE_THREAD_PROC			LPTHREAD_START_ROUTINE
	#define NATIVE_THREAD_CALL			WINAPI
	#define NATIVE_INFINITE_WAIT		INFINITE

	// Sockets
	#define NATIVE_SOCKET				SOCKET
	#define NATIVE_SOCKET_UNINIT		INVALID_SOCKET
	#define NATIVE_SOCKET_ERROR			-1
	#define NATIVE_IP_ADDRESS			u_long
	#define NATIVE_SOCKET_LEN			int

	// Critical Sections
	#define NATIVE_CRITICALSECTION		CRITICAL_SECTION

	// Conditions
	#define NATIVE_EVENT				HANDLE
#else	
#include <pthread.h>
#include <netinet/in.h>
#include <errno.h>
#include <semaphore.h>
#include <list>

	// Semaphores
	#define NATIVE_SEMAPHORE			sem_t*

	// Events
	struct EventListener
	{
		pthread_mutex_t mutex;
		pthread_cond_t condition;
	};

	struct WrappedEvent
	{
		bool initialised;
		std::list<EventListener*> listenerList;
		pthread_cond_t noMoreListenersEvent;
		pthread_mutex_t mutex;
		bool state;
	};

	// Threads
	struct WrappedThread
	{
		bool initialised;
		pthread_t thread;
		WrappedEvent resumeEvent;

		bool operator <( const WrappedThread& other ) const
		{
		   return this->thread < other.thread;
		}
	};

	struct ThreadEntryData
	{
		WrappedThread* nativeThread;
		void* argument;

	};

	#define NATIVE_EVENT				WrappedEvent

	#define NATIVE_THREAD				WrappedThread
	#define NATIVE_INTERRUPT			WrappedEvent
	#define NATIVE_THREAD_PROC			void*
	#define NATIVE_THREAD_CALL
	#define NATIVE_INFINITE_WAIT		ULONG_MAX

	// Sockets
	#define NATIVE_SOCKET				int
	#define NATIVE_SOCKET_UNINIT		-1
	#define NATIVE_SOCKET_ERROR			-1
	#define NATIVE_IP_ADDRESS			u_int32_t
	#define NATIVE_SOCKET_LEN			socklen_t

	// Critical Sections
	#define NATIVE_CRITICALSECTION		pthread_mutex_t
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
	#define tcstok ::strtok

	#ifndef _WIN32
	#define TEXT(T) T
	#endif

 #else
	typedef std::wstring String;
	typedef wchar_t tchar;
	#define totupper ::towupper
	#define tcslen ::wcslen
	#define _ltot_s _ltow_s
	#define _ttol wtol
	#define tcstok wcstok

	#ifndef _WIN32
	#define TEXT(T) L##T
	#endif
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

		public:
			static const tchar* DIRECTORY_SEPARATOR;
			static const tchar* PATH_SEPARATOR;

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
			static NATIVE_SEMAPHORE createUninitialisedSemaphore();
			static bool initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
											 unsigned int maximumCount, 
											 const String& name );
			static bool isSemaphoreInitialised( const NATIVE_SEMAPHORE nativeSemaphore );
			static bool destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore );
			static WaitResult waitOnSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
											   NATIVE_INTERRUPT& threadInterrupt, 
											   unsigned long timeout );
			static bool releaseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore );
	
			// Critical Sections
			static void initialiseCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void destroyCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void enterCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );
			static void leaveCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection );

			// Threads
			static NATIVE_THREAD createUninitialisedThread();
			static bool initialiseThread( NATIVE_THREAD& nativeThread, 
										  void* argument );
			static bool isThreadInitialised( const NATIVE_THREAD& nativeThread );
			static bool destroyThread( NATIVE_THREAD& nativeThread );
			static bool resumeThread( NATIVE_THREAD& nativeThread );
			static WaitResult joinThread( NATIVE_THREAD& nativeThread, 
										  NATIVE_INTERRUPT& threadInterrupt, 
										  unsigned long timeout );
			static NATIVE_THREAD getCurrentThreadHandle();
			static bool threadHandlesEqual( NATIVE_THREAD handleOne, NATIVE_THREAD handleTwo );

			static NATIVE_INTERRUPT createUninitialisedInterrupt();
			static bool initialiseThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt, 
												   const String& name );
			static bool isThreadInterruptInitialised( const NATIVE_INTERRUPT& nativeInterrupt );
			static bool destroyThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt );
			static WaitResult performInterruptableSleep( NATIVE_INTERRUPT& threadInterrupt, 
														 unsigned long timeout );
			static bool signalInterrupt( NATIVE_INTERRUPT& nativeInterrupt );

			// Events
			static NATIVE_EVENT createUninitialisedEvent();
			static bool initialiseEvent( NATIVE_EVENT& event, bool initialState, const String& name );
			static bool isEventInitialised( const NATIVE_EVENT& event );
			static bool signalEvent( NATIVE_EVENT& event );
			static void clearEvent( NATIVE_EVENT& event );
			static WaitResult waitOnEvent( NATIVE_EVENT& event, unsigned long timeout );
			static bool destroyEvent( NATIVE_EVENT& event );

			// Sockets
			static bool initialiseSocketFramework();
			static void cleanupSocketFramework();
			static NATIVE_IP_ADDRESS lookupHost( const tchar* hostName );
			static void setMulticastSocketOptions( NATIVE_SOCKET& socket );
			static int lookupHostName( NATIVE_IP_ADDRESS address, 
									   int addressType, 
									   tchar* outBuffer, 
									   int outBufferSize );
			static int getHostAddress( NATIVE_IP_ADDRESS address, 
									   int addressType, 
									   tchar* outBuffer, 
									   int outBufferSize );
			static const int closeSocket( NATIVE_SOCKET socket );
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

			static void longToChars( const long value, tchar* buffer, size_t length );

			// Files
			static bool fileExists( const tchar* fileName );
			static String getCurrentDirectoryString();

			// Time
			static unsigned long getCurrentTimeMilliseconds();
			static bool getRandomBytes( char* buffer, size_t length );

#ifdef _WIN32
		// Win32 Specific
		private:
			// The handle returned by ::GetCurrentThread() is only a psuedo-handle, and 
			// trying to call ::DuplicateHandle() on it gives a different result each time.
			// So, we have to store the unique Thread ID returned by ::CreateThread() against
			// the handle it gave as at that point so that we can look it up in the future.
			//
			// An entry for a thread is inserted when it is created in Platform::initialiseThread()
			// and removed when it is destroyed in Platform::destroyThread()
			static std::map<DWORD,HANDLE> threadIdToHandleMap;

			static DWORD WINAPI threadEntry( LPVOID argument );
			static WaitResult translateWaitResult( DWORD nativeWaitResult, bool interruptIsFirstHandle );
			static WaitResult waitOnInterruptableHandle( HANDLE interruptableHandle, 
														 HANDLE interruptHandle, 
														 DWORD timeout );
#else
			static void* threadEntry( void* argument );
#endif
	};
}
