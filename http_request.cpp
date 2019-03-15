#include "http_request.hpp"

namespace nvhttpd {

Http_request::Http_request(std::string request_header) {
  // todo: parse http request header and set members.

  /*
  // requested path
  std::string path = "";
  std::string path_string = "";
  std::string exe = executive_file;
  // find / position
  std::size_t pos = exe.rfind('/');
  // std::string::npos is the error indicator
  if (pos != std::string::npos) {
      exe = exe.substr(pos + 1);
  }

  string request_target;
  request_target = get_request_target(request_header);

  path = HTTP1_Parser::get_requestline_path(buf);
  path_string = HTTP1_Parser::argv_path_analyzer(path, HTML_FILE, exe.c_str());
  std::cout << "path_string: " << path_string << std::endl;

  // get requested file content
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

  vector<string> request_header_vector;
  stringstream ss{&request_header};
  string item;
  while (getline(ss, item, ' ')) {
    if (!item.empty()) {
     string_header_vector.push_back(item);
    }
  }
  return string_header_vector.at(1);
  */
}

std::string Http_request::get_method(void){
  return this->method;
}

std::string Http_request::get_request_target(void){
  return this->request_target;
}

std::string Http_request::get_http_version(void) {
  return this->http_version;
}

}  // namespace nvhttpd
