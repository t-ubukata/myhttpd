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
  constexpr size_t buf_sz = 1024;
  char method_c_str[buf_sz] = {};
  char target_c_str[buf_sz] = {};
  char version_c_str[buf_sz] = {};
  auto ret = sscanf(request_header.c_str(), "%s %s %s", method_c_str,
                    target_c_str, version_c_str);
  std::string method(method_c_str);
  std::string target(target_c_str);
  std::string version(version_c_str);

  if (ret == EOF) {
    std::cerr << "sscanf() failed\n";
    return "HTTP/1.0 500 Internal Server Error\r\n\r\n";
  }
  if (method != "GET") {
    return "HTTP/1.0 501 Not Implemented\r\n\r\n";
  }
  if (version != "HTTP/1.0") {
    return "HTTP/1.0 505 HTTP Version Not Supported \r\n\r\n";
  }

  // Removes trailing slash.
  std::string canonical_root_path = root_path;
  if (root_path.substr(root_path.size() - 1) == "/") {
    canonical_root_path = root_path.substr(0, root_path.size() - 1);
  }

  std::string canonical_target;
  if (target == "/") {
    canonical_target = canonical_root_path + "/index.html";
  } else {
    canonical_target = canonical_root_path + target;
  }

  std::ifstream ifs(target);
  if(ifs.fail()) {
    // TODO: Return 404.
  }
  std::string body((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

  // TODO: Define 200.

  return "Hello, World.";
}

void Serve(uint16_t port, const std::string& root_path) {
  auto fd = Init(port);
  constexpr size_t buf_sz = 1024;

  while (true) {
    auto accepted_fd =
        accept(fd, static_cast<struct sockaddr*>(nullptr), nullptr);
    // Continues when accept() fails.
    if (accepted_fd == -1) {
      std::cerr << "accept() failed: " << strerror(errno) << "\n";
      continue;
    }

    char buf[buf_sz] = {};
    std::string req_h;
    // TODO: Define rest header.
    std::string res = "HTTP/1.0 500 Internal Server Error\r\n\r\n";
    // TODO: Fork.
    // Receives message until meeting CRLFCRLF.
    while (true) {
      // Gives sizeof(buf) -1 because of NULL termination.
      auto sz = recv(accepted_fd, buf, sizeof(buf) - 1, 0);
      if (sz == -1) {
        std::cerr << "recv() failed: " << strerror(errno) << "\n";
        accepted_fd = -1;
        close(accepted_fd);
        // TODO: Check error.

        // Response is 500.
        break;
      }
      req_h.append(buf);
      if (req_h.size() >= 4 && req_h.substr(req_h.size() - 4) == "\r\n\r\n") {
        break;
      }
    }

    res = Response(req_h, root_path);

    if (send(accepted_fd, res.c_str(), res.length(), 0) == -1) {
      std::cerr << "send() failed: " << strerror(errno) << "\n";
    }

    close(accepted_fd);
    // TODO: Check error.
    accepted_fd = -1;
  }
}

}  // namespace myhttpd
