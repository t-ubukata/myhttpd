CXX := clang++
CXXFLAGS := -g -std=c++17 -Wall -Wextra -pedantic -pedantic-errors \
            -fno-exceptions -I.
BIN := myhttpd
OBJS := main.o server.o

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cc
	$(CXX) $(CXXFLAGS) $^ -c -o $@
server.o: server.cc
	$(CXX) $(CXXFLAGS) $^ -c -o $@

.PHONY: format clean

format:
	clang-format -i -style=Google *.cc *.h

clean:
	$(RM) $(BIN) $(OBJS)
