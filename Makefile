CFLAGS=-std=c99 -Oz -Wall -Wextra -Weverything
CC=clang
SOURCE=main.c
BINARY=bd

all: $(BINARY)

$(BINARY): $(SOURCE)
	$(CC) $(CFLAGS) -o $(BINARY) $(SOURCE)

clean:
	rm ./bd

