CC     := x86_64-w64-mingw32-gcc
LD     := x86_64-w64-mingw32-ld
OBJCPY := x86_64-w64-mingw32-objcopy

SRC_DIR   := src
INC_DIR   := include
BUILD_DIR := build

TARGET_EXE := $(BUILD_DIR)/shellcode.exe
TARGET_BIN := $(BUILD_DIR)/shellcode.bin

CFLAGS := \
	-Wall \
	-m64 \
	-ffunction-sections \
	-fno-asynchronous-unwind-tables \
	-fno-stack-protector \
	-nostdlib \
	-fno-ident \
	-O2 \
	-Wno-array-bounds \
	-I$(INC_DIR)

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean re

all: $(TARGET_BIN)

$(TARGET_EXE): $(OBJS) | $(BUILD_DIR)
	$(LD) -T shellcode.ld --gc-sections -s $^ -o $@

$(TARGET_BIN): $(TARGET_EXE)
	$(OBJCPY) -O binary -j .text $< $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)

re: clean all