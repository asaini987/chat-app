# Directories and compiler settings
IDIR = include
CC = gcc
CFLAGS = -I$(IDIR) -Wall -Wextra -g -pthread
ODIR = obj
SDIR = src
LDIR = lib
# TDIR = test
LIBS = -lm
XXLIBS = -lpthread $(LIBS)

# Header dependencies
_DEPS = client.h server.h task.h thread_pool.h user.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

# Source and object files
_MAIN_CLIENT_SRC = main_client.c
_MAIN_SERVER_SRC = main_server.c
_CLIENT_SRC = client.c
_SERVER_SRC = server.c
_ADDITIONAL_SRC = task.c thread_pool.c user.c  # List your additional source files here

MAIN_CLIENT_OBJ = $(patsubst %,$(ODIR)/%,$(_MAIN_CLIENT_SRC:.c=.o))
MAIN_SERVER_OBJ = $(patsubst %,$(ODIR)/%,$(_MAIN_SERVER_SRC:.c=.o))
CLIENT_OBJ = $(patsubst %,$(ODIR)/%,$(_CLIENT_SRC:.c=.o))
SERVER_OBJ = $(patsubst %,$(ODIR)/%,$(_SERVER_SRC:.c=.o))
ADDITIONAL_OBJ = $(patsubst %,$(ODIR)/%,$(_ADDITIONAL_SRC:.c=.o))

# Targets
APPBIN = server_app
CLIENTBIN = client_app

# Rule to ensure the object directory exists
$(shell mkdir -p $(ODIR))

# Rules to compile source files into object files
$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Rule to build the main application
$(APPBIN): $(MAIN_SERVER_OBJ) $(SERVER_OBJ) $(ADDITIONAL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Rule to build the client application
$(CLIENTBIN): $(MAIN_CLIENT_OBJ) $(CLIENT_OBJ) $(ADDITIONAL_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# Default target
all: $(APPBIN) $(CLIENTBIN)

.PHONY: clean

# Rule for cleaning up generated files
clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~
	rm -f $(APPBIN) $(CLIENTBIN)
