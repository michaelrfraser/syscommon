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

#include "assert.h"
#include <stdlib.h>
#include "concurrent/Thread.h"

using namespace SysCommon;

#ifdef _WIN32
std::map<DWORD,HANDLE> Platform::threadIdToHandleMap;

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
NATIVE_SEMAPHORE Platform::createUninitialisedSemaphore()
{
	return INVALID_HANDLE_VALUE;
}

bool Platform::initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
								    unsigned int maximumCount, 
									const String& name )
{
	assert( !Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( !Platform::isSemaphoreInitialised(nativeSemaphore) )
	{
		nativeSemaphore = ::CreateSemaphore( NULL, 
											 maximumCount, 
											 maximumCount, 
											 name.c_str() );
		result = Platform::isSemaphoreInitialised(nativeSemaphore);
	}

	return result;
}

bool Platform::isSemaphoreInitialised( const NATIVE_SEMAPHORE nativeSemaphore )
{
	return nativeSemaphore != INVALID_HANDLE_VALUE;
}

bool Platform::destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) )
	{
		result = ::CloseHandle( nativeSemaphore ) != FALSE;

		if ( result )
			nativeSemaphore = INVALID_HANDLE_VALUE;
	}
	
	return result;
}

WaitResult Platform::waitOnSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
									  NATIVE_INTERRUPT& threadInterrupt, 
									  DWORD timeout )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );
	assert( Platform::isThreadInterruptInitialised(threadInterrupt) );

	WaitResult result = WR_FAILED;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) &&
		 Platform::isThreadInterruptInitialised(threadInterrupt) )
	{
		result = Platform::waitOnInterruptableHandle( threadInterrupt, nativeSemaphore, timeout );
	}

	return result;
}

bool Platform::releaseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) )
		result = ::ReleaseSemaphore( nativeSemaphore, 1, NULL ) != FALSE;
	
	return result;
}

void Platform::initialiseCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::InitializeCriticalSection( &nativeCriticalSection );
}

void Platform::destroyCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::DeleteCriticalSection( &nativeCriticalSection );
}

void Platform::enterCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::EnterCriticalSection( &nativeCriticalSection );
}

void Platform::leaveCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::LeaveCriticalSection( &nativeCriticalSection );
}

NATIVE_THREAD Platform::createUninitialisedThread()
{
	return INVALID_HANDLE_VALUE;
}

bool Platform::initialiseThread( NATIVE_THREAD& nativeThread, 
								 void* thisPointer )
{
	assert( !Platform::isThreadInitialised(nativeThread) );
	assert( thisPointer );

	bool result = false;
	if ( !Platform::isThreadInitialised(nativeThread) && thisPointer )
	{
		DWORD nativeThreadID = 0;
		nativeThread = ::CreateThread( NULL, 
									   NULL, 
									   Platform::threadEntry,
									   thisPointer, 
									   CREATE_SUSPENDED, 
									   &nativeThreadID );
		result = nativeThread != INVALID_HANDLE_VALUE;

		// Map the thread handle to the Win32 Thread ID (see notes in header file for more
		// info regarding why this is necessary)
		if( result )
			threadIdToHandleMap[nativeThreadID] = nativeThread;
	}

	return result;
}

bool Platform::isThreadInitialised( const NATIVE_THREAD& nativeThread )
{
	return nativeThread != INVALID_HANDLE_VALUE;
}

bool Platform::destroyThread( NATIVE_THREAD& nativeThread )
{
	assert( Platform::isThreadInitialised(nativeThread) );

	bool result = false;
	if ( Platform::isThreadInitialised(nativeThread) )
	{
		// Unmap the handle from the corresponding Win32 Thread ID
		DWORD threadId = ::GetThreadId( nativeThread );
		threadIdToHandleMap.erase( threadId );

		result = ::CloseHandle( nativeThread ) != FALSE;

		if ( result )
			nativeThread = INVALID_HANDLE_VALUE;
	}

	return result;
}

bool Platform::resumeThread( NATIVE_THREAD& nativeThread )
{
	assert( Platform::isThreadInitialised(nativeThread) );

	bool result = false;
	if ( Platform::isThreadInitialised(nativeThread) )
	{
		::ResumeThread( nativeThread );
		result = true;
	}

	return result;
}

WaitResult Platform::joinThread( NATIVE_THREAD& nativeThread, 
								 NATIVE_INTERRUPT& threadInterrupt, 
								 DWORD timeout )
{
	assert( Platform::isThreadInitialised(nativeThread) );
	assert( threadInterrupt != INVALID_HANDLE_VALUE );

	WaitResult result = WR_FAILED;
	if ( Platform::isThreadInitialised(nativeThread) && threadInterrupt != INVALID_HANDLE_VALUE )
		result = Platform::waitOnInterruptableHandle( nativeThread, threadInterrupt, timeout );

	return result;
}

NATIVE_THREAD Platform::getCurrentThreadHandle()
{
	NATIVE_THREAD handle = INVALID_HANDLE_VALUE;

	// Get the unique Win32 Thread Id
	DWORD currentId = ::GetCurrentThreadId();

	// Get the handle that was assigned to the Thread ID when the thread was created
	std::map<DWORD,HANDLE>::iterator handleIt = threadIdToHandleMap.find( currentId );
	if( handleIt != threadIdToHandleMap.end() )
		handle = (*handleIt).second;

	return handle;
}

bool Platform::threadHandlesEqual( NATIVE_THREAD handleOne, NATIVE_THREAD handleTwo )
{
	return handleOne == handleTwo;
}

