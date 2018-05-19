# compiler to use
CC = gcc

# flags to pass compiler
CFLAGS = -ggdb3 -std=c11 -Wall

# name for executable
EXE = brainfuck

# space-separated list of header files
HDRS = stack.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS =

# space-separated list of source files
SRCS = stack.c brainfuck.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
$(EXE): $(OBJS) $(HDRS) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies
$(OBJS): $(HDRS) Makefile

# housekeeping
clean:
	rm -f core $(EXE) *.o
