# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html

# TODO: CXX is a predefined variable. What is its default value on different platforms?
# TODO: MAKEOPTS += 2 (to parallelize building with 2 commands at once)
CXX = g++
SRC_FLAGS = -std=c++0x -I nginx-configparser/ -Wall -Wextra
# TODO: This should be named LDFLAGS for consistency with the Makefile community
LIB_FLAGS = -lpthread -lboost_system
PARSER_PATH = ./nginx-configparser/
GTEST_DIR = nginx-configparser/googletest/googletest
GTEST_FLAGS = -isystem ${GTEST_DIR}/include

# TODO: Split out dependency handling so that we can avoid rebuilding
.PHONY: $(TARGET) $(TESTS) all test clean

TARGET = lightning
TESTS = lightning_server_test
SRC = $(PARSER_PATH)config_parser.cc lightning_main.cc lightning_server.cc

define \n


endef

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(SRC_FLAGS) $(SRC) $(LIB_FLAGS) -o $(TARGET)

# TODO: Generalize for multiple unit tests
# TODO: These assume compilation on LINUX, need to add OS check
$(TESTS):
	$(CXX) $(SRC_FLAGS) $(GTEST_FLAGS) -I${GTEST_DIR} -c ${GTEST_DIR}/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o
	$(CXX) $(SRC_FLAGS) $(GTEST_FLAGS) $(TESTS).cc ${GTEST_DIR}/src/gtest_main.cc libgtest.a $(LIB_FLAGS) -o $(TESTS)

test: $(TARGET) $(TESTS)
	./$(TESTS)
	python3 lightning_integration_test.py

clean:
	$(RM) $(TARGET) $(TESTS) *.o *.a