NATIVE_INTERRUPT Platform::createUninitialisedInterrupt()
{
	return INVALID_HANDLE_VALUE;
}

bool Platform::initialiseThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt, const String& name )
{
	assert( !Platform::isThreadInterruptInitialised(nativeInterrupt) );

	bool result = false;
	if ( !Platform::isThreadInterruptInitialised(nativeInterrupt) )
	{
		nativeInterrupt = ::CreateEvent( NULL, FALSE, FALSE, NULL );
		result = Platform::isThreadInterruptInitialised( nativeInterrupt );
	}

	return result;
}

bool Platform::isThreadInterruptInitialised( const NATIVE_INTERRUPT& nativeInterrupt )
{
	return nativeInterrupt != INVALID_HANDLE_VALUE;
}

bool Platform::destroyThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	assert( Platform::isThreadInterruptInitialised(nativeInterrupt) );

	bool result = false;
	if ( Platform::isThreadInterruptInitialised(nativeInterrupt) )
	{
		result = ::CloseHandle( nativeInterrupt ) != FALSE;

		if ( result )
			nativeInterrupt = INVALID_HANDLE_VALUE;
	}

	return result;
}

WaitResult Platform::performInterruptableSleep( NATIVE_INTERRUPT& threadInterrupt, DWORD timeout )
{
	assert( Platform::isThreadInterruptInitialised(threadInterrupt) );

	WaitResult result = WR_FAILED;
	if ( Platform::isThreadInterruptInitialised(threadInterrupt) )
	{
		DWORD nativeWaitResult = ::WaitForSingleObject( threadInterrupt, timeout );
		result = Platform::translateWaitResult( nativeWaitResult, true );
	}

	return result;
}

bool Platform::signalInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	assert( Platform::isThreadInterruptInitialised(nativeInterrupt) );

	bool result = false;
	if ( Platform::isThreadInterruptInitialised(nativeInterrupt) )
		result = ::SetEvent( nativeInterrupt ) != FALSE;

	return result;
}

NATIVE_EVENT Platform::createUninitialisedEvent()
{
	return INVALID_HANDLE_VALUE;
}

bool Platform::initialiseEvent( NATIVE_EVENT& event, bool initialState, const String& name )
{
	assert( !Platform::isEventInitialised(event) );

	bool initialised = false;
	if( !Platform::isEventInitialised(event) )
	{
		event = ::CreateEvent( NULL, TRUE, initialState, NULL );
		initialised = event != INVALID_HANDLE_VALUE;
	}

	return initialised;
}

bool Platform::isEventInitialised( const NATIVE_EVENT& event )
{
	return event != INVALID_HANDLE_VALUE;
}

bool Platform::signalEvent( NATIVE_EVENT& event )
{
	assert( Platform::isEventInitialised(event) );

	bool signaled = false;
	if( Platform::isEventInitialised(event) )
		signaled = (::SetEvent(event) != FALSE);

	return signaled;
}

void Platform::clearEvent( NATIVE_EVENT& event )
{
	assert( Platform::isEventInitialised(event) );
	if( Platform::isEventInitialised(event) )
		::ResetEvent( event );
}

WaitResult Platform::waitOnEvent( NATIVE_EVENT& event, unsigned long timeout )
{
	assert( Platform::isEventInitialised(event) );
	WaitResult result = WR_FAILED;
	if( Platform::isEventInitialised(event) )
	{
		DWORD nativeResult = ::WaitForSingleObject( event, timeout );
		result = Platform::translateWaitResult( nativeResult, false );
	}

	return result;
}

bool Platform::destroyEvent( NATIVE_EVENT& event )
{
	assert( Platform::isEventInitialised(event) );

	bool destroyed = false;
	if( Platform::isEventInitialised(event) )
		destroyed = (::CloseHandle( event ) != FALSE);

	return destroyed;

}

bool Platform::initialiseSocketFramework()
{
	WSAData wsaInfo;
	int startupResult = ::WSAStartup( MAKEWORD(1, 1), &wsaInfo );

	return startupResult == 0;
}

void Platform::cleanupSocketFramework()
{
	::WSACleanup();
}

NATIVE_IP_ADDRESS Platform::lookupHost( const tchar* hostName )
{
	NATIVE_IP_ADDRESS result = INADDR_NONE;

	bool frameworkInitialised = Platform::initialiseSocketFramework();

	if ( frameworkInitialised )
	{
		std::string narrowHostName = Platform::toAnsiString( hostName );
		if( !narrowHostName.empty() )
		{
			const char* narrowHostNameCStr = narrowHostName.c_str();

			// Check to see if it is a simple xxx.xxx.xxx.xxx
			result = ntohl(::inet_addr(narrowHostNameCStr) );
			if ( result == INADDR_NONE )
			{
				// Not in 4xOctet form, so look up the host name
				hostent* hostEntry = ::gethostbyname( narrowHostNameCStr );
				if ( hostEntry != NULL )
				{
					result = ntohl(*((NATIVE_IP_ADDRESS*)hostEntry->h_addr_list[0]));
				}
			}
		}
		
		Platform::cleanupSocketFramework();
	}

	return result;
}

void Platform::setMulticastSocketOptions( NATIVE_SOCKET& socket )
{
	// Set SO_REUSEADDR so that other processes can bind to it
	int intTrue = 1;
	::setsockopt( socket,
				  SOL_SOCKET, 
				  SO_REUSEADDR, 
				  (char*)&intTrue,
				  sizeof(intTrue) );
}

