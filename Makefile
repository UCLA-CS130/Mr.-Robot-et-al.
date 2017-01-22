# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
CXX = g++
SRC_FLAGS = -std=c++0x -I nginx-configparser/
LIB_FLAGS = -L/usr/lib/x86_64-linux-gnu -lboost_system
PARSER_PATH = ./nginx-configparser/

TARGET = lightning
SRC = $(PARSER_PATH)config_parser.cc lightning_main.cc lightning_server.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC_FLAGS) $(SRC) $(LIB_FLAGS) -o $(TARGET)

test:
	python3 lightning_integration_test.py

clean:
	$(RM) $(TARGET)