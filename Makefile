# Compiler and archiver
CC = gcc
AR = ar

# Directories
INCDIR = inc
SRCDIR = src
OBJDIR = obj
LIBDIR = lib
BINDIR = bin
TESTDIR = test

# Library name
LIB_NAME = libz_share_memory
LIB = $(LIBDIR)/lib$(LIB_NAME).a

# Source files (excluding test files)
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

# Test programs
TEST_SEND = $(BINDIR)/test_send
TEST_RECV = $(BINDIR)/test_recv

# Compiler and linker flags
CFLAGS = -Wall -I$(INCDIR)
LDFLAGS = -L$(LIBDIR) -l$(LIB_NAME) -pthread -lrt

# Default target
all: prepare $(LIB) $(TEST_SEND) $(TEST_RECV)

# Create necessary directories
prepare:
	@mkdir -p $(OBJDIR)
	@mkdir -p $(LIBDIR)
	@mkdir -p $(BINDIR)

# Build static library
$(LIB): $(OBJ)
	$(AR) rcs $@ $^

# Compile source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(OBJDIR)/test_send.o: $(TESTDIR)/test_send.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/test_recv.o: $(TESTDIR)/test_recv.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link test programs
$(TEST_SEND): $(OBJDIR)/test_send.o $(LIB)
	$(CC) $< -o $@ $(LDFLAGS)

$(TEST_RECV): $(OBJDIR)/test_recv.o $(LIB)
	$(CC) $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) $(LIBDIR) $(BINDIR)

.PHONY: all clean prepare