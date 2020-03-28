#include "server.hpp"

namespace nvhttpd {

Server::Server() noexcept {
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ < 0) {
    std::cerr << "socket() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Server::Serve(const int port, const std::string& domain,
                   const std::string& root_path) noexcept {
  SetOption();
  Bind(port);
  Listen();

  std::cout << "Serving..." << std::endl;

  // Main loop.
  while (true) {
    auto accepted_socket_fd {accept(socket_fd(), (struct sockaddr*)NULL, NULL)};
    // Continues when accept() fails.
    if (accepted_socket_fd == -1) {
        continue;
    }

    char buf[1024] {0};
    std::string request_header;

    // Receives message until meeting CRLFCRLF.
    while (true) {
      // Gives sizeof(buf) -1 because of NULL termination.
      auto read_size {recv(accepted_socket_fd, buf, sizeof(buf) - 1, 0)};
      if (read_size == -1) {
        std::cerr << "read() failed." << std::endl;
        std::cerr << "ERROR: " << strerror(errno) << std::endl;
        close(accepted_socket_fd);
        socket_fd_ = -1;
        break;
      }

      if (read_size > 0) {
        request_header.append(buf);
      }

      // Checks end of header.
      constexpr unsigned char kCR {0x0d};
      constexpr unsigned char kLF {0x0a};
      if ((request_header[request_header.length() - 4] == kCR) &&
          (request_header[request_header.length() - 3] == kLF) &&
          (request_header[request_header.length() - 2] == kCR) &&
          (request_header[request_header.length() - 1] == kLF)) {
        break;
      }

      if (read_size <= 0) {
        // TODO: Return Bad Request.
        break;
      }
    }

    // Get response.
    auto response = Response(request_header);

    // Writes response.
    if (send(accepted_socket_fd, response.c_str(),
        response.length(), 0) == -1) {
      std::cerr << "write() failed." << std::endl;
      std::cerr << "ERROR: " << strerror(errno) << std::endl;
    }

    // Closes accepted socket.
    close(accepted_socket_fd);
    accepted_socket_fd = -1;
  }
}

int Server::socket_fd() const noexcept {
  return socket_fd_;
}

void Server::SetOption() noexcept {
  // Sets level SOL_SOCKET to operate option in socket layer (required)
  // SO_REUSEADDR: you can reuse address as long as not listened
  // argument is an integer boolean flag.
  // optval 1 means true.
  // For more info, see man page of socket(2)
  int optval {1};
  if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &optval,
      sizeof(optval))) {
    std::cerr << "setsockopt() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    // Close requires unistd.h.
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

void Server::Bind(const int port) noexcept {
  // Initializes sock_addr_in structure type variable
  // zero padding to initialize
  // reference operator is prior to address operator.
  struct sockaddr_in socket_address_in {0, AF_INET, htonl(INADDR_ANY),
      {htons(port)}, {0}};

  // memset(socket_address_in, 0, sizeof(this->serv_addr));

  // AF_INET means IPv4.
  // socket_address_in.sin_family = AF_INET;
  // htonl stands for host to net long
  // INADDR_ANY means any IP address
  // socket_address_in.sin_addr.s_addr = htonl(INADDR_ANY);
  // htons stands for host to net short
  // socket_address_in.sin_port = htons(port);

  // struct sockaddr: The only purpose of this structure is
  //   to cast the structure pointer
  //   passed in addr in order to avoid compiler warnings.
  if (bind(socket_fd_, (struct sockaddr*)&socket_address_in,
      sizeof(socket_address_in)) == -2) {
    std::cerr << "bind() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

void Server::Listen() noexcept {
  if (listen(socket_fd_, SOMAXCONN) == -1) {
    std::cerr << "listen() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

namespace {

std::string Response(const std::string& http_request) {
  // TODO: Implement

  // Get target path

  // Get file content

  return "Hello, World.";
}

}

}  // namespace nvhttpd
