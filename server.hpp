#ifndef SERVER_HPP_
#define SERVER_HPP_

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "http_request.hpp"
#include "http_response.hpp"

namespace nvhttpd {

class Server {
 public:
  // Opens socket and set socket file descriptor.
  Server() noexcept;
  // Serves.
  void Serve(const int port) noexcept;
  int socket_fd() const noexcept;

 private:
  // Sets socket option.
  void SetOption() noexcept;
  // Binds socket file descriptor.
  void Bind(const int port) noexcept;
  // Listens on Socket file descriptor.
  void Listen() noexcept;

  // Socket file descriptor.
  int socket_fd_;
};

}  // namespace nvhttpd

#endif  // SERVER_HPP_
