# The following two variables must be set.
#
# Directory containing the C client include files.
DIFFUSION_C_CLIENT_INCDIR	= ./include
#
# Directory containing libdiffusion.a
DIFFUSION_C_CLIENT_LIBDIR	= ./lib

ifndef DIFFUSION_C_CLIENT_INCDIR
$(error DIFFUSION_C_CLIENT_INCDIR is not set)
endif

ifndef DIFFUSION_C_CLIENT_LIBDIR
$(error DIFFUSION_C_CLIENT_LIBDIR is not set)
endif

CC			= gcc

# Extra definitions from parent directory, if they exist.
-include ../makefile.defs

CFLAGS		+= -g -Wall -Werror -std=c99 -D_POSIX_C_SOURCE=200112L -D_XOPEN_SOURCE=700 -I$(DIFFUSION_C_CLIENT_INCDIR)
LDFLAGS		+= $(DIFFUSION_C_CLIENT_LIBDIR)/libdiffusion.a -lpthread -lpcre -lz $(LIBS)

LDFLAGS     += -lssl -lcrypto

# added per `brew link openssl` instructions
# LDFLAGS		+= -L/usr/local/opt/openssl/lib
CFLAGS		+= -I/usr/local/opt/openssl/include
CPPFLAGS	+= -I/usr/local/opt/openssl/include


#  Link Time optimisation changes (doesnt seem to change anything)
# CFLAGS		+= -flto -fno-lto
# LDFLAGS		+= -flto -fuse-linker-plugin

ARFLAGS		+=
SOURCES 	= connect.c

TARGETDIR	= target
OBJDIR		= $(TARGETDIR)/objs
BINDIR		= $(TARGETDIR)/bin
OBJECTS		= $(SOURCES:.c=.o)
TARGETS 	= connect

all:		prepare $(TARGETS)
.PHONY:		all

prepare:
		mkdir -p $(OBJDIR) $(BINDIR)

$(OBJDIR)/%.o:	%.c
		$(CC) $(CFLAGS) -c -o $@ $<

connect:	$(OBJDIR)/connect.o
		$(CC) $< $(LDFLAGS) -o $(BINDIR)/$@

clean:
		rm -rf $(TARGETS) $(OBJECTS) $(TARGETDIR) core a.out
