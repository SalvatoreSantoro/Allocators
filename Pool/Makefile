### COMPILER AND LINKER
CC := gcc
CFLAGS := -std=c99 -DNDDEBUG -Wall -Wextra -Werror -O2 -pedantic -Wpadded

CFLAGS_DEBUG := -std=c99 -DDEBUG -DPOOL_DEBUG -Wall -Wextra -Werror -pedantic -O0 -g

TEST_CFLAGS := $(CFLAGS_DEBUG) -fno-plt -fno-pie 
TEST_LDFLAGS := -no-pie

### DIRECTORIES
SRC_DIR := ./src
BUILD_DIR := ./build
TEST_DIR := ./test
LIB_NAME := pool.a
LIB_NAME_DEBUG := pool_debug.a

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
OBJS_DEBUG := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.d.o, $(SRCS))

TEST_SRCS := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJS := $(patsubst $(TEST_DIR)/%.c, $(BUILD_DIR)/%.t.o, $(TEST_SRCS)) \
             $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.t.o, $(SRCS))

### BUILD
all: $(BUILD_DIR)/$(LIB_NAME)

$(BUILD_DIR)/$(LIB_NAME): $(OBJS)
	ar rcs $@ $^


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


### DEBUG
debug: $(BUILD_DIR)/$(LIB_NAME_DEBUG)

$(BUILD_DIR)/$(LIB_NAME_DEBUG): $(OBJS_DEBUG)
	ar rcs $@ $^

$(BUILD_DIR)/%.d.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS_DEBUG) -c $< -o $@



### CREATE BUILD DIRECTORY IF NOT EXISTS
$(BUILD_DIR):
	@"mkdir" $(BUILD_DIR)

### TEST TARGET
test: $(TEST_OBJS)
	$(CC) $(TEST_CFLAGS) $(TEST_LDFLAGS) $(TEST_OBJS) -o $(BUILD_DIR)/test_binary
	@echo "Running tests..."
	$(BUILD_DIR)/test_binary

$(BUILD_DIR)/%.t.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.t.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(TEST_CFLAGS) -c $< -o $@

### CLEAN BUILD FILES
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
