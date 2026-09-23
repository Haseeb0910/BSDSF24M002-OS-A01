CC = gcc
CFLAGS = -Wall -Iinclude
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Separate main.o from the library objects — main.c is the driver,
# not part of the library itself
LIB_SRCS = $(SRC_DIR)/mystrfunctions.c $(SRC_DIR)/myfilefunctions.c
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))

TARGET = $(BIN_DIR)/client
STATIC_LIB = $(LIB_DIR)/libmyutils.a
STATIC_TARGET = $(BIN_DIR)/client_static

.PHONY: all clean static

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build the static library from the library object files (not main.o)
static: $(STATIC_TARGET)

$(STATIC_LIB): $(LIB_OBJS)
	ar rcs $(STATIC_LIB) $(LIB_OBJS)
	ranlib $(STATIC_LIB)

$(STATIC_TARGET): $(OBJ_DIR)/main.o $(STATIC_LIB)
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lmyutils -o $(STATIC_TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/* $(LIB_DIR)/*.a
