# Based on example Makefile:
# https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html
CC = g++
CFLAGS = -std=c++11 -I nginx-configparser/
TARGET = lightning

all: $(TARGET)

$(TARGET): lightning_main.cc
	$(CC) $(CFLAGS) lightning_main.cc -o $(TARGET)

clean:
	$(RM) $(TARGET)