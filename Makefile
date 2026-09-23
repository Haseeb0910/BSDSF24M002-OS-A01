CC = gcc
CFLAGS = -Wall -Iinclude
PIC_FLAGS = -fPIC
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

LIB_SRCS = $(SRC_DIR)/mystrfunctions.c $(SRC_DIR)/myfilefunctions.c
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))
PIC_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.pic.o, $(LIB_SRCS))

TARGET = $(BIN_DIR)/client
STATIC_LIB = $(LIB_DIR)/libmyutils.a
STATIC_TARGET = $(BIN_DIR)/client_static
DYNAMIC_LIB = $(LIB_DIR)/libmyutils.so
DYNAMIC_TARGET = $(BIN_DIR)/client_dynamic

.PHONY: all clean static dynamic

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

static: $(STATIC_TARGET)

$(STATIC_LIB): $(LIB_OBJS)
	ar rcs $(STATIC_LIB) $(LIB_OBJS)
	ranlib $(STATIC_LIB)

$(STATIC_TARGET): $(OBJ_DIR)/main.o $(STATIC_LIB)
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lmyutils -o $(STATIC_TARGET)

# Dynamic library build
dynamic: $(DYNAMIC_TARGET)

# Separate PIC object files (.pic.o) so they don't collide with the
# regular non-PIC .o files used by the static/multi-file builds
$(OBJ_DIR)/%.pic.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(PIC_FLAGS) -c $< -o $@

$(DYNAMIC_LIB): $(PIC_OBJS)
	$(CC) -shared -o $(DYNAMIC_LIB) $(PIC_OBJS)

$(DYNAMIC_TARGET): $(OBJ_DIR)/main.o $(DYNAMIC_LIB)
	$(CC) $(OBJ_DIR)/main.o -L$(LIB_DIR) -lmyutils -o $(DYNAMIC_TARGET)

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/* $(LIB_DIR)/*.a $(LIB_DIR)/*.so
