# nx/Makefile - std module makefile
# NOTE: Do not call this by itself, run from the main directory `make modules`

# -*- VARS -*-

MOD_NAME        := nx


# -*- DEPENDS -*-

# any static libraries that were built
_include_static := -Wl,-Bsymbolic -Wl,--whole-archive $(wildcard $(patsubst %,$(KS_DIR)/deps/prefix/lib/lib%.$(STATIC_END),fftw3)) -Wl,--no-whole-archive

# C libraries
mod_LIBS        := $(_include_static)

# add optional dependencies
ifneq ($(findstring FFTW3,$(HAVE_LIBS)),)
	mod_LIBS += -lfftw3
endif


# other modules this one depends on
mod_MODDEPS     := 


# -*- FILES -*-

# C source files
mod_C           := $(wildcard src/*.c) $(wildcard ops/*.c) $(wildcard fft/*.c) $(wildcard la/*.c)

# C header files
mod_H           := nx.h nx-impl.h nx-fft.h nx-la.h


# -*- TARGETS -*-

# main shared library
mod_SHARED      := libksm_$(MOD_NAME).$(SHARED_END)

# generated objects
mod_O           := $(patsubst %.c,%.o,$(mod_C))

# installation directory
MOD_INSTALL_DIR := $(DESTDIR)$(PREFIX)/lib/kscript/modules/$(MOD_NAME)

# -*- RULES -*-

# non-file targets
.PHONY: default clean install uninstall

default: $(mod_SHARED)

# remove all created files
clean:
	rm -rf $(wildcard $(mod_SHARED) $(mod_O))

# install all files
install: $(mod_SHARED) $(mod_H)
	install -d $(MOD_INSTALL_DIR)
	install -m 755 $(mod_SHARED) $(MOD_INSTALL_DIR)
	install -m 644 $(mod_H) $(MOD_INSTALL_DIR)

# rule to compile a single object file, no need to link yet
%.o: %.c $(mod_H) | $(all_H)
	$(CC) $(CFLAGS) $(patsubst %,-I$(KS_DIR)/modules/%,$(mod_MODDEPS)) -I./ $< -fPIC -c -o $@

# rule to compile the module file, linking it with kscript
$(mod_SHARED): $(mod_O)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(libks_SHARED) -Wl,-rpath,'$$ORIGIN' $(mod_LIBS) -shared -o $@
	$(STRIP) $@ $(STRIP_OPTS)