int Platform::lookupHostName( NATIVE_IP_ADDRESS address, 
							  int addressType, 
							  tchar* outBuffer, 
							  int outBufferSize )
{
	int returnSize = 0;

	bool frameworkInitialised = Platform::initialiseSocketFramework();
	if( frameworkInitialised )
	{
		in_addr addressRequest;
		addressRequest.S_un.S_addr = htonl(address);
		hostent* host = ::gethostbyaddr( (const char*)&addressRequest, 
										 sizeof(in_addr), 
										 addressType );
		if( host )
		{
			const char* name = host->h_name;
			returnSize = Platform::toPlatformChars( name, (int)strlen(name), outBuffer, outBufferSize );
		}

		Platform::cleanupSocketFramework();
	}
	return returnSize;
}

int Platform::getHostAddress( NATIVE_IP_ADDRESS address, 
							  int addressType, 
							  tchar* outBuffer, 
							  int outBufferSize )
{
	int returnSize = 0;
	bool frameworkInitialised = Platform::initialiseSocketFramework();
	if( frameworkInitialised )
	{
		in_addr addressRequest;
		addressRequest.S_un.S_addr = htonl(address);
		char* ansiAddress = ::inet_ntoa( addressRequest );
		if( ansiAddress )
		{
			returnSize = Platform::toPlatformChars( ansiAddress, 
													(int)strlen(ansiAddress), 
													outBuffer, 
													outBufferSize );
		}

		Platform::cleanupSocketFramework();
	}
	return returnSize;
}

const int Platform::closeSocket( NATIVE_SOCKET socket )
{
	return ::closesocket( socket );
}

const tchar* Platform::describeLastSocketError()
{
	const tchar* error = TEXT("Unknown Error");
	int lastError = ::WSAGetLastError();

	switch( lastError )
	{
		case WSAEACCES:
			error = TEXT("Permission denied");
			break;

		case WSAEADDRINUSE:
			error = TEXT("Address already in use");
			break;

		case WSAEADDRNOTAVAIL:
			error = TEXT("Cannot assign requested address");
			break;

		case WSAEAFNOSUPPORT:
			error = TEXT("Address family not supported by protocol family");
			break;

		case WSAEALREADY:
			error = TEXT("Operation already in progress");
			break;

		case WSAEBADF:
			error = TEXT("Bad file descriptor");
			break;

		case WSAECONNABORTED:
			error = TEXT("Software caused connection abort");
			break;

		case WSAECONNREFUSED:
			error = TEXT("Connection refused");
			break;

		case WSAECONNRESET:
			error = TEXT("Connection reset");
			break;

		case WSAEDESTADDRREQ:
			error = TEXT("Destination address required");
			break;

		case WSAEDQUOT:
			error = TEXT("Disc quota exceeded");
			break;

		case WSAEFAULT:
			error = TEXT("Bad address");
			break;

		case WSAEHOSTDOWN:
			error = TEXT("Host is down");
			break;

		case WSAEHOSTUNREACH:
			error = TEXT("No route to host");
			break;

		case WSAEINPROGRESS:
			error = TEXT("Operation now in progress");
			break;

		case WSAEINTR:
			error = TEXT("Interrupted function call");
			break;

		case WSAEINVAL:
			error = TEXT("Invalid argument");
			break;

		case WSAEISCONN:
			error = TEXT("Socket is already connected");
			break;

		case WSAELOOP:
			error = TEXT("Too many levels of symbolic links");
			break;

		case WSAEMFILE:
			error = TEXT("Too many open files");
			break;

		case WSAEMSGSIZE:
			error = TEXT("Message too long");
			break;

		case WSAENAMETOOLONG:
			error = TEXT("File name too long");
			break;

		case WSAENETDOWN:
			error = TEXT("Network is down");
			break;

		case WSAENETRESET:
			error = TEXT("Network dropped connection on reset");
			break;

		case WSAENETUNREACH:
			error = TEXT("Network is unreachable");
			break;

		case WSAENOBUFS:
			error = TEXT("No buffer space available");
			break;

		case WSAENOPROTOOPT:
			error = TEXT("Bad protocol option");
			break;

		case WSAENOTCONN:
			error = TEXT("Socket is not connected");
			break;

		case WSAENOTEMPTY:
			error = TEXT("Directory not empty");
			break;

		case WSAENOTSOCK:
			error = TEXT("Socket operation on non-socket");
			break;

		case WSAEOPNOTSUPP:
			error = TEXT("Operation not suppprted");
			break;

		case WSAEPFNOSUPPORT:
			error = TEXT("Protocol family not supported");
			break;

		case WSAEPROCLIM:
			error = TEXT("Too many processes");
			break;

		case WSAEPROTONOSUPPORT:
			error = TEXT("Protocol not supported");
			break;

		case WSAEPROTOTYPE:
			error = TEXT("Protocol wrong type for socket");
			break;

		case WSAEREMOTE:
			error = TEXT("Too many levels of remote in path");
			break;

		case WSAESHUTDOWN:
			error = TEXT("Cannot send after socket shutdown");
			break;

		case WSAESOCKTNOSUPPORT:
			error = TEXT("Socket type not supported");
			break;

		case WSAESTALE:
			error = TEXT("Stale NFS file handle");
			break;

		case WSAETIMEDOUT:
			error = TEXT("Connection timed out");
			break;

		case WSAETOOMANYREFS:
			error = TEXT("Too many references, can't splice");
			break;

		case WSAEUSERS:
			error = TEXT("Too many users");
			break;

		case WSAEWOULDBLOCK:
			error = TEXT("Resource temporarily unavailable");
			break;

		case WSAHOST_NOT_FOUND:
			error = TEXT("Host not found");
			break;

		case WSANOTINITIALISED:
			error = TEXT("Successful WSAStartup() not yet performed");
			break;

		case WSANO_DATA:
			error = TEXT("Valid name, no data record of requested type");
			break;

		case WSANO_RECOVERY:
			error = TEXT("Non-recoverable error");
			break;

		case WSASYSNOTREADY:
			error = TEXT("Network subsystem is unavailable");
			break;

		case WSATRY_AGAIN:
			error = TEXT("Non-authoritative host not found");
			break;

		case WSAVERNOTSUPPORTED:
			error = TEXT("WINSOCK.DLL version out of range");
			break;

	}

	return error;
}

