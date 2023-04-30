SRC=src
SRCS=$(wildcard $(SRC)/*.cpp)

INC=vendor src/include 
HDRS=$(wildcard $(INC)/*.hpp)

OBJ=obj
OBJS=$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))

BIN_DIR=bin
BIN=bin/cig

CC=g++
LIBS=-lraylib -lm
CFLAGS=-Wall -Wextra -pedantic -std=c++2a -I$(HDRS) 

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS) $(OBJ) $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJ)/%.o: $(SRC)/%.cpp $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN_DIR)

run:
	clear
	$(MAKE) all
	$(BIN) $(MODE)
