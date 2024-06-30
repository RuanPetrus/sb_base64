BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj

CFLAGS=-Wall -Wextra -Wshadow -std=c99

TARGET=$(BUILD_DIR)/base64

$(TARGET): main.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGS) -o $@ $^
