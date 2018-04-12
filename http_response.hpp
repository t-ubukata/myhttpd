#ifndef _HTTP_RESPONSE_HPP_
#define _HTTP_RESPONSE_HPP_

#include <string>
#include <vector>
#include <sstream>

#include "http_request.hpp"

namespace sshttpsv {

class Http_response {
  public:
    std::string get_response(const Http_request http_request) const;
};

}  // namespace sshttpsv

#endif
