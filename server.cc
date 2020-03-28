#include "server.h"

namespace nvhttpd {

Server::Server() noexcept {
  socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd_ < 0) {
    std::cerr << "socket() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }
}

void Server::Serve(const int port, const std::string& domain,
                   const std::string& root_path) noexcept {

  // TODO: Use.
  (void)domain;
  (void)root_path;

  SetOption();
  Bind(port);
  Listen();

  std::cout << "Serving..." << std::endl;

  // Main loop.
  while (true) {
    auto accepted_socket_fd {accept(socket_fd(), (struct sockaddr*)NULL, NULL)};
    // Continues when accept() fails.
    if (accepted_socket_fd == -1) {
        continue;
    }

    char buf[1024] {0};
    std::string request_header;

    // Receives message until meeting CRLFCRLF.
    while (true) {
      // Gives sizeof(buf) -1 because of NULL termination.
      auto read_size {recv(accepted_socket_fd, buf, sizeof(buf) - 1, 0)};
      if (read_size == -1) {
        std::cerr << "read() failed." << std::endl;
        std::cerr << "ERROR: " << strerror(errno) << std::endl;
        close(accepted_socket_fd);
        socket_fd_ = -1;
        break;
      }

      if (read_size > 0) {
        request_header.append(buf);
      }

      // Checks end of header.
      constexpr unsigned char kCR {0x0d};
      constexpr unsigned char kLF {0x0a};
      if ((request_header[request_header.length() - 4] == kCR) &&
          (request_header[request_header.length() - 3] == kLF) &&
          (request_header[request_header.length() - 2] == kCR) &&
          (request_header[request_header.length() - 1] == kLF)) {
        break;
      }

      if (read_size <= 0) {
        // TODO: Return Bad Request.
        break;
      }
    }

    // Get response.
    auto response = Response(request_header);

    // Writes response.
    if (send(accepted_socket_fd, response.c_str(),
        response.length(), 0) == -1) {
      std::cerr << "write() failed." << std::endl;
      std::cerr << "ERROR: " << strerror(errno) << std::endl;
    }

    // Closes accepted socket.
    close(accepted_socket_fd);
    accepted_socket_fd = -1;
  }
}

int Server::socket_fd() const noexcept {
  return socket_fd_;
}

void Server::SetOption() noexcept {
  // Sets level SOL_SOCKET to operate option in socket layer (required)
  // SO_REUSEADDR: you can reuse address as long as not listened
  // argument is an integer boolean flag.
  // optval 1 means true.
  // For more info, see man page of socket(2)
  int optval {1};
  if (setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &optval,
      sizeof(optval))) {
    std::cerr << "setsockopt() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    // Close requires unistd.h.
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

void Server::Bind(const int port) noexcept {
  // Initializes sock_addr_in structure type variable
  // zero padding to initialize
  // reference operator is prior to address operator.
  struct sockaddr_in socket_address_in {0, AF_INET, htonl(INADDR_ANY),
      {htons(port)}, {0}};

  // memset(socket_address_in, 0, sizeof(this->serv_addr));

  // AF_INET means IPv4.
  // socket_address_in.sin_family = AF_INET;
  // htonl stands for host to net long
  // INADDR_ANY means any IP address
  // socket_address_in.sin_addr.s_addr = htonl(INADDR_ANY);
  // htons stands for host to net short
  // socket_address_in.sin_port = htons(port);

  // struct sockaddr: The only purpose of this structure is
  //   to cast the structure pointer
  //   passed in addr in order to avoid compiler warnings.
  if (bind(socket_fd_, (struct sockaddr*)&socket_address_in,
      sizeof(socket_address_in)) == -2) {
    std::cerr << "bind() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

void Server::Listen() noexcept {
  if (listen(socket_fd_, SOMAXCONN) == -1) {
    std::cerr << "listen() failed." << std::endl;
    std::cerr << "ERROR: " << strerror(errno) << std::endl;
    close(socket_fd_);
    exit(EXIT_FAILURE);
  }
}

std::string Response(const std::string& http_request) {
  // TODO: Implement
  (void)http_request;

  // Get target path

  // Get file content

  return "Hello, World.";
}

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

Http_request::Http_request(std::string request_header) {
  (void)request_header;
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

class Http_response {
 public:
  std::string get_response(const Http_request http_request) const;
};

std::string Http_response::get_response(const Http_request http_request) const {
  (void)http_request;
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

}  // namespace nvhttpd
