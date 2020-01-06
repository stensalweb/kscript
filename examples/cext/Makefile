# cext/Makefile - example makefile for create a C extension

cexec_src_c := $(addprefix ./,cexec.c)
cexec_SO    := ./libksm_cexec.so

# generated
cexec_o     := $(patsubst %.c,%.o, $(cexec_src_c))


$(cexec_SO): $(cexec_o)
	$(CC) $(CFLAGS) -shared $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -I../../src -fPIC $< -c -o $@


