SRC_DIR = src
BIN_DIR = bin
RES_DIR = res

TARGET = $(BIN_DIR)/oglc
CC = gcc
LIBS = `pkg-config --static --libs glew sdl2`
CFLAGS = -I$(SRC_DIR) -Wall

SRC = $(shell find $(SRC_DIR) -name *.c)
OBJ = $(SRC:%.c=%.o)

# create dirs if they dont exist
_dummy := $(shell mkdir -p $(BIN_DIR))

$(TARGET): $(OBJ)
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
