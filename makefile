CXX = clang++
CXXFLAGS = -std=c++17 -pedantic -Wall -Wextra -fno-exceptions
BIN = nvhttpd
OBJS = main.o socket.o http_request.o http_response.o

$(BIN): $(OBJS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

main.o: main.cpp
	$(CXX) $< -c $(CXXFLAGS)

socket.o: socket.cpp
	$(CXX) $< -c $(CXXFLAGS)

http_request.o: http_request.cpp
	$(CXX) $< -c $(CXXFLAGS)

http_response.o: http_response.cpp
	$(CXX) $< -c $(CXXFLAGS)

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJS)