int Platform::toAnsiChars( const wchar_t* string, 
						   int stringLength, 
						   char* outBuffer, 
						   int outBufferSize )
{
	int result = ::WideCharToMultiByte( CP_ACP, 
										NULL, 
										string, 
										stringLength, 
										outBuffer, 
										outBufferSize, 
										NULL, 
										NULL );
	return result;
}

std::string Platform::toAnsiString( const char* string )
{
	return std::string( string );
}

std::string Platform::toAnsiString( const char* string, int length )
{
	return std::string( string, length );
}

std::string Platform::toAnsiString( const wchar_t* string )
{
	return toAnsiString( string, (int)::wcslen(string) );
}

std::string Platform::toAnsiString( const wchar_t* string, int length )
{
	std::string result;

	int ansiLength = Platform::toAnsiChars( string, length, NULL, 0 );
	if ( ansiLength > 0 )
	{
		char* ansiChars = new char[ ansiLength ];
		Platform::toAnsiChars( string, length, ansiChars, ansiLength );
		result = std::string( ansiChars, ansiLength );

		delete[] ansiChars;
	}	

	return result;
}

int Platform::toUnicodeChars( const char* string, 
							  int stringLength, 
							  wchar_t* outBuffer, 
							  int outBufferSize )
{
	int result = ::MultiByteToWideChar( CP_ACP, 0, string, stringLength, outBuffer, outBufferSize );
	return result;
}

std::wstring Platform::toUnicodeString( const char* string )
{
	return Platform::toUnicodeString( string, (int)strlen(string) );
}

std::wstring Platform::toUnicodeString( const char* string, int length )
{
	std::wstring result;
	
	int unicodeLength = Platform::toUnicodeChars( string, length, NULL, 0 );
	if( unicodeLength > 0 )
	{
		wchar_t* unicodeChars = new wchar_t[ unicodeLength ];
		Platform::toUnicodeChars( string, length, unicodeChars, unicodeLength );
		result = std::wstring( unicodeChars, unicodeLength );

		delete[] unicodeChars;
	}

	return result;
}

std::wstring Platform::toUnicodeString( const wchar_t* string )
{
	return std::wstring( string );
}

std::wstring Platform::toUnicodeString( const wchar_t* string, int length )
{
	return std::wstring( string, length );
}

int Platform::toPlatformChars( const char* string, 
							   int stringLength, 
							   tchar* outBuffer, 
							   int outBufferSize )
{
	int size = 0;
#ifndef UNICODE
	size = stringLength;
	if( outBuffer != NULL )
	{
		::memcpy_s( (void*)outBuffer, 
					sizeof(tchar)* outBufferSize, 
					string, 
					sizeof(char) * stringLength );
		size = stringLength;
	}
#else
	size = Platform::toUnicodeChars( string, stringLength, outBuffer, outBufferSize );
#endif
	return size;
}

String Platform::toPlatformString( const char* string )
{
#ifndef UNICODE
	return std::string( string );	
#else
	return Platform::toUnicodeString( string );
#endif
}

String Platform::toPlatformString( const char* string, int length )
{
#ifndef UNICODE
	return Platform::toAnsiString( string, length );
#else
	return Platform::toUnicodeString( string, length );
#endif
}

String toPlatformString( const wchar_t* string )
{
#ifndef UNICODE
	return Platform::toAnsiString( string );
#else
	return std::wstring( string );
#endif
}

void Platform::longToChars( const long value, tchar* buffer, size_t length )
{
	::_ltot_s( value, buffer, length, 10 );
}

bool Platform::fileExists( const tchar* fileName )
{
	DWORD fileAttributes = ::GetFileAttributes( fileName );
	return fileAttributes != INVALID_FILE_ATTRIBUTES;
}

unsigned long Platform::getCurrentTimeMilliseconds()
{
	unsigned long time = 0;

	LARGE_INTEGER frequency;
	if( ::QueryPerformanceFrequency( &frequency ) )
	{
		LARGE_INTEGER now;
		::QueryPerformanceCounter( &now );

		time = (unsigned long)((1000LL * now.QuadPart) / frequency.QuadPart);
	}
	else
	{
		time = ::GetTickCount();
	}

	return time;
}

DWORD WINAPI Platform::threadEntry( LPVOID argument )
{
	Thread::threadEntry( argument );
	return 0;
}

WaitResult Platform::translateWaitResult( DWORD nativeWaitResult, bool interruptIsFirstHandle )
{
	WaitResult result = WR_FAILED;

	switch ( nativeWaitResult )
	{
		case WAIT_OBJECT_0:
			result = interruptIsFirstHandle ? WR_INTERRUPTED : WR_SUCCEEDED;
			break;

		case (WAIT_OBJECT_0 + 1):
			result = WR_SUCCEEDED;
			break;

		// Don't know if this is needed, apparently should only be for mutex objects
		case WAIT_ABANDONED_0:
		case (WAIT_ABANDONED_0 + 1):
			result = WR_ABANDONED;
			break;

		case WAIT_TIMEOUT:
			result = WR_TIMEOUT;
			break;

		case WAIT_FAILED:
		default:
			result = WR_FAILED;
			break;
	}

	return result;
}

