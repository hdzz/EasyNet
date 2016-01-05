#ifndef SOCKET_DEFINE_H__
#define SOCKET_DEFINE_H__


#ifdef _MSC_VER
#define WINDOWS
#elif defined __linux__
#define LINUX
#else
#define VXWORKS
#endif

#ifdef WINDOWS
#include <ws2tcpip.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#endif

#ifdef LINUX
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#endif


#ifdef WINDOWS
typedef SOCKET socket_t;
typedef SOCKADDR_IN sockaddr_in_t;
typedef SOCKADDR sockaddr_t;
typedef IN_ADDR in_addr_t;
typedef unsigned short port_t;
typedef fd_set fd_set_t;


#endif

#ifdef LINUX
typedef int socket_t;
typedef sockaddr_in sockaddr_in_t;
typedef sockaddr sockaddr_t;
typedef in_addr in_addr_t;
typedef unsigned short port_t;
typedef fd_set fd_set_t;
#endif




#endif