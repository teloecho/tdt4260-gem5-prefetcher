CC := gcc
CROSS_COMPILE := 

CFLAGS=-O2 -g
LDFLAGS=-static

STRIDE_OFFSET=64

.PHONY: all clean

all: random strided stream complex


random: random.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) $^ -o $@

random.o: %.o : %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -c $^ -o $@

stream strided: % : %.o
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) $^ -o $@

strided.o: CFLAGS += -DSTRIDED -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
complex.o: CFLAGS +=-DCOMPLEX_STRIDE -DSTRIDE_OFFSET=$(STRIDE_OFFSET)
stream.o strided.o complex.o: strided.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) $(DEFINES) -c $^ -o $@

clean:	
	rm -f *.o
	rm -f stream strided random complex
