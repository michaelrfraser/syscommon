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

using namespace SysCommon;

#ifdef _WIN32

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
bool Platform::initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
								    unsigned int maximumCount, 
									const String& name )
{
	assert( nativeSemaphore == NATIVE_SEMAPHORE_UNINIT );

	bool result = false;
	if ( nativeSemaphore == NATIVE_SEMAPHORE_UNINIT )
	{
		nativeSemaphore = ::CreateSemaphore( NULL, 
											 maximumCount, 
											 maximumCount, 
											 name.c_str() );
		result = (nativeSemaphore != NATIVE_SEMAPHORE_UNINIT);
	}

	return result;
}

bool Platform::destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT );

	bool result = false;
	if ( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT )
	{
		result = ::CloseHandle( nativeSemaphore ) != FALSE;

		if ( result )
			nativeSemaphore = NATIVE_SEMAPHORE_UNINIT;
	}
	
	return result;
}

WaitResult Platform::waitOnSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
									  NATIVE_INTERRUPT& threadInterrupt, 
									  DWORD timeout )
{
	assert( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT );
	assert( threadInterrupt != NATIVE_INTERRUPT_UNINIT );

	WaitResult result = WR_FAILED;
	if ( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT && threadInterrupt != NATIVE_INTERRUPT_UNINIT )
		result = Platform::waitOnInterruptableHandle( threadInterrupt, nativeSemaphore, timeout );

	return result;
}

bool Platform::releaseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	assert( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT );

	bool result = false;
	if ( nativeSemaphore != NATIVE_SEMAPHORE_UNINIT )
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

bool Platform::initialiseThread( NATIVE_THREAD& nativeThread, 
								 NATIVE_THREAD_ID& nativeThreadID, 
								 NATIVE_THREAD_PROC startRoutine, 
								 void* thisPointer )
{
	assert( nativeThread == NATIVE_THREAD_UNINIT );
	assert( startRoutine );
	assert( thisPointer );

	bool result = false;
	if ( nativeThread == NATIVE_THREAD_UNINIT && startRoutine && thisPointer )
	{
		nativeThread = ::CreateThread( NULL, 
									   NULL, 
									   startRoutine, 
									   thisPointer, 
									   CREATE_SUSPENDED, 
									   &nativeThreadID );
		result = nativeThread != INVALID_HANDLE_VALUE;
	}

	return result;
}

bool Platform::destroyThread( NATIVE_THREAD& nativeThread )
{
	assert( nativeThread != NATIVE_THREAD_UNINIT );

	bool result = false;
	if ( nativeThread != NATIVE_THREAD_UNINIT )
	{
		result = ::CloseHandle( nativeThread ) != FALSE;

		if ( result )
			nativeThread = NATIVE_THREAD_UNINIT;
	}

	return result;
}

bool Platform::resumeThread( NATIVE_THREAD& nativeThread )
{
	assert( nativeThread != NATIVE_THREAD_UNINIT );

	bool result = false;
	if ( nativeThread != NATIVE_THREAD_UNINIT )
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
	assert( nativeThread != NATIVE_THREAD_UNINIT );
	assert( threadInterrupt != NATIVE_INTERRUPT_UNINIT );

	WaitResult result = WR_FAILED;
	if ( nativeThread != NATIVE_THREAD_UNINIT && threadInterrupt != NATIVE_INTERRUPT_UNINIT )
		result = Platform::waitOnInterruptableHandle( nativeThread, threadInterrupt, timeout );

	return result;
}

NATIVE_THREAD_ID Platform::getCurrentThreadID()
{
	return ::GetCurrentThreadId();
}

bool Platform::initialiseThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt, const String& name )
{
	assert( nativeInterrupt == NATIVE_INTERRUPT_UNINIT );

	bool result = false;
	if ( nativeInterrupt == NATIVE_INTERRUPT_UNINIT )
	{
		nativeInterrupt = ::CreateEvent( NULL, FALSE, FALSE, name.c_str() );
		result = (nativeInterrupt != NATIVE_INTERRUPT_UNINIT);
	}

	return result;
}

