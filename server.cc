#include "server.h"

namespace myhttpd {

namespace {
constexpr size_t kBufSz = 1024;
constexpr const char* kRes200Hdr = "HTTP/1.1 200 OK\r\n\r\n";
constexpr const char* kRes404 =
    "HTTP/1.1 404 Not Found\r\n\r\n<html><body><p>404 Not "
    "Found</p></body></html>";
constexpr const char* kRes500 =
    "HTTP/1.1 500 Internal Server Error\r\n\r\n<html><body><p>500 Internal "
    "Server Error</p></body></html>";
constexpr const char* kRes501 =
    "HTTP/1.1 501 Not Implemented\r\n\r\n<html><body><p>501 Not "
    "Implemented</p></body></html>";
constexpr const char* kRes505 =
    "HTTP/1.1 505 HTTP Version Not Supported\r\n\r\n<html><body><p>505 HTTP "
    "Version Not Supported</p></body></html>";
}  // namespace

int Init(uint16_t port) {
  auto fd = socket(AF_INET, SOCK_STREAM, 0);
  MYHTTPD_CHECK_ERRNO(fd != -1, "socket() failed.");

  int optval = 1;
  auto setsockopt_ret =
      setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
  MYHTTPD_CHECK_ERRNO(setsockopt_ret == 0, "setsockopt() failed.");

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  auto bind_ret =
      bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
  MYHTTPD_CHECK_ERRNO(bind_ret == 0, "bind() failed.");

  auto listen_ret = listen(fd, SOMAXCONN);
  MYHTTPD_CHECK_ERRNO(listen_ret == 0, "listen() failed.");

  return fd;
}

std::string Response(std::string_view request_header) {
  char method_c_str[kBufSz] = {};
  char target_c_str[kBufSz] = {};
  char version_c_str[kBufSz] = {};
  auto ret = sscanf(request_header.data(), "%s %s %s", method_c_str,
                    target_c_str, version_c_str);
  if (ret == EOF) {
    auto sscanf_err = strerror(errno);
    MYHTTPD_LOG_ERROR(std::string("sscanf() failed: ") + sscanf_err);
    return kRes500;
  }
  std::string method(method_c_str);
  std::string target(target_c_str);
  std::string version(version_c_str);

  if (method != "GET") {
    return kRes501;
  }
  if (version != "HTTP/1.1") {
    return kRes505;
  }

  auto canonical_target = (target == "/") ? "./index.html" : "./" + target;

  std::ifstream ifs(canonical_target);
  if (ifs.fail()) {
    return kRes404;
  }
  std::string body((std::istreambuf_iterator<char>(ifs)),
                   std::istreambuf_iterator<char>());

  return kRes200Hdr + body;
}

void Serve(uint16_t port) {
  auto fd = Init(port);

  while (true) {
    auto accepted_fd =
        accept(fd, static_cast<struct sockaddr*>(nullptr), nullptr);
    // Continues when accept() fails.
    if (accepted_fd == -1) {
      auto e = strerror(errno);
      MYHTTPD_LOG_ERROR(std::string("accept() failed: ") + e);
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
        auto recv_err = strerror(errno);
        MYHTTPD_LOG_ERROR(std::string("recv() failed: ") + recv_err);
        accepted_fd = -1;
        auto close_ret = close(accepted_fd);
        if (close_ret != 0) {
          auto close_err = strerror(errno);
          MYHTTPD_LOG_ERROR(std::string("close() failed: ") + close_err);
        }

        // Response is 500.
        break;
      }
      req_h.append(buf);
      if (req_h.size() >= 4 && req_h.substr(req_h.size() - 4) == "\r\n\r\n") {
        is_recv_succeeded = true;
        MYHTTPD_LOG_INFO("Request header:\n" + req_h);
        break;
      }
    }

    auto res = is_recv_succeeded ? Response(req_h) : kRes500;
    auto send_ret = send(accepted_fd, res.c_str(), res.length(), 0);
    MYHTTPD_CHECK_ERRNO(send_ret != -1, "send() failed.");

    auto close_ret = close(accepted_fd);
    if (close_ret != 0) {
      auto close_err = strerror(errno);
      MYHTTPD_LOG_ERROR(std::string("close() failed: ") + close_err);
    }
    accepted_fd = -1;
  }
}

}  // namespace myhttpd
