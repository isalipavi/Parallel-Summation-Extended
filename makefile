SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build

C = gcc

FLAGS = -lpthread -c

C_SRC = $(wildcard $(SRC_DIR)/*.c)
C_OBJ = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SRC))

TARGET = array_summation

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(C_OBJ) | $(BIN_DIR)
	$(C) $(C_OBJ) -o $@

$(BIN_DIR):
	@mkdir $(BIN_DIR)

$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(C) $(FLAGS) $< -o $@

run:
	@./$(BIN_DIR)/$(TARGET)

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
