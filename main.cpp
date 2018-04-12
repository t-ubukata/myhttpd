#include "socket.hpp"

int main(int argc, char* argv[]) {
  sshttpsv::Socket sock {};
  sock.Wait(8080);

  return EXIT_SUCCESS;
}
