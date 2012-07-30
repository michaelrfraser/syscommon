#Welcome to the SysCommon Project!#

SysCommon is an extremely lightweight set of utility classes that encapsulate
commonly used systems programming tools such as Sockets, Threads, Locks etc.
in an easy to use API.

The interfaces presented are extremely similar to Java's, making SysCommon an
ideal step into the Systems Programming world from the Java language. If you
haven't come from a Java background, well trust me its a lot easier :)

SysCommon currently provides:
- Lock class
- Semaphore class with interrupt-able acquire()
- Thread class with interrupt-able join and sleep()
- Properties class with load from file support
- Logger class with log4j like levels
- Multicast Socket (UDP) class with wrapped DatagramPacket for input/output
- Socket (TCP) class
- InetSocketAddress class for easy host/address lookup
- Hot swappable Unicode support based on #define UNICODE
- A few string functions to augment std:string (startsWith, endsWith, toUpper, 
  trim)

NOTE: Currently only the Windows implementation is complete. I will be actively
working on Linux/Mac implementations shortly.

SysCommon is provided under the Common Development and Distribution License (CDDL).