WaitResult Platform::waitOnInterruptableHandle( HANDLE interruptableHandle, 
											    HANDLE interruptHandle, 
												DWORD timeout )
{
	WaitResult result = WR_FAILED;

	if ( interruptHandle != INVALID_HANDLE_VALUE && interruptableHandle  != INVALID_HANDLE_VALUE )
	{
		HANDLE waitHandles[2];
		waitHandles[0] = interruptHandle;
		waitHandles[1] = interruptableHandle;

		DWORD nativeWaitResult = ::WaitForMultipleObjects( 2, waitHandles, FALSE, timeout );
		result = translateWaitResult( nativeWaitResult, true );

		if( result == WR_INTERRUPTED )
			::ResetEvent( interruptHandle );
	}

	return result;
}

#else

#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>


//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
NATIVE_SEMAPHORE Platform::createUninitialisedSemaphore()
{
	return NULL;
}

bool Platform::initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
								    unsigned int maximumCount, 
									const String &name )
{
	assert( !Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( !Platform::isSemaphoreInitialised(nativeSemaphore) )
	{
		// MacOSX doesn't support in-process semaphores created by sem_init(), so we need to create
		// a 'grownup' semaphore instead with sem_open()

		// Semaphore path
		char semName[PATH_MAX];
		semName[0] = 0;
		sprintf( semName, "/tmp/sem_pid%ld_%s", (long)getpid(), name.c_str() );

		// Create the semaphore
		sem_t* createdSemaphore = ::sem_open( semName,
											  O_CREAT | O_EXCL,
											  S_IWUSR | S_IRUSR,
											  maximumCount );
		if( createdSemaphore != SEM_FAILED )
		{
			nativeSemaphore = createdSemaphore;
			result = true;
		}
	}

	return result;
}

bool Platform::isSemaphoreInitialised( const NATIVE_SEMAPHORE nativeSemaphore )
{
	return nativeSemaphore != NULL;
}

bool Platform::destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) )
	{
		int destroyResult = ::sem_close( nativeSemaphore );
		if ( destroyResult != -1 )
		{
			nativeSemaphore = NULL;
			result = true;
		}
	}

	return result;
}

WaitResult Platform::waitOnSemaphore( NATIVE_SEMAPHORE& nativeSemaphore,
									  NATIVE_INTERRUPT& threadInterrupt,
									  unsigned long timeout )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );
	assert( Platform::isThreadInterruptInitialised(threadInterrupt) );

	WaitResult result = WR_FAILED;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) /*&&
		 Platform::isThreadInterruptInitialised(threadInterrupt)*/ )
	{
		// Interruptable/timed wait on semaphores don't work for posix at the moment :( The docco
		// says however: "The sem_wait() function is interruptible by the delivery of a signal."
		//
		// Just need to work out how to implement that - don't have the time or the need at the
		// moment though.
		// TODO Looks like http://www.aao.gov.au/local/www/ks/uploads/sem_timedwait.c could be
		// a possible implementation
		int waitResult = ::sem_wait( nativeSemaphore );
		if( waitResult != -1 )
			result = WR_SUCCEEDED;
	}

	return result;
}

bool Platform::releaseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( Platform::isSemaphoreInitialised(nativeSemaphore) );

	bool result = false;
	if ( Platform::isSemaphoreInitialised(nativeSemaphore) )
	{
		int postResult = ::sem_post( nativeSemaphore );
		result = postResult != -1;
	}

	return result;
}

void Platform::initialiseCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	pthread_mutexattr_t attributes;
	::pthread_mutexattr_init( &attributes );
	::pthread_mutexattr_settype( &attributes, PTHREAD_MUTEX_RECURSIVE );

	int initResult = ::pthread_mutex_init( &nativeCriticalSection, &attributes );
	assert( initResult == 0 );

	::pthread_mutexattr_destroy( &attributes );
}

void Platform::destroyCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::pthread_mutex_destroy( &nativeCriticalSection );
}

void Platform::enterCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::pthread_mutex_lock( &nativeCriticalSection );
}

void Platform::leaveCriticalSection( NATIVE_CRITICALSECTION& nativeCriticalSection )
{
	::pthread_mutex_unlock( &nativeCriticalSection );
}

NATIVE_THREAD Platform::createUninitialisedThread()
{
	NATIVE_THREAD thread;
	thread.initialised = false;

	return thread;
}

bool Platform::initialiseThread( NATIVE_THREAD& nativeThread,
								 void* thisPointer )
{
	assert( !Platform::isThreadInitialised(nativeThread) );
	assert( thisPointer );

	bool result = false;
	if( !Platform::isThreadInitialised(nativeThread) && thisPointer )
	{
		nativeThread.resumeEvent.initialised = false;
		Platform::initialiseEvent( nativeThread.resumeEvent, false, TEXT("ThreadResume") );

		// This will be freed in the thread entry routine
		ThreadEntryData* entryData = (ThreadEntryData*)::malloc( sizeof(ThreadEntryData) );
		entryData->nativeThread = &nativeThread;
		entryData->argument = thisPointer;

		int createResult = ::pthread_create( &nativeThread.thread,
											 NULL,
											 Platform::threadEntry,
											 entryData );
		if( createResult == 0 )
		{
			nativeThread.initialised = true;
			result = true;
		}
	}

	return result;
}

