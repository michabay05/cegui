SRC=src
SRCS=$(wildcard $(SRC)/*.cpp)

INC=vendor src 
HDRS=$(wildcard $(INC)/*.hpp)

OBJ=obj
OBJS=$(patsubst $(SRC)/%.cpp,$(OBJ)/%.o,$(SRCS))

BIN_DIR=bin
BIN=$(BIN_DIR)/cegui

CC=g++
LIBS=-lraylib -lm
CFLAGS=-Wall -Wextra -pedantic -std=c++2a

.PHONY: clean

$(BIN): $(OBJS) $(OBJ) $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LIBS)

#obj/%.o: $(SRC)/%.cpp
$(OBJ)/%.o: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

clean:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN_DIR)
