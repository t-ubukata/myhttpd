#include "server.h"

namespace myhttpd {

int Init(uint16_t port) {
  auto fd = socket(AF_INET, SOCK_STREAM, 0);
  CHECK_ERRNO(fd != -1, "socket() failed.");

  int optval = 1;
  auto setsockopt_ret =
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  CHECK_ERRNO(setsockopt_ret == 0, "setsockopt() failed.");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  auto bind_ret =
      bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  CHECK_ERRNO(bind_ret == 0, "bind() failed.");

  auto listen_ret = listen(fd, SOMAXCONN);
  CHECK_ERRNO(listen_ret == 0, "listen() failed.");

  return fd;
}

std::string Response(const std::string& request_header,
                     const std::string& root_path) {
  char method[16];
  char uri[256];
  char ver[64];
  auto ret = sscanf(request_header.c_str(), "%s %s %s", method, uri, ver);
  if (ret == EOF) {
    std::cerr << "sscanf() failed: "
              << "\n";
    // TODO: Return 500.
  }
  if (std::string(method) != "GET") {
    // TODO: Return 501.
  }
  if (std::string(ver) != "HTTP/1.1") {
    // TODO: Return 505.
  }
  // TODO: Handle absolute URI, relative URI, absolute path, relative path.
  if (std::string(uri) == "/") {
    // TODO: Read index.html.
  } else {
    // TODO: Read requested file.
  }
  return "Hello, World.";
}

void Serve(uint16_t port, const std::string& root_path) {
  auto fd = Init(port);
  while (true) {
    auto accepted_fd =
        accept(fd, static_cast<struct sockaddr*>(nullptr), nullptr);
    // Continues when accept() fails.
    if (accepted_fd == -1) {
      std::cerr << "accept() failed: " << strerror(errno) << "\n";
      continue;
    }
    char buf[1024] = {};
    std::string req_h;
    // TODO: Fork.
    // Receives message until meeting CRLFCRLF.
    while (true) {
      // Gives sizeof(buf) -1 because of NULL termination.
      auto sz = recv(accepted_fd, buf, sizeof(buf) - 1, 0);
      if (sz == -1) {
        std::cerr << "recv() failed: " << strerror(errno) << "\n";
        close(accepted_fd);
        accepted_fd = -1;
        // TODO: Return 500.
        break;
      }
      req_h.append(buf);
      // Checks end of header.
      constexpr uint8_t CR = 0x0d;
      constexpr uint8_t LF = 0x0a;
      auto l = req_h.length();
      if ((req_h[l - 4] == CR) && (req_h[l - 3] == LF) &&
          (req_h[l - 2] == CR) && (req_h[l - 1] == LF)) {
        break;
      }
    }
    auto res = Response(req_h, root_path);
    if (send(accepted_fd, res.c_str(), res.length(), 0) == -1) {
      std::cerr << "send() failed: " << strerror(errno) << "\n";
    }
    close(accepted_fd);
    accepted_fd = -1;
  }
}

}  // namespace myhttpd
