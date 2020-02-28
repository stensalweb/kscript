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

# set the C compiler (?= means 'set if not already set')
CC         ?= cc
# set the compiler flags
CFLAGS     ?= -O3 -std=c99 
# set the installation prefix
PREFIX     ?= /usr/local

# which standard modules to build? default is all of them
KSM_STD    ?= 


# -*- INPUT FILES

# the sources for our ks library (addprefix basically just adds `src`
#   to each of the files, since we are in `./` and they're in `./src`)
libks_types_src := $(addprefix src/types/, none.c bool.c int.c float.c str.c tuple.c list.c dict.c code.c kfunc.c type.c module.c parser.c ast.c cfunc.c pfunc.c kobj.c $(addprefix iter/, list.c dict.c) $(addprefix error/, error.c))
libks_src       := $(addprefix src/, mem.c log.c err.c kso.c fmt.c exec.c funcs.c codegen.c util.c $(addprefix opt/, propconst.c ) ) $(libks_types_src)
libks_src_h     := $(addprefix ./include/, ks_config.h ks_bytecode.h ks_common.h ks_funcs.h ks_module.h ks_types.h ks.h kso.h)

# the sources for the ks executable (so things can be ran from 
#   commandline)
ks_src          := $(addprefix src/, ks.c )


# standard modules
ksm_std         := $(KSM_STD)

# now, generate a list of `.o` files needed
libks_o         := $(patsubst %.c,%.o, $(libks_src))
ks_o            := $(patsubst %.c,%.o, $(ks_src))
ksm_std_so      := $(patsubst %,std/%/libksm_%.so, $(ksm_std))


# -*- OUTPUT FILES

# where to build the shared library to
libks_so  := ./lib/libks.so
# and static
libks_a   := ./lib/libks.a

# where to build the executable to
ks_exe    := ./bin/ks


# -*- RULES

# these are rules that are `not real files`, but can be ran like `make clean`
.PHONY: default init clean uninstall

# by default, build the `ec` binary
default: $(ks_exe) $(ksm_std_so)

# initializes the build process, cleaning, and then creating the configuration header
init: clean
	cp ks_config.T.h ./include/ks_config.h

# using wildcard means it only removes what exists, which makes for more useful
#   messages
clean:
	rm -rf $(wildcard $(ks_o) $(libks_o) $(ks_exe) $(libks_so) $(libks_a))
	-for subdir in $(patsubst %,std/%,$(ksm_std)); do \
		$(MAKE) -C $$subdir clean ; \
	done

# rule to build the object files (.o's) from a C file
# in makefile, `%` is like a wildcard, `%.c` will match `DIR/ANYTHING.c`
# `$<`: means the input file (%.c in this case)
# `$@`: means the output file (%.o in thie case)
%.o: %.c $(libks_src_h)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I./include -fPIC $< -c -o $@

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
$(ks_exe): $(ks_o) $(libks_so) $(MOD_std_so)
	$(CC) $(CFLAGS) -Wl,-rpath=./lib/ -L./lib/ $(ks_o) -lks -lm -ldl -o $@

# rule to build a standard module
std/%/libksm_%.so:
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

