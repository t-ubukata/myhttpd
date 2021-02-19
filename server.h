#ifndef SERVER_H_
#define SERVER_H_

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace myhttpd {

void Serve(uint16_t port);

}  // namespace myhttpd

#define MYHTTPD_LOG(LEVEL_CHAR, MSG)                                         \
  do {                                                                       \
    auto tp = std::chrono::system_clock::now();                              \
    auto tt = std::chrono::system_clock::to_time_t(tp);                      \
    char time_str[20];                                                       \
    std::strftime(time_str, sizeof(time_str), "%F %T", std::localtime(&tt)); \
    std::cerr << time_str << ": " << #LEVEL_CHAR << " " << __FILE__ << ":"   \
              << __LINE__ << "] " << (MSG) << "\n";                          \
  } while (false)

#define MYHTTPD_LOG_INFO(MSG) MYHTTPD_LOG(I, (MSG))
#define MYHTTPD_LOG_WARN(MSG) MYHTTPD_LOG(W, (MSG))
#define MYHTTPD_LOG_ERROR(MSG) MYHTTPD_LOG(E, (MSG))

#define MYHTTPD_LOG_FATAL(MSG) \
  do {                         \
    MYHTTPD_LOG(F, (MSG));     \
    abort();                   \
  } while (false)

#define MYHTTPD_CHECK(COND, MSG) \
  do {                           \
    if (!(COND)) {               \
      MYHTTPD_LOG_FATAL((MSG));  \
    }                            \
  } while (false)

#define MYHTTPD_CHECK_ERRNO(COND, MSG)                  \
  do {                                                  \
    if (!(COND)) {                                      \
      auto e = strerror(errno);                         \
      MYHTTPD_LOG_FATAL(std::string((MSG)) + ": " + e); \
    }                                                   \
  } while (false)

#endif  // SERVER_H_
