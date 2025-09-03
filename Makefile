BIN := main.a
INCLUDE := include/
SRC := $(wildcard src/*.c)

all: build run

build:
	gcc -o $(BIN) -I$(INCLUDE) $(wildcard *.c) $(SRC)

run:
	./$(BIN) 172.17.0.3 172.17.0.4