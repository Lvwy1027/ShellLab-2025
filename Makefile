# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# Directories
SRCDIR = src
BUILDDIR = build

# Find all source files in the source directory
SOURCES = $(wildcard $(SRCDIR)/*.c)

# Convert source file names to corresponding object file names in the build directory
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

# Final executable name
TARGET = tsh

# Additional executables
MYINT = myint
MYSPIN = myspin
MYSPLIT = mysplit
MYSTOP = mystop

# Default rule to build all executables
all: $(TARGET) $(MYINT) $(MYSPIN) $(MYSPLIT) $(MYSTOP)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Pattern rule to compile source files into object files in the build folder
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(MYINT): myint.c
	$(CC) $(CFLAGS) -o $@ $<

$(MYSPIN): myspin.c
	$(CC) $(CFLAGS) -o $@ $<

$(MYSPLIT): mysplit.c
	$(CC) $(CFLAGS) -o $@ $<

$(MYSTOP): mystop.c
	$(CC) $(CFLAGS) -o $@ $<

# Clean up build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILDDIR) $(TARGET) $(MYINT) $(MYSPIN) $(MYSPLIT) $(MYSTOP)
