BIN_NAME   := tlint
CC         := gcc
CC_FLAGS   := -Wall -Wextra -Wundef -Ofast
SRC_DIR    := src
OBJ_DIR    := build
BUILD_DIR  := ./build
TST_DIR    := ./tests
SRC_EXT    := .c
SRC_FILES  := $(wildcard $(SRC_DIR)/*$(SRC_EXT))
OBJ_FILES  := $(addprefix $(OBJ_DIR)/,$(notdir $(SRC_FILES:$(SRC_EXT)=.o)))

ifndef VERBOSE
.SILENT:
endif

all: $(BIN_NAME)

$(BIN_NAME): $(OBJ_FILES)
	$(CC) -o $@ $^ $(CC_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%$(SRC_EXT)
	$(CC) $(CXX_FLAGS) -c -o $@ $<


clean:
	rm -rf $(OBJ_FILES)
	rm -f ./$(BIN_NAME)

test:
	find $(TST_DIR) -name "*.[ch]"  > $(BUILD_DIR)/c_files.txt
	find $(SRC_DIR) -name "*.[ch]" >> $(BUILD_DIR)/c_files.txt
	./$(BIN_NAME) $(BUILD_DIR)/c_files.txt
	
