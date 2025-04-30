# Makefile for IMDB

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -pthread -I./src

# Source files
SRC = \
src/datastore/DataStore.cpp \
src/command/CommandParser.cpp \
src/server/Server.cpp \
src/persistence/PersistenceManager.cpp \
src/logger/Logger.cpp \
src/worker/WorkerPool.cpp \
src/crypto/SHA256.cpp \
src/auth/AuthManager.cpp \
src/utils/Utils.cpp \
src/subscription/subscriptionManager.cpp \
src/main.cpp

# Output executable name
TARGET = inmemorydb

# Default build target
all: $(TARGET)

# How to build
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)
