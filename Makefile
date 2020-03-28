CXX := clang++
# CXXFLAGS := -g -std=c++17 -Wall -Wextra -pedantic -pedantic-errors \
#             -fno-exceptions -I.
CXXFLAGS := -g -std=c++17 -Wall -pedantic -pedantic-errors \
            -fno-exceptions -I.
BIN = myhttpd
OBJS = main.o server.o

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

main.o: main.cc
	$(CXX) $(CXXFLAGS) $< -c
server.o: server.cc
	$(CXX) $(CXXFLAGS) $< -c

.PHONY: clean
clean:
	$(RM) $(BIN) $(OBJS)
