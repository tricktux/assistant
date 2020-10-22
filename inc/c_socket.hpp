#ifndef C_SOCKET_HPP
#define C_SOCKET_HPP

#ifndef unix
#define WIN32
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <string>

class c_socket_server {
  // Server's file descriptor
  int descriptor;
  // Accepted connection file descriptor
  int conn_fd;
  const int MAXDATASIZE = 100;

public:
  c_socket_server() : descriptor(-1), conn_fd(-1) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  }

  ~c_socket_server() {
    closesocket(descriptor);
#ifdef WIN32
    WSACleanup();
#endif
  }

  int connect(int port);
  int wait_for_incoming_connection(void);
  int recv_info(void);
};

class c_socket_client {
  int descriptor;
  const int MAXDATASIZE = 100;

public:
  c_socket_client() : descriptor(-1) {
#ifdef WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
  }

  ~c_socket_client() {
    if (descriptor >= 0) {
      closesocket(descriptor);
#ifdef WIN32
      WSACleanup();
#endif
    }
  }

  int connect(const std::string &host, int port);
  int send_info();
};

#endif
