CC = clang++
CFLAGS = -std=c++14 -pedantic -Wall -Wextra -fno-exceptions
PROGRAM = sshttpsv
OBJS = main.o socket.o http_request.o http_response.o

$(PROGRAM): $(OBJS)
	$(CC) $^ -o $@ $(CFLAGS)

main.o: main.cpp
	$(CC) $< -c $(CFLAGS)

socket.o: socket.cpp
	$(CC) $< -c $(CFLAGS)

http_request.o: http_request.cpp
	$(CC) $< -c $(CFLAGS)

http_response.o: http_response.cpp
	$(CC) $< -c $(CFLAGS)

.PHONY: clean
clean:
		$(RM) $(PROGRAM) $(OBJS)
