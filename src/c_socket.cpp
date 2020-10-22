#include <errno.h>
#include <glog/logging.h>
#include <iostream>
#include <unordered_map>
#include <string.h>
#include <string>

#include "c_socket.hpp"

int c_socket_server::connect(int port) {
  int rc, yes = 1, sock_fd;

  if (descriptor > -1) {
    DLOG(WARNING) << "Socket already initialized and connected";
    return 0;
  }

  if (port <= 0) {
    DLOG(ERROR) << "Invalid input port: " << port << "\n";
    return -1;
  }

  struct addrinfo hints, *add_info;
  memset(&hints, 0, sizeof hints); /*clear sockaddr structure*/

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rc = getaddrinfo(nullptr, std::to_string(port).c_str(), &hints,
                        &add_info)) != 0) {
    DLOG(ERROR) << "Failed to get address information. " << gai_strerror(rc);
    return -2;
  }

  for (struct addrinfo *ptr = add_info; (ptr != nullptr); ptr = ptr->ai_next) {
    if ((sock_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) <
        0) {
      PLOG(WARNING) << "Failed to created socket.";
      continue;
    }

    // Set socket option SO_REUSEADDR to 1
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      PLOG(ERROR) << "Failed to set socket option.";
      freeaddrinfo(add_info);
      return -3;
    }

    if ((bind(sock_fd, ptr->ai_addr, ptr->ai_addrlen) < 0) || (ptr == NULL) ||
        (ptr == nullptr)) {
      PLOG(ERROR) << "Failed to bind address.";
      continue;
    }

    descriptor = sock_fd;
    freeaddrinfo(add_info);
    break;
  }

  if (listen(descriptor, SOMAXCONN) < 0) {
    PLOG(ERROR) << "Failed to listen to socket.";
    return -5;
  }

  return 1;
}

int c_socket_server::wait_for_incoming_connection(void) {
  std::string buff;
  int inc_descriptor = -1, visits = 0;
  struct sockaddr_in client_addr;
  memset((char *)&client_addr, 0,
         sizeof(client_addr)); /*clear sockaddr structure*/
  socklen_t c_add_len = 0;
  c_add_len = sizeof(client_addr);

  if (descriptor < 0) {
    DLOG(ERROR) << "Socket hasn't being connected\n";
    return 0;
  }

  if (c_add_len < 1) {
    DLOG(ERROR) << "Failed to get size of sockaddr_in c_add\n";
    return -1;
  }

  DLOG(INFO) << "Server: waiting for connections on descriptor = "
             << descriptor;

  while (1) {
    inc_descriptor = -1;

    if ((inc_descriptor = accept(descriptor, (struct sockaddr *)&client_addr,
                                 &c_add_len)) < 0) {
      PLOG(ERROR) << "Failed to accept incoming socket";
      return -2;
    }

    buff =
        "This server has been contacted " + std::to_string(++visits) + " time";
    buff.append(visits == 1 ? "." : "s.");
    DLOG(INFO) << buff << "\n";

    if (send(inc_descriptor, buff.c_str(), buff.length(), 0) < 0) {
      PLOG(ERROR) << "Failed to send message.";
      return -3;
    }

    conn_fd = inc_descriptor;
    break;
  }

  if (conn_fd < 1) {
    DLOG(ERROR) << "Failed to obtain connection file descriptor";
    return -4;
  }

  return 1;
}

int c_socket_server::recv_info() {
  int rc, num_bytes;
  char buff[MAXDATASIZE];

  while (1) {
    buff[0] = '\0';
    if ((num_bytes = recv(conn_fd, buff, MAXDATASIZE - 1, 0)) < 0) {
      PLOG(ERROR) << "Failed to connect to socket address.";
      return -6;
    }

    buff[num_bytes] = '\0';
    DLOG_IF(INFO, num_bytes > 0) << "Server: Recv'd = " << buff;
  }
  return 1;
}

int c_socket_client::connect(const std::string &host, int port) {
  int rc, num_bytes;
  char buff[MAXDATASIZE];

  if (host.empty()) {
    DLOG(ERROR) << "Invalid host name\n";
    return -1;
  }

  if (port <= 0) {
    DLOG(ERROR) << "Invalid input port: " << port << "\n";
    return -2;
  }

  struct addrinfo hints, *add_info = nullptr;
  memset((char *)&add_info, 0, sizeof(add_info)); /*clear sockaddr structure*/
  memset((char *)&hints, 0, sizeof(hints));       /*clear sockaddr structure*/

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rc = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints,
                        &add_info)) != 0) {
    DLOG(ERROR) << "Failed to get address information. " << gai_strerror(rc);
    return -3;
  }

  if (add_info == nullptr) {
    PLOG(ERROR) << "Invalid address information returned.";
    return -4;
  }

  int s_desc = -1;
  for (struct addrinfo *ptr = add_info; ((ptr != nullptr) && (ptr != 0));
       ptr = ptr->ai_next, s_desc = -1) {
    if ((s_desc = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) <
        0) {
      PLOG(ERROR) << "Failed to created socket.";
      continue;
    }

    if (::connect(s_desc, ptr->ai_addr, ptr->ai_addrlen) < 0) {
      PLOG(ERROR) << "Failed to connect to socket address.";
      closesocket(s_desc);
      continue;
    }

    DLOG(INFO) << "Connected to socket.";
    descriptor = s_desc;
    break;
  }

  if (descriptor <= 0) {
    DLOG(ERROR) << "Failed to created socket\n";
    freeaddrinfo(add_info);
    return -5;
  }

  if ((num_bytes = recv(descriptor, buff, MAXDATASIZE - 1, 0)) < 0) {
    PLOG(ERROR) << "Failed to connect to socket address.";
    return -6;
  }

  buff[num_bytes] = '\0';
  DLOG(INFO) << "Client: Recv'd = " << buff;

  freeaddrinfo(add_info);
  return 1;
}

int c_socket_client::send_info() {
  const std::unordered_map<std::string, std::string> data = {
      {"StudID: ", "1111"},
      {"Name: ", "Bob Smith"},
      {"SSN: ", "222-333-1111"},
      {"EmailAddress: ", "bsmith@yahoo.com"},
      {"HomePhone: ", "215-777-8888"},
      {"HomeAddr: ", "123 Tulip Road, Ambler, PA 19002"},
      {"LocalAddr: ", "321 Maple Avenue, Lion Town, PA 16800"},
      {"EmergencyContact: ", "John Smith (215-222-6666)"},
      {"ProgramID: ", "206"},
      {"PaymentID: ", "1111-206"},
      {"AcademicStatus: ", "1"}};

  for (auto &info : data) {
    if (send(descriptor, info.first.c_str(), info.first.length(), 0) < 0) {
      PLOG(ERROR) << "Failed to send message.";
      return -1;
    }

    if (send(descriptor, info.second.c_str(), info.second.length(), 0) < 0) {
      PLOG(ERROR) << "Failed to send message.";
      return -2;
    }

    sleep(1);
  }
  return 1;
}
