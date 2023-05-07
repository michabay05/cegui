SRCDIR = src
INCDIR = include
VENDORDIR = include/vendor
OBJDIR_DEBUG = obj/debug
OBJDIR_RELEASE = obj/release
BINDIR_DEBUG = bin/debug
BINDIR_RELEASE = bin/release

CXX = g++
COMMON_CXXFLAGS = -Wall -Wextra -pedantic -std=c++2a -I$(INCDIR) -I$(VENDORDIR)
CXXFLAGS_DEBUG = -g
CXXFLAGS_RELEASE = -O3
LDFLAGS = -lraylib -lm

SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS_DEBUG = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR_DEBUG)/%.o, $(SOURCES))
OBJECTS_RELEASE = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR_RELEASE)/%.o, $(SOURCES))
BIN_NAME = cegui
BINARY_DEBUG = $(BINDIR_DEBUG)/$(BIN_NAME)
BINARY_RELEASE = $(BINDIR_RELEASE)/$(BIN_NAME)

.PHONY: all clean debug debug_setup release release_setup

all: debug release

debug: debug_setup $(BINARY_DEBUG)

debug_setup:
	mkdir -p $(OBJDIR_DEBUG)
	mkdir -p $(BINDIR_DEBUG)

$(BINARY_DEBUG): $(OBJECTS_DEBUG)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR_DEBUG)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(CXXFLAGS_DEBUG) -c $< -o $@

release: release_setup $(BINARY_RELEASE)

release_setup:
	mkdir -p $(OBJDIR_RELEASE)
	mkdir -p $(BINDIR_RELEASE)

$(BINARY_RELEASE): $(OBJECTS_RELEASE)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR_RELEASE)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(COMMON_CXXFLAGS) $(CXXFLAGS_RELEASE) -c $< -o $@

clean:
	rm -rf $(OBJDIR_DEBUG)/* $(OBJDIR_RELEASE)/* $(BINDIR_DEBUG)/* $(BINDIR_RELEASE)/*
