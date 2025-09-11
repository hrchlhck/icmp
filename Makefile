BIN := main.a
INCLUDE := include/
SRC := $(wildcard src/*.c)

all: build run

build:
	gcc -o $(BIN) -I$(INCLUDE) $(wildcard *.c) $(SRC)

run:
	./$(BIN) 192.168.50.113 1.1.1.1
