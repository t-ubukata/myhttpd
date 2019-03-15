#include "server.hpp"

int main(int argc, char* argv[]) {
  nvhttpd::Server().Serve(8080);

  return EXIT_SUCCESS;
}
