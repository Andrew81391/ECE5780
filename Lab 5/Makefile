# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -g

# Source files
SRCS = main.cpp task.cpp input.cpp edf.cpp llf.cpp

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = scheduler

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)
