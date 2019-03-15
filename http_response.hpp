#ifndef HTTP_RESPONSE_HPP_
#define HTTP_RESPONSE_HPP_

#include <string>
#include <vector>
#include <sstream>

#include "http_request.hpp"

namespace nvhttpd {

class Http_response {
 public:
  std::string get_response(const Http_request http_request) const;
};

}  // namespace nvhttpd

#endif  // HTTP_RESPONSE_HPP_
