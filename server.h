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
#include <string>

namespace myhttpd {

void Serve(uint16_t port, const std::string& root_path);

}  // namespace myhttpd

#define LOG_FATAL(MSG)                                                       \
  do {                                                                       \
    const std::chrono::system_clock::time_point tp =                         \
        std::chrono::system_clock::now();                                    \
    const std::time_t tt = std::chrono::system_clock::to_time_t(tp);         \
    char time_str[20];                                                       \
    std::strftime(time_str, sizeof(time_str), "%F %T", std::localtime(&tt)); \
    std::cerr << time_str << ":"                                             \
              << " F " << __FILE__ << ":" << __LINE__ << "] " << (MSG)       \
              << "\n";                                                       \
    abort();                                                                 \
  } while (false)

#define CHECK(COND, MSG) \
  do {                   \
    if (!(COND)) {       \
      LOG_FATAL((MSG));  \
    }                    \
  } while (false)

#define CHECK_ERRNO(COND, MSG)                                             \
  do {                                                                     \
    if (!(COND)) {                                                         \
      LOG_FATAL(std::string((MSG)) + ": " + std::string(strerror(errno))); \
    }                                                                      \
  } while (false)

#endif  // SERVER_H_
