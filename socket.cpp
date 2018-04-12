#include "socket.hpp"

namespace sshttpsv {

Socket::Socket() noexcept {
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_fd_ < 0) {
    std::cerr << "socket() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Socket::Wait(const int port) noexcept {
  SetOption();
  Bind(port);
  Listen();

  int accepted_socket_fd {0};
  constexpr int kCR {0x0d};
  constexpr int kLF {0x0a};
  constexpr int kBufSize {1024};

  // Main loop.
  while(true) {
    accepted_socket_fd = accept(socket_fd(), (struct sockaddr*)NULL, NULL);
    // Continues when accept() fails.
    if(accepted_socket_fd == -1) {
        continue;
    }

    char buf[kBufSize] {0};
    std::string request_header;
    ssize_t read_size {0};

    // Receives message until meeting CRLFCRLF.
    while(true) {
      // Gives sizeof(buf) -1 because of NULL termination.
      read_size = recv(accepted_socket_fd, buf, sizeof(buf) - 1, 0);
      if(read_size == -1) {
        std::cerr << "read() failed." << std::endl;
        std::cerr << "ERROR: " << strerror(errno) << std::endl;
        close(accepted_socket_fd);
        socket_fd_ = -1;
        break;
      }

      if(read_size > 0) {
        request_header.append(buf);
      }

      // Checks end of header.
      if((request_header[request_header.length() - 4] == kCR) &&
          (request_header[request_header.length() - 3] == kLF) &&
          (request_header[request_header.length() - 2] == kCR) &&
          (request_header[request_header.length() - 1] == kLF)) {
        break;
      }

      if(read_size <= 0) {
        // TODO: Return Bad Request.
        break;
      }
    }

    // TODO: Parse request and get response.
    Http_request http_request {request_header};

    Http_response http_response;
    std::string http_response_string = http_response.get_response(http_request);

    // Writes response.
    if(send(accepted_socket_fd, http_response_string.c_str(),
        http_response_string.length(), 0) == -1) {
      std::cerr << "write() failed." << std::endl;
      std::cerr << "ERROR: " << strerror(errno) << std::endl;
    }

    // Closes accepted socket.
    close(accepted_socket_fd);
    accepted_socket_fd = -1;
  }
}

int Socket::socket_fd() const noexcept {
  return socket_fd_;
}

void Socket::SetOption() noexcept {
  // Sets level SOL_SOCKET to operate option in socket layer (required)
  // SO_REUSEADDR: you can reuse address as long as not listened
  // argument is an integer boolean flag.
  // optval 1 means true.
  // For more info, see man page of socket(2)
  int optval {1};
  if(setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &optval,
      sizeof(optval))) {
    std::cerr << "setsockopt() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    // Close requires unistd.h.
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

void Socket::Bind(const int port) noexcept {
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

void Socket::Listen() noexcept {
  if(listen(socket_fd_, SOMAXCONN) == -1) {
    std::cerr << "listen() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

} // namespace sshttpsv
