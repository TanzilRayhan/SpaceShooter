CC = g++
CFLAGS = -Wall -std=c++17 -MMD -MP
LDFLAGS = -lopengl32 -lfreeglut -lglew32 -lglu32 -lwinmm

SRC_DIR = src
BIN_DIR = bin
# P_LOCAL_INCLUDE = deps\include

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BIN_DIR)/%.o,$(SRCS))

TARGET = $(BIN_DIR)/main

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -I.

$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c -o $@ $< -I.

run: $(TARGET)
	./$(TARGET)

clean:
	del -f $(OBJS) $(OBJS:.o=.d) $(TARGET)

-include $(OBJS:.o=.d)