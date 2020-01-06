# cext/Makefile - example makefile for create a C extension

# -*- configuration

# the kscript installation directory. Use `../../` to use the local one,
#   or `/usr/local` (or another path that it was installed to) for the system-wide one
KS_DIR         ?= ../..
#KS_DIR         ?= /usr/local

# CFLAGS for compiling this module
cexec_CFLAGS   ?= -I$(KS_DIR)/include

# linker flags for compiling this module
cexec_LDFLAGS  ?= -L$(KS_DIR)/lib


# -*- targets/source setup

# the C files that make up the module
cexec_src_c    := ./cexec.c

# the output library file (this should begin with `libksm_` so KSM knows its a valid module file)
cexec_LIB      := ./libksm_cexec.so


# -*- generated output
cexec_o     := $(patsubst %.c,%.o, $(cexec_src_c))



# -*- compilation rules

# rule to compile the module file
$(cexec_LIB): $(cexec_o)
	$(CC) $(CFLAGS) $(cexec_CFLAGS) -shared $^ $(cexec_LDFLAGS) -lkscript -o $@

# rule to compile a single object file
%.o: %.c
	$(CC) $(CFLAGS) $(cexec_CFLAGS) -fPIC $< -c -o $@

# rule to restart the compilation, removing anything done so far
clean:
	rm $(wildcard $(cexec_LIB) $(cexec_o))
