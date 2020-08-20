# _puts_c/Makefile - build rules for compiling the `_puts_c` module (C implementation)

# kscript directory
KS_DIR   ?= ../../../../
#KS_DIR   ?= /usr/local
#KS_DIR   ?=

# C source code
mod_C    := module.c

# generated object files
mod_O    := $(patsubst %.c,%.o,$(mod_C))


# if KS_DIR is given, add flags
ifneq ($(KS_DIR),)
  CFLAGS   += -I$(KS_DIR)include 
  LDFLAGS  += -L$(KS_DIR)lib
endif

# -*- RULES -*-

# compile single file to single object output
%.o: %.c
	$(CC) $(CFLAGS) $< -fPIC -c -o $@

# compile all object files to the shared library (which can be imported)
libksm__puts_c.so: $(mod_O)
	$(CC) $(LDFLAGS) $^ -shared -o $@
