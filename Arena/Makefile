# Compiler and Linke
CC := gcc
CFLAGS := -DNDEBUG -Wall -Wextra -O2 -pedantic
TEST_CFLAGS := -DDEBUG -DARENA_DEBUG -Wall -Wextra -pedantic -fno-plt -fno-pie -g -O0
TEST_LDFLAGS := -no-pie

# Directories
SRC_DIR := ./
BUILD_DIR := ./build
TEST_DIR := ./test

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.t.o, $(TEST_SRCS)) \
             $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.t.o, $(SRCS))


# Build target
all: $(OBJS)

# Compile each .c file to .o
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if not exists
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

test: $(TEST_OBJS)
	$(CC) $(TEST_CFLAGS) $(TEST_LDFLAGS) $(TEST_OBJS) -o $(BUILD_DIR)/test_binary
	@echo "Running tests..."
	@$(BUILD_DIR)/test_binary

$(BUILD_DIR)/%.t.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.t.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

.PHONY:clean
# Clean build files
clean:
	rm -rf $(BUILD_DIR)
