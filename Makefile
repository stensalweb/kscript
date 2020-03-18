# Makefile - describes building the ks library & executable
#
# Full makefile build:
# 1. run `make init`. This creates `include/ks_config.h`
# 2. edit `include/ks_config.h`, reading the comments to understand what each option does
# 3. run `make` to build the library, executable, etc into `lib/`, `bin/`
# 4. Run them
# 5. If desired, run `sudo make install` to install into `/usr/local/`.
#      To install into another directory, run `PREFIX=directory/to/install sudo make install`
#
# @author   : Cade Brown <brown.cade@gmail.com>
# @license  : WTFPL (http://www.wtfpl.net/)

# -*- CONFIGURATION

# set the C compiler (?= means 'set if not already set', even though 'CC' is a special case which is always set)
#CC         ?= cc
# set the compiler flags
CFLAGS     ?= -O3 -std=c99 
# set the installation prefix
PREFIX     ?= /usr/local

# which standard modules to build? default is all of them
KSM_STD    ?= m nx

# default to not changing the options
KS_OPTS    ?= NONE_NEW


# configuration file
KS_CONFIG   = src/ks-config.h

ifeq ($(wildcard $(KS_CONFIG)),)
    $(warning No kscript config found, copying the default one)
    $(shell ./gen_config_h.sh $(KS_OPTS) > $(KS_CONFIG))
endif

ifneq ($(KS_OPTS),NONE_NEW)
ifneq ($(shell head -n 1 $(KS_CONFIG)),"//$(KS_OPTS)")
    $(warning 'KS_OPTS' changed; recalculating header file)
    $(shell ./gen_config_h.sh $(KS_OPTS) > $(KS_CONFIG))
endif
endif


# -*- INPUT FILES

# the sources for our ks library (addprefix basically just adds `src`
#   to each of the files, since we are in `./` and they're in `./src`)
libks_src       := $(addprefix src/, init.c log.c mem.c util.c obj.c fmt.c funcs.c codegen.c exec.c getopt_long.c) \
				   $(addprefix src/types/, type.c none.c bool.c int.c float.c complex.c str.c tuple.c list.c dict.c Error.c cfunc.c kfunc.c pfunc.c code.c ast.c parser.c thread.c module.c)

# the header files that if changed, should cause recompilation
libks_src_h     := $(addprefix src/, ks.h ks-impl.h) $(KS_CONFIG)

# the sources for the ks executable (so things can be ran from 
#   commandline)
ks_src          := $(addprefix src/, ks.c )


# standard modules
ksm_std         := $(KSM_STD)

# now, generate a list of `.o` files needed
libks_o         := $(patsubst %.c,%.o, $(libks_src))
ks_o            := $(patsubst %.c,%.o, $(ks_src))
ksm_std_so      := $(patsubst %,modules/%/libksm_%.so, $(ksm_std))


# -*- OUTPUT FILES

# where to build the shared library to
libks_so  := ./lib/libks.so
# and static
libks_a   := ./lib/libks.a

# where to build the executable to
ks_exe    := ./bin/ks


$(info Building with modules: $(KSM_STD))


# -*- RULES

# these are rules that are `not real files`, but can be ran like `make clean`
.PHONY: default clean uninstall

# by default, build the `ec` binary
default: $(ks_exe) $(ksm_std_so) 

# using wildcard means it only removes what exists, which makes for more useful
#   messages
clean:
	rm -rf $(wildcard $(ks_o) $(libks_o) $(ks_exe) $(libks_so) $(libks_a))
	-for subdir in $(patsubst %,modules/%,$(ksm_std)); do \
		$(MAKE) -C $$subdir clean ; \
	done

# rule to build the object files (.o's) from a C file
# in makefile, `%` is like a wildcard, `%.c` will match `DIR/ANYTHING.c`
# `$<`: means the input file (%.c in this case)
# `$@`: means the output file (%.o in thie case)
%.o: %.c $(libks_src_h)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I./src -fPIC $< -c -o $@

# rule to build the shared object file (.so) from all the individual compilations
# Since `libks_o` contains many files, we use `$^` to mean `all input files together`
$(libks_so): $(libks_o)
	$(CC) $(CFLAGS) -shared $^ -o $@

# rule to build the static object file (.a)
$(libks_a): $(libks_o)
	$(AR) cr $@ $^

# rule to build the executable (no extension) from the library and it's `.o`'s
#   since we require a library, and object files, we don't use `$^`, but just build
#   explicitly
$(ks_exe): $(libks_so) $(ks_o) $(MOD_std_so)
	$(CC) $(CFLAGS) -Wl,-rpath=./lib/ -L./lib/ $(ks_o) -lks -lm -ldl -lpthread -o $@

# rule to build a standard module
modules/%/libksm_%.so:
	$(MAKE) -C $(dir $@)

# rule to install the whole package to PREFIX
install: $(libks_so) $(ks_exe) $(libks_src_h)
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include
	cp -rf $(ks_exe) $(DESTDIR)$(PREFIX)/bin
	cp -rf $(libks_so) $(DESTDIR)$(PREFIX)/lib
	cp -rf  $(libks_src_h) $(DESTDIR)$(PREFIX)/include
	@touch install

# rule to uninstall the whole package from PREFIX
uninstall:
	rm -f $(wildcard $(DESTDIR)$(PREFIX)/bin/$(notdir $(ks_exe)) $(DESTDIR)$(PREFIX)/lib/$(notdir $(libks_so)) $(addprefix $(DESTDIR)$(PREFIX)/include/,$(notdir $(libks_src_h))))

# rule to build a tarfile
ks.tar.gz: $(ks_exe) $(libks_so)
	tar -cvf $@ $(ks_exe) $(libks_so) $(libks_src_h)

