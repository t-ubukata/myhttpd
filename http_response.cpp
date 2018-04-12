#include "http_response.hpp"

namespace sshttpsv {

std::string Http_response::get_response(const Http_request http_request) const {
  std::string http_response_string;
  // todo:
  // get http_request member

  // resolve requested path
  /*
  path = HTTP1_Parser::get_requestline_path(buf);
  path_string = HTTP1_Parser::argv_path_analyzer(path, HTML_FILE, exe.c_str());
  std::cout << "path_string: " << path_string << std::endl;
  */

  // get requested file content
  /*
  std::ifstream output_file(path_string.c_str());
  // todo: what 256 means?
  // path length restriction?
  char line[256];
  // todo: change name to file_exists
  is_file_exist = output_file.fail();
  body_length = 0;
  std::vector<std::string> message_body;
  while (output_file.getline(line, 256 - 1)) {
      body_length += strlen(line);
      message_body.push_back(std::string(line));
  }

  // close requested file
  */

  return http_response_string;

}

}  // namespace sshttpsv
