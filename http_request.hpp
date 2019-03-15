#ifndef HTTP_REQUEST_HPP_
#define HTTP_REQUEST_HPP_

#include <string>

namespace nvhttpd {

class Http_request {
 public:
  explicit Http_request(std::string request_header);
  std::string get_method(void);
  std::string get_request_target(void);
  std::string get_http_version(void);

 private:
  const std::string method;
  const std::string request_target;
  const std::string http_version;
};

}  // namespace nvhttpd

#endif  // HTTP_REQUEST_HPP_
