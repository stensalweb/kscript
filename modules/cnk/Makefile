# cnk/Makefile - bindings for the Nuklear library makefile
# NOTE: Do not call this by itself, run from the main directory `make modules`

# -*- VARS -*-

MOD_NAME       := cnk


# -*- FILES -*-

mod_C          := $(wildcard src/*.c) $(wildcard src/ext/*.c)

mod_H          := 


# -*- TARGETS -*-

# main shared library
mod_SHARED     := libksm_$(MOD_NAME).$(SHARED_END)

# generated
mod_O          := $(patsubst %.c,%.o,$(mod_C))

# static libraries that were built
_include_static := $(wildcard $(patsubst %,$(KS_DIR)/deps/prefix/lib/lib%.$(STATIC_END),glfw3))

# libraries
mod_LIBS       := -Wl,-Bsymbolic -Wl,--whole-archive $(_include_static) -Wl,--no-whole-archive -lglfw -lX11


# -*- RULES -*-

# non-file targets
.PHONY: default clean install uninstall

default: $(mod_SHARED)

# remove all created files
clean:
	rm -rf $(wildcard $(mod_SHARED) $(mod_O))

# install all files
install: $(mod_SHARED) $(mod_H)
	install -d $(DESTDIR)$(PREFIX)/lib/kscript/modules/$(MOD_NAME)
	install -m 755 $(mod_SHARED) $(DESTDIR)$(PREFIX)/lib/kscript/modules/$(MOD_NAME)

# rule to uninstall the whole package from PREFIX
uninstall:
	rm -rf $(wildcard $(DESTDIR)$(PREFIX)/lib/kscript/modules/$(MOD_NAME)) 

# rule to compile a single object file, no need to link yet
%.o: %.c $(mod_H) | $(all_H)
	$(CC) $(CFLAGS) $< -fPIC -c -o $@

# rule to compile the module file, linking it with kscript
$(mod_SHARED): $(mod_O)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) $(libks_SHARED) -Wl,-rpath,'$$ORIGIN' $(mod_LIBS) -shared -o $@
	strip $@ $(STRIP_OPTS)
	@echo "\n|| Built: $@\n"
