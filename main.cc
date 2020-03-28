#include "server.h"

int main(int argc, char* argv[]) {
  // TODO: Get args.
  (void)argc;
  (void)argv;
  uint16_t port = 8080;
  const char* root_path = "./";
  myhttpd::Serve(port, root_path);
}