bool Platform::destroyThreadInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	assert( nativeInterrupt != NATIVE_INTERRUPT_UNINIT );

	bool result = false;
	if ( nativeInterrupt != NATIVE_INTERRUPT_UNINIT )
	{
		result = ::CloseHandle( nativeInterrupt ) != FALSE;

		if ( result )
			nativeInterrupt = NATIVE_INTERRUPT_UNINIT;
	}

	return result;
}

WaitResult Platform::performInterruptableSleep( NATIVE_INTERRUPT& threadInterrupt, DWORD timeout )
{
	assert( threadInterrupt != NATIVE_INTERRUPT_UNINIT );

	WaitResult result = WR_FAILED;
	if ( threadInterrupt != NATIVE_INTERRUPT_UNINIT )
	{
		DWORD nativeWaitResult = ::WaitForSingleObject( threadInterrupt, timeout );
		result = Platform::translateWaitResult( nativeWaitResult );
	}

	return result;
}

bool Platform::signalInterrupt( NATIVE_INTERRUPT& nativeInterrupt )
{
	assert( nativeInterrupt != NATIVE_INTERRUPT_UNINIT );

	bool result = false;
	if ( nativeInterrupt != NATIVE_INTERRUPT_UNINIT )
		result = ::SetEvent( nativeInterrupt ) != FALSE;

	return result;
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
			result = ::inet_addr( narrowHostNameCStr );
			if ( result == INADDR_NONE )
			{
				// Not in 4xOctet form, so look up the host name
				hostent* hostEntry = ::gethostbyname( narrowHostNameCStr );
				if ( hostEntry != NULL )
				{
					result = *((NATIVE_IP_ADDRESS*)hostEntry->h_addr_list[0]);
				}
			}
		}
		
		Platform::cleanupSocketFramework();
	}

	return result;
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
		addressRequest.S_un.S_addr = address;
		hostent* host = ::gethostbyaddr( (const char*)&addressRequest, 
										 sizeof(in_addr), 
										 addressType );
		if( host )
		{
			const char* name = host->h_name;
			returnSize = Platform::toPlatformChars( name, strlen(name), outBuffer, outBufferSize );
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
		addressRequest.S_un.S_addr = address;
		char* ansiAddress = ::inet_ntoa( addressRequest );
		if( ansiAddress )
		{
			returnSize = Platform::toPlatformChars( ansiAddress, 
													strlen(ansiAddress), 
													outBuffer, 
													outBufferSize );
		}

		Platform::cleanupSocketFramework();
	}
	return returnSize;
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

std::string Platform::toAnsiString( const char* string, size_t length )
{
	return std::string( string, length );
}

std::string Platform::toAnsiString( const wchar_t* string )
{
	return toAnsiString( string, ::wcslen(string) );
}

std::string Platform::toAnsiString( const wchar_t* string, size_t length )
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
	return Platform::toUnicodeString( string, strlen(string) );
}

std::wstring Platform::toUnicodeString( const char* string, size_t length )
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

std::wstring Platform::toUnicodeString( const wchar_t* string, size_t length )
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

String Platform::toPlatformString( const char* string, size_t length )
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

bool Platform::fileExists( const tchar* fileName )
{
	DWORD fileAttributes = ::GetFileAttributes( fileName );
	return fileAttributes != INVALID_FILE_ATTRIBUTES;
}

WaitResult Platform::translateWaitResult( DWORD nativeWaitResult )
{
	WaitResult result = WR_FAILED;

	switch ( nativeWaitResult )
	{
		case WAIT_OBJECT_0:
			result = WR_INTERRUPTED;
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
		result = translateWaitResult( nativeWaitResult );

		if( result == WR_INTERRUPTED )
			::ResetEvent( interruptHandle );
	}

	return result;
}

#else

//----------------------------------------------------------
//                     STATIC METHODS
//----------------------------------------------------------
bool Platform::initialiseSemaphore( NATIVE_SEMAPHORE& nativeSemaphore, 
								    unsigned int maximumCount, 
									const String &name )
{
	int result = sem_init( &nativeSemaphore, 0, maximumCount );
	return result != 0;
}

bool Platform::destroySemaphore( NATIVE_SEMAPHORE& nativeSemaphore )
{
	bool result = sem_destroy( &nativeSemaphore );
	return result != 0;
}



#endif