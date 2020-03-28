#include "server.h"

int main(int argc, char* argv[]) {
  // TODO: Get args.
  (void)argc;
  (void)argv;
  const int port {8080};
  const std::string domain {"machine.local"};
  const std::string root_path {"/Users/admin/project/nvhttpd/test/data/http_root"};
  nvhttpd::Server().Serve(port, domain, root_path);

  return EXIT_SUCCESS;
}
