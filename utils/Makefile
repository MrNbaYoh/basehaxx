CC = gcc

all: lzss

lzss: blz.h blz.c lzss.c
	@$(CC) -o $@ $?

clean:
	@rm -f lzss
	@rm -f lzss.exe