bool Platform::isThreadInitialised( const NATIVE_THREAD& nativeThread )
{
	return nativeThread.initialised;
}

bool Platform::destroyThread( NATIVE_THREAD& nativeThread )
{
	assert( Platform::isThreadInitialised(nativeThread) );
	bool result = false;
	if( Platform::isThreadInitialised(nativeThread) )
	{
		nativeThread.initialised = false;
		Platform::destroyEvent( nativeThread.resumeEvent );
		result = true;
	}

	return result;
}

bool Platform::resumeThread( NATIVE_THREAD& nativeThread )
{
	assert( Platform::isThreadInitialised(nativeThread) );
	bool result = false;
	if( Platform::isThreadInitialised(nativeThread) )
	{
		result = Platform::signalEvent( nativeThread.resumeEvent );
	}

	return result;
}

WaitResult Platform::joinThread( NATIVE_THREAD& nativeThread,
								 NATIVE_INTERRUPT& threadInterrupt,
								 unsigned long timeout )
{
	assert( Platform::isThreadInitialised(nativeThread) );
	assert( Platform::isThreadInterruptInitialised(threadInterrupt) );

	WaitResult result = WR_FAILED;
	if( Platform::isThreadInitialised(nativeThread) &&
			Platform::isThreadInterruptInitialised(threadInterrupt) )
	{
		int joinResult = ::pthread_join( nativeThread.thread, NULL );
		if( joinResult == 0 )
			result = WR_SUCCEEDED;
	}

	return result;
}

NATIVE_THREAD Platform::getCurrentThreadHandle()
{
	NATIVE_THREAD self;
	self.initialised = true;
	self.thread = ::pthread_self();

	return self;
}

bool Platform::threadHandlesEqual( NATIVE_THREAD handleOne, NATIVE_THREAD handleTwo )
{
	return ::pthread_equal( handleOne.thread, handleTwo.thread ) != 0;
}

NATIVE_INTERRUPT Platform::createUninitialisedInterrupt()
{
	return Platform::createUninitialisedEvent();
}

bool Platform::initialiseThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt, const String& name )
{
	return Platform::initialiseEvent( nativeInterrupt, false, name );
}

bool Platform::isThreadInterruptInitialised( const NATIVE_INTERRUPT& nativeInterrupt )
{
	return Platform::isEventInitialised( nativeInterrupt );
}

bool Platform::destroyThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	return Platform::destroyEvent( nativeInterrupt );
}

WaitResult Platform::performInterruptableSleep( NATIVE_INTERRUPT& threadInterrupt,
												unsigned long timeout )
{
	assert( Platform::isThreadInterruptInitialised(threadInterrupt) );
	WaitResult result = WR_FAILED;

	if( Platform::isThreadInterruptInitialised(threadInterrupt) )
	{
		result = Platform::waitOnEvent( threadInterrupt, timeout );

		// If waitInEvent was returned a success, then the interrupt was called
		if( result == WR_SUCCEEDED )
			result = WR_INTERRUPTED;
	}

	return result;
}

bool Platform::signalInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	return Platform::signalEvent( nativeInterrupt );
}

NATIVE_EVENT Platform::createUninitialisedEvent()
{
	NATIVE_EVENT event;
	event.initialised = false;
	return event;
}

bool Platform::initialiseEvent( NATIVE_EVENT& event, bool initialState, const String& name )
{
	assert( !event.initialised );
	if( !event.initialised )
	{
		::pthread_mutex_init( &event.mutex, NULL );
		::pthread_cond_init( &event.noMoreListenersEvent, NULL );

		event.state = initialState;
		event.initialised = true;
	}

	return event.initialised;
}

bool Platform::isEventInitialised( const NATIVE_EVENT& event )
{
	return event.initialised;
}

bool Platform::signalEvent( NATIVE_EVENT& event )
{
	bool signaled = false;

	assert( event.initialised );
	if( event.initialised )
	{
		::pthread_mutex_lock( &event.mutex );
		event.state = true;
		signaled = true;

		std::list<EventListener*>::iterator listenerIterator = event.listenerList.begin();
		while( listenerIterator != event.listenerList.end() )
		{
			EventListener* listener = *listenerIterator;
			::pthread_mutex_lock( &listener->mutex );
			::pthread_cond_broadcast( &listener->condition );
			::pthread_mutex_unlock( &listener->mutex );

			++listenerIterator;
		}

		::pthread_mutex_unlock( &event.mutex );
	}

	return signaled;
}

void Platform::clearEvent( NATIVE_EVENT& event )
{
	assert( event.initialised );

	::pthread_mutex_lock( &event.mutex );
	event.state = false;
	::pthread_mutex_unlock( &event.mutex );
}

