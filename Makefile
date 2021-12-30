
CFLAGS = -Wall -o
CC = g++

morseCodeEncoder: morseCodeEncoder.cpp
	$(CC) $(CFLAGS) morseCodeEncoder morseCodeEncoder.cpp

.PHONY clean:
	rm -f *.o
	rm -f morseCodeEncoder
