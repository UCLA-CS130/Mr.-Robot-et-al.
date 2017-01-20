# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
CXX = g++
CFLAGS = -std=c++0x -I nginx-configparser/
TARGET = lightning

all: $(TARGET)

$(TARGET): lightning_main.cc
	$(CXX) $(CFLAGS) lightning_main.cc -o $(TARGET)

test:
	python3 lightning_integration_test.py

clean:
	$(RM) $(TARGET)