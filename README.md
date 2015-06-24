#Welcome to the syscommon Project!#

## About syscommon ##
Syscommon is an extremely lightweight set of utility classes that encapsulate
commonly used systems programming tools such as Sockets, Threads, Locks etc.
in an easy to use API.

The interfaces presented are extremely similar to Java's, making SysCommon an
ideal step into the Systems Programming world from the Java language. If you
haven't come from a Java background, well trust me its a lot easier :)

Syscommon currently provides:
- Lock class
- Semaphore class with interrupt-able acquire()
- Thread class with interrupt-able join and sleep()
- Properties class with load from file support
- Logger class with log4j like levels
- Multicast Socket (UDP) class with wrapped DatagramPacket for input/output
- Socket (TCP) and ServerSocket classes
- InetSocketAddress class for easy host/address lookup
- Hot swappable Unicode support based on #define UNICODE
- A few string functions to augment std:string (startsWith, endsWith, toUpper, 
  trim)

Syscommon is cross-platform and compiles under Windows, OSX and Linux.

## Licensing ##
Syscommon is provided under the Common Development and Distribution License (CDDL).

## Building ##
Syscommon uses a simple build system of batch files/shell scripts. The repository
contains the relevant scripts to build Syscommon as a static library, which is the
recommended way to deplay syscommon.

### Windows ###
Syscommon can be built either via the commandline, or through Visual Studio.

To compile on the command line, navigate to the `codebase\` directory and issue either
of the following commands:

```
scripts\windows\vc8.bat    # compiles for VC8
scripts\windows\vc10.bat   # compiles for VC10
```

The scripts will build 32-bit and 64-bit static libraries for both ANSI and Unicode 
character sets, placing them in the `codebase\dist` directory.

Visual Studio solution files are located at `scripts\windows\vc8\vc8.sln` and 
`scripts\windows\vc10\vc10.sln`.

### Linux ###
SysCommon can be built via the commandline using the provided build shell script.

```
scripts/linux32/static.sh   # compiles 32-bit versions of the library
scripts/linux64/static.sh   # compiles 64-bit versions of the library
```

The scripts will build 32-bit and 64-bit static libraries for both ANSI and Unicode 
character sets, placing them in the `codebase\dist` directory.

### Mac OSX ###
SysCommon can be built via the commandline using the provided build shell script.

```
scripts/macosx/static.sh    # compiles 64-bit versions of the library
```

The scripts will build 64-bit static libraries for both ANSI and Unicode character 
sets, placing them in the `codebase\dist` directory.

## Version History ##
### Version 0.3.1, 2015-03-17 ###
**New and Noteworthy**
- Added `formatTime()` method to `StringUtils` class
- Added `stringSplit()` method to `StringUtils` class

**Bug Fixes and Improvements**
- Improvements to the Logger class
  - The log level marker is now right-aligned
  - All log messages are now timestamped

### Version 0.3, 2014-10-21 ###
**New and Noteworthy**
- Simplified build system
  - Ant has been replaced with batch/shell scripts
  - UNICODE build is now compiled for all platforms
- Visual Studio solution files are now shipped for VC8 and VC10

**Bug Fixes**
- Fixed race condition in the `Thread` class that caused `Thread::currentThread()` to 
  occasionally return `NULL` for a valid thread.
- Fixed unit tests for the previous namespace/structure changes.
