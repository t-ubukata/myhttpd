#include "server.h"

namespace myhttpd {

namespace {
constexpr size_t kBufSz = 1024;
}

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
  char method_c_str[kBufSz] = {};
  char target_c_str[kBufSz] = {};
  char version_c_str[kBufSz] = {};
  auto ret = sscanf(request_header.c_str(), "%s %s %s", method_c_str,
                    target_c_str, version_c_str);
  std::string method(method_c_str);
  std::string target(target_c_str);
  std::string version(version_c_str);

  if (ret == EOF) {
    std::cerr << "sscanf() failed\n";
    return "HTTP/1.1 500 Internal Server Error\r\n\r\n<html><body><p>500 "
           "Internal Server Error</p></body></html>";
  }
  if (method != "GET") {
    return "HTTP/1.1 501 Not Implemented\r\n\r\n<html><body><p>501 Not "
           "Implemented</p></body></html>";
  }
  if (version != "HTTP/1.1") {
    return "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n<html><body><p>505 "
           "HTTP Version Not Supported</p></body></html>";
  }

  // Removes trailing slash.
  auto canonical_root_path = (root_path.substr(root_path.size() - 1) == "/")
                                 ? root_path.substr(0, root_path.size() - 1)
                                 : root_path;
  auto canonical_target = (target == "/")
                              ? (canonical_root_path + "/index.html")
                              : canonical_root_path + target;

  std::ifstream ifs(canonical_target);
  if (ifs.fail()) {
    return "HTTP/1.1 404 Not Found\r\n\r\n<html><body><p>404 Not "
           "Found</p></body></html>";
  }
  std::string body((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());

  return "HTTP/1.1 200 OK\r\n\r\n" + body;
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

    char buf[kBufSz] = {};
    std::string req_h;
    auto is_recv_succeeded = false;
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
        is_recv_succeeded = true;
        break;
      }
    }

    std::cerr << "Request header:\n" << req_h << "\n";
    auto res =
        is_recv_succeeded
            ? Response(req_h, root_path)
            : "HTTP/1.1 500 Internal Server Error\r\n\r\n<html><body><p>500 "
              "Internal Server Error</p></body></html>";

    if (send(accepted_fd, res.c_str(), res.length(), 0) == -1) {
      std::cerr << "send() failed: " << strerror(errno) << "\n";
    }

    close(accepted_fd);
    // TODO: Check error.
    accepted_fd = -1;
  }
}

}  // namespace myhttpd
