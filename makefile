SRC_DIR = src
BIN_DIR = bin
RES_DIR = res

SHADER_DIR = $(SRC_DIR)/_shader
SHADER_ROOT_DIR = $(SHADER_DIR)/root
SHADER_INCLUDE_DIR = $(SHADER_DIR)/common

TARGET = $(BIN_DIR)/oglc
CC = gcc
LIBS = `pkg-config --static --libs glew sdl2`
CFLAGS = -I$(SRC_DIR) -Wall

SRC = $(shell find $(SRC_DIR) -name *.c)
OBJ = $(SRC:%.c=%.o)

# find files in SHADER_ROOT_DIR
# top level compute shader programs
SHADERS = $(shell find $(SHADER_ROOT_DIR) -name *.glsl)

# find files in SHADER_INCLUDE_DIR
# small chunks of shader code, included repeatedly in the top-level programs
SHADER_INCLUDES = $(shell find $(SHADER_INCLUDE_DIR) -name *.glsl)

# create dirs if they dont exist
_dummy := $(shell mkdir -p $(BIN_DIR))

$(TARGET): $(OBJ)
	# preprocess shaders and store results in bin/res/shader/ under root name
	foreach root,$(SHADER_ROOT_DIR),$(echo $(root))

	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	cp -r $(RES_DIR) $(BIN_DIR)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -r $(BIN_DIR)
	-rm */*.o

run: $(TARGET)
	$(TARGET)


.PHONY: run clean
