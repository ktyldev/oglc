SRC_DIR = src
BIN_DIR = bin
RES_DIR = res

SHADER_DIR 			= $(RES_DIR)/shader
SHADER_QUAD_DIR 	= $(SHADER_DIR)/quad
SHADER_ROOT_DIR 	= $(SHADER_DIR)/root
SHADER_INCLUDE_DIR 	= $(SHADER_DIR)/include
SHADER_TARGET_DIR 	= $(BIN_DIR)/$(RES_DIR)

# find files in SHADER_ROOT_DIR
# top level compute shader programs
SHADERS = $(shell find $(SHADER_ROOT_DIR) -wholename "$(SHADER_ROOT_DIR)*.glsl")
# find files in SHADER_INCLUDE_DIR
# small chunks of shader code, included repeatedly in the top-level programs
SHADER_INCLUDES = $(shell find $(SHADER_INCLUDE_DIR) -name *.glsl)
SHADER_TARGETS = $(SHADERS:$(SHADER_ROOT_DIR)/%.glsl=$(SHADER_TARGET_DIR)/%.compute)

TARGET = $(BIN_DIR)/oglc
LAUNCH = "./launch"

CC = gcc
LIBS = `pkg-config --static --libs glew sdl2 cglm`
CFLAGS = -I$(SRC_DIR) -Wall

SRC = $(shell find $(SRC_DIR) -name *.c)
OBJ = $(SRC:%.c=%.o)

# create dirs if they dont exist
_dummy := $(shell mkdir -p $(BIN_DIR))

$(TARGET): $(OBJ) $(SHADER_TARGETS)
	mkdir -p $(BIN_DIR)
	mkdir -p $(SHADER_TARGET_DIR)
	cp $(shell find $(SHADER_QUAD_DIR) -wholename "$(SHADER_QUAD_DIR)/*") $(SHADER_TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

$(SHADER_TARGET_DIR)/%.compute: $(SHADER_ROOT_DIR)/%.glsl $(SHADER_INCLUDES)
	mkdir -p $(SHADER_TARGET_DIR)
	python ppp.py $< $(SHADER_INCLUDES) > $@

# how to make a .o out of a .c
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -r $(BIN_DIR)
	-rm */*.o

run: $(TARGET)
	$(LAUNCH) $(TARGET)

.PHONY: run clean
