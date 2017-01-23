# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

# TODO: CXX is a predefined variable. What is its default value on different platforms?
# TODO: MAKEOPTS += 2 (to parallelize building with 2 commands at once)
CXX = g++
SRC_FLAGS = -std=c++0x -I nginx-configparser/ -Wall -Wextra
# TODO: This should be named LDFLAGS for consistency with the Makefile community
LIB_FLAGS = -lpthread -lboost_system
PARSER_PATH = ./nginx-configparser/

# TODO: Make a phony, so that targets are not treated as files on-disk
# TODO: Split out dependency handling so that we can avoid rebuilding

TARGET = lightning
SRC = $(PARSER_PATH)config_parser.cc lightning_main.cc lightning_server.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC_FLAGS) $(SRC) $(LIB_FLAGS) -o $(TARGET)

test:
	python3 lightning_integration_test.py

clean:
	$(RM) $(TARGET)
