#include <iostream>
#include <sys/types.h>
#include <string.h>
#include <climits>
#define MAXLINE 1024
#if defined (_WIN32)  || defined(_WIN32_)
	#include<ws2tcpip.h>
	#include<winsock2.h>
	#define ATTR_PACKED
	#define  SASTART() \
		WSADATA wsaData; \
		WORD wVers; \
		wVers = MAKEWORD(1, 1); \
		if(0 != WSAStartup(wVers, &wsaData)) \
		{ \
		std::cout<<"Fail of calling WSAStartup!"<<std::endl; \
		return 1; \
		}
	#define SACLEAN()\
		WSACleanup();
#else
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#define ATTR_PACKED __attribute__((packed))
	#define  SASTART()
	#define SACLEAN()
#endif
// 8 bits integer types
#if UCHAR_MAX == 0xFF
	typedef signed   char Int8;
	typedef unsigned char UInt8;
#else
	#error No 8 bits integer type for this platform
#endif

// 16 bits integer types
#if USHRT_MAX == 0xFFFF
	typedef signed   short Int16;
	typedef unsigned short UInt16;
#elif UINT_MAX == 0xFFFF
	typedef signed   int Int16;
	typedef unsigned int UInt16;
#elif ULONG_MAX == 0xFFFF
	typedef signed   long Int16;
	typedef unsigned long UInt16;
#else
	#error No 16 bits integer type for this platform
#endif

// 32 bits integer types
#if USHRT_MAX == 0xFFFFFFFF
	typedef signed   short Int32;
	typedef unsigned short UInt32;
	#elif UINT_MAX == 0xFFFFFFFF
	typedef signed   int Int32;
	typedef unsigned int UInt32;
#elif ULONG_MAX == 0xFFFFFFFF
	typedef signed   long Int32;
	typedef unsigned long UInt32;
#else
	#error No 32 bits integer type for this platform
#endif

// 64 bits integer types
#if USHRT_MAX == 0xFFFFFFFFFFFFFFFFull
	typedef signed   short Int64;
	typedef unsigned short UInt64;
#elif UINT_MAX == 0xFFFFFFFFFFFFFFFFull
	typedef signed   int Int64;
	typedef unsigned int UInt64;
#elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFull
	typedef signed   long Int64;
	typedef unsigned long UInt64;
#elif ULLONG_MAX == 0xFFFFFFFFFFFFFFFFull
	typedef signed   long long Int64;
	typedef unsigned long long UInt64;
#else
	#error No 64 bits integer type for this platform
#endif
