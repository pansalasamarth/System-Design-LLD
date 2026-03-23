# Makefile for Warehouse Order System

# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -pthread -std=c++17

# Target executable
TARGET = warehouse

# Source files
SRCS = Main.cpp Warehouse.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Compile executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile .cpp files into .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean object files and executable
clean:
	rm -f $(OBJS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)