WaitResult Platform::waitOnEvent( NATIVE_EVENT& event, unsigned long timeout )
{
	if( !event.initialised )
		return WR_ABANDONED;

	bool eventInitiallySignaled = false;
	timeval time;
	timespec timeoutSpec;
	int nativeWaitResult = -1;
	WaitResult result = WR_FAILED;

	// Lock the event object so that changes can be made to it while we modify the listener list
	::pthread_mutex_lock( &event.mutex );

	// Initialise an EventListener instance for this thread
	EventListener listener;
	::pthread_mutex_init( &listener.mutex, NULL );
	::pthread_cond_init( &listener.condition, NULL );

	// Add this thread to the listener list, and do an initial check to see if it has been signaled
	event.listenerList.push_back( &listener );
	if( event.state )
	{
		eventInitiallySignaled = true;
	}

	// Event has not been signaled yet, so wait for it
	if( !eventInitiallySignaled )
	{
		::pthread_mutex_lock( &listener.mutex );
		if( timeout != NATIVE_INFINITE_WAIT )
		{
			::gettimeofday( &time, NULL );
			timeoutSpec.tv_nsec = time.tv_usec * 1000 + ((long)timeout) * 1000000L;
			timeoutSpec.tv_sec = time.tv_sec;
			// If the wait is over 1 second, then increase the second count
			if( timeoutSpec.tv_nsec >= 1000000000L )
			{
				timeoutSpec.tv_sec += timeoutSpec.tv_nsec / 1000000000L;
				timeoutSpec.tv_nsec %= 1000000000L;
			}
		}

		// Unlock the event mutex and wait for the signal
		::pthread_mutex_unlock( &event.mutex );
		if( timeout == NATIVE_INFINITE_WAIT )
		{
			nativeWaitResult = ::pthread_cond_wait( &listener.condition, &listener.mutex );
		}
		else
		{
			nativeWaitResult = ::pthread_cond_timedwait( &listener.condition,
														 &listener.mutex,
														 &timeoutSpec );
		}

		// Translate the native wait result
		if( nativeWaitResult == ETIMEDOUT )
			result = WR_TIMEOUT;

		// Unlock listener
		::pthread_mutex_unlock( &listener.mutex );

		// Relock event
		::pthread_mutex_lock( &event.mutex );
	}

	event.listenerList.remove( &listener );
	if( event.listenerList.size() == 0 )
		::pthread_cond_broadcast( &event.noMoreListenersEvent );

	if( eventInitiallySignaled )
	{
		result = WR_SUCCEEDED;
	}
	else
	{
		if( result != WR_TIMEOUT )
		{
			if( event.state )
				result = WR_SUCCEEDED;
			else
				result = WR_ABANDONED;
		}
	}

	// Unlock event
	::pthread_mutex_unlock( &event.mutex );

	// Clean up the listener
	::pthread_mutex_destroy( &listener.mutex );
	::pthread_cond_destroy( &listener.condition );

	return result;
}

bool Platform::destroyEvent( NATIVE_EVENT& event )
{
	bool destroyed = false;
	if( event.initialised )
	{
		event.initialised = false;

		::pthread_mutex_lock( &event.mutex );
		if( event.listenerList.size() > 0 )
		{
			std::list<EventListener*>::iterator listenerIterator = event.listenerList.begin();
			while( listenerIterator != event.listenerList.end() )
			{
				EventListener* listener = *listenerIterator;
				::pthread_mutex_lock( &listener->mutex );
				::pthread_cond_broadcast( &listener->condition );
				::pthread_mutex_unlock( &listener->mutex );

				++listenerIterator;
			}

			::pthread_cond_wait( &event.noMoreListenersEvent, &event.mutex );
		}
		::pthread_mutex_unlock( &event.mutex );
		::pthread_mutex_destroy( &event.mutex );
	}

	return destroyed;
}

bool Platform::initialiseSocketFramework()
{
	// Nothing to do on Posix
	return true;
}

void Platform::cleanupSocketFramework()
{
	// Nothing to do on Posix
}

void Platform::setMulticastSocketOptions( NATIVE_SOCKET& socket )
{
	// Set SO_REUSEADDR so that other processes can bind to it
	int intTrue = 1;
	::setsockopt( socket,
				  SOL_SOCKET, 
				  SO_REUSEADDR, 
				  (char*)&intTrue,
				  sizeof(intTrue) );

	// MacOSX also requires us to set SO_REUSEPORT
	::setsockopt( socket,
				  SOL_SOCKET,
				  SO_REUSEPORT,
				  (char*)&intTrue,
				  sizeof(intTrue) );
}

NATIVE_IP_ADDRESS Platform::lookupHost( const tchar* hostName )
{
	NATIVE_IP_ADDRESS result = INADDR_NONE;

	std::string narrowHostName = Platform::toAnsiString( hostName );
	if( !narrowHostName.empty() )
	{
		const char* narrowHostNameCStr = narrowHostName.c_str();

		hostent* hostEntry = ::gethostbyname( narrowHostNameCStr );
		if ( hostEntry != NULL )
			result = ntohl( *((NATIVE_IP_ADDRESS*)hostEntry->h_addr) );
	}

	return result;

}

#include <arpa/inet.h>

int Platform::lookupHostName( NATIVE_IP_ADDRESS address,
							  int addressType,
							  tchar* outBuffer,
							  int outBufferSize )
{
	int returnSize = 0;

	in_addr addressRequest;
	addressRequest.s_addr = htonl( address );
	hostent* host = ::gethostbyaddr( (const char*)&addressRequest,
									 sizeof(in_addr),
									 addressType );
	if( host )
	{
		const char* name = host->h_name;
		returnSize = Platform::toPlatformChars( name, strlen(name), outBuffer, outBufferSize );
	}

	return returnSize;
}


#include <arpa/inet.h>

int Platform::getHostAddress( NATIVE_IP_ADDRESS address,
							  int addressType,
							  tchar* outBuffer,
							  int outBufferSize )
{
	int returnSize = 0;

	in_addr addressRequest;
	addressRequest.s_addr = htonl( address );
	char* ansiAddress = ::inet_ntoa( addressRequest );

	if( ansiAddress )
	{
		returnSize = Platform::toPlatformChars( ansiAddress,
												strlen(ansiAddress),
												outBuffer,
												outBufferSize );
	}

	return returnSize;
}

const int Platform::closeSocket( NATIVE_SOCKET socket )
{
	return ::close( socket );
}

