CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -O2
SOLOMON=reedsolomon/berlekamp.c reedsolomon/crcgen.c reedsolomon/galois.c reedsolomon/rs.c

all: bms2A bms2B


bms2A: bms2A.cpp
	$(CC) $(CFLAGS) $^ $(SOLOMON) -o $@ 

bms2B: bms2B.cpp
	$(CC) $(CFLAGS) $^ $(SOLOMON) -o $@ 

clean:
	rm -f *.o bms2A bms2B
