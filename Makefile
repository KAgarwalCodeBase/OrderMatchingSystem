# Compiler and Flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread
LDFLAGS = -pthread

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source Files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/OrderBook.cpp \
       $(SRC_DIR)/MatchingEngine.cpp \
       $(SRC_DIR)/NetworkInterface.cpp

# Object Files
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cpp=.o)))

# Target Executable
TARGET = $(BIN_DIR)/matching_system

# Default Rule
all: $(TARGET)

# Create Binary
$(TARGET): $(OBJS)
	mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile Source Files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean Build Files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the Application
run: $(TARGET)
	./$(TARGET)

# Phony Targets
.PHONY: all clean run