const tchar* Platform::describeLastSocketError()
{
	const tchar* error = TEXT("Unknown Error");

	switch( errno )
	{
		case EADDRINUSE:
			error = TEXT("Address already in use");
			break;
		case EADDRNOTAVAIL:
			error = TEXT("Address not available");
			break;
		case EAFNOSUPPORT:
			error = TEXT("Address family not supported");
			break;
		case EAGAIN:
			error = TEXT("Resource temporarily unavailable");
			break;
		case EALREADY:
			error = TEXT("Connection already in progress");
			break;
		case EBADF:
			error = TEXT("Bad file descriptor");
			break;
		case EBUSY:
			error = TEXT("Device or resource busy");
			break;
		case ECONNABORTED:
			error = TEXT("Connection aborted");
			break;
		case ECONNREFUSED:
			error = TEXT("Connection refused");
			break;
		case ECONNRESET:
			error = TEXT("Connection reset");
			break;
		case EFAULT:
			error = TEXT("Bad address");
			break;
		case EHOSTDOWN:
			error = TEXT("Host is down");
			break;
		case EHOSTUNREACH:
			error = TEXT("Host is unreachable");
			break;
		case EINVAL:
			error = TEXT("Invalid argument");
			break;
		case EISCONN:
			error = TEXT("Socket is connected");
			break;
		case ENETDOWN:
			error = TEXT("Network is down");
			break;
		case ENETRESET:
			error = TEXT("Connection aborted by network");
			break;
		case ENETUNREACH:
			error = TEXT("Network unreachable");
			break;
		case ENOPROTOOPT:
			error = TEXT("Protocol is not available");
			break;
		case ENOTCONN:
			error = TEXT("The socket is not connected");
			break;
		case ENOTSOCK:
			error = TEXT("Not a socket");
			break;
		case EPFNOSUPPORT:
			error = TEXT("Protocol not supported");
			break;
		case EPROTO:
			error = TEXT("Protocol error");
			break;
		case EPROTONOSUPPORT:
			error = TEXT("Protocol not supported");
			break;
		case EPROTOTYPE:
			error = TEXT("Protocol wrong type for socket");
			break;
		case ESHUTDOWN:
			error = TEXT("Cannot send after transport endpoint shutdown");
			break;
		case ESOCKTNOSUPPORT:
			error = TEXT("Socket type not supported");
			break;
	}

	return error;
}

int Platform::toAnsiChars( const wchar_t* string,
						   int stringLength,
						   char* outBuffer,
						   int outBufferSize )
{
	return 0;
}

std::string Platform::toAnsiString( const char* string )
{
	return std::string( string );
}

std::string Platform::toAnsiString( const char* string, int length )
{
	return std::string( string, length );
}

std::string Platform::toAnsiString( const wchar_t* string )
{
	return toAnsiString( string, ::wcslen(string) );
}

std::string Platform::toAnsiString( const wchar_t* string, int length )
{
	std::string result;
	return result;
}

int Platform::toUnicodeChars( const char* string,
							  int stringLength,
							  wchar_t* outBuffer,
							  int outBufferSize )
{
	return 0;
}

std::wstring Platform::toUnicodeString( const char* string )
{
	return Platform::toUnicodeString( string, (int)strlen(string) );
}

std::wstring Platform::toUnicodeString( const char* string, int length )
{
	std::wstring result;
	return result;
}

std::wstring Platform::toUnicodeString( const wchar_t* string )
{
	return std::wstring( string );
}

std::wstring Platform::toUnicodeString( const wchar_t* string, int length )
{
	return std::wstring( string, length );
}

int Platform::toPlatformChars( const char* string,
							   int stringLength,
							   tchar* outBuffer,
							   int outBufferSize )
{
	int size = 0;
#ifndef UNICODE
	size = stringLength;
	if( outBuffer != NULL )
	{
		::memcpy( outBuffer, string, stringLength );
	}
#else
	size = Platform::toUnicodeChars( string, stringLength, outBuffer, outBufferSize );
#endif
	return size;
}

String Platform::toPlatformString( const char* string )
{
#ifndef UNICODE
	return std::string( string );
#else
	return Platform::toUnicodeString( string );
#endif
}

String Platform::toPlatformString( const char* string, int length )
{
#ifndef UNICODE
	return Platform::toAnsiString( string, length );
#else
	return Platform::toUnicodeString( string, length );
#endif
}

String Platform::toPlatformString( const wchar_t* string )
{
#ifndef UNICODE
	return Platform::toAnsiString( string );
#else
	return std::wstring( string );
#endif
}

void Platform::longToChars( const long value, tchar* buffer, size_t length )
{
#ifndef UNICODE
	::snprintf( buffer, length, "%ld", value );
#else
	::swnprintf( buffer, length "%ld", value );
#endif
}

bool Platform::fileExists( const tchar* fileName )
{
	return false;
}

unsigned long Platform::getCurrentTimeMilliseconds()
{
	timeval time;
	::gettimeofday( &time, NULL );

	return (time.tv_sec * 1000) + (time.tv_usec / 1000L);
}

void* Platform::threadEntry( void* argument )
{
	ThreadEntryData* data = (ThreadEntryData*)argument;
	WrappedThread* nativeThread = data->nativeThread;
	void* threadArgument = data->argument;
	::free( data );

	// Wait for the resume event to be signaled, and then start the execution
	Platform::waitOnEvent( nativeThread->resumeEvent, NATIVE_INFINITE_WAIT );
	Platform::clearEvent( nativeThread->resumeEvent );
	Thread::threadEntry( threadArgument );

	return NULL;
}

#endif
