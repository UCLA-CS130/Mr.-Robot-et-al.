# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
CXX = g++
CFLAGS = -std=c++0x -I nginx-configparser/
TARGET = lightning

SRC = lightning_main.cc lightning_server.cc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CFLAGS) $(SRC) -o $(TARGET)

test:
	python3 lightning_integration_test.py

clean:
	$(RM) $(TARGET)