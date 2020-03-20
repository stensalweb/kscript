# m/Makefile - Makefile for the math library

# -*- configuration -*-

# the kscript installation directory. Use `../../` to use the local one,
#   or `/usr/local` (or another path that it was installed to) for the system-wide one
KS_DIR         ?= ../..
#KS_DIR         ?= /usr/local

# module name
MOD_NAME       := m

# CFLAGS for compiling this module, just need to include kscript
mod_CFLAGS     ?= $(CFLAGS) -I$(KS_DIR)/include

# linker flags for compiling this module, just need to link to kscript & the C math library
mod_LDFLAGS    ?= $(LDFLAGS) -L$(KS_DIR)/lib -lks -lm


# -*- targets/source setup -*-

# the C files that make up the module
mod_src_c      := $(wildcard src/*.c)

# the output library file (this should begin with `libksm_` so KSM knows its a valid module file)
mod_LIB        := ./libksm_$(MOD_NAME).so


# -*- generated output -*-
mod_o          := $(patsubst %.c,%.o, $(mod_src_c))


# -*- compilation rules -*-

# rule to compile the module file, linking it with kscript
$(mod_LIB): $(mod_o)
	$(CC) $(mod_CFLAGS) -shared $^ $(mod_LDFLAGS) -o $@

# rule to compile a single object file, no need to link yet
%.o: %.c
	$(CC) $(mod_CFLAGS) -fPIC -c $< -o $@

# rule to restart the compilation, removing anything done so far
clean:
	rm -rf $(wildcard $(mod_LIB) $(mod_o))
