# m/Makefile - std module makefile
# NOTE: Do not call this by itself, run from the main directory `make modules`

# -*- VARS -*-

MOD_NAME        := m


# -*- DEPENDS -*-

# any static libraries that were built
_include_static :=

# C libraries
mod_LIBS        := -lm

# other modules this one depends on
mod_MODDEPS     := 


# -*- FILES -*-

# C source files
mod_C           := $(wildcard src/*.c)

# C header files
mod_H           :=


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
	#install -m 644 $(mod_H) $(MOD_INSTALL_DIR)

# rule to compile a single object file, no need to link yet
%.o: %.c $(mod_H) | $(all_H)
	$(CC) $(CFLAGS) $(patsubst %,-I$(KS_DIR)/modules/%,$(mod_MODDEPS)) $< -fPIC -c -o $@

# rule to compile the module file, linking it with kscript
$(mod_SHARED): $(mod_O)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(libks_SHARED) -Wl,-rpath,'$$ORIGIN' $(mod_LIBS) -shared -o $@
	$(STRIP) $@ $(STRIP_OPTS)

