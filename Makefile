
CFLAGS=-O3

all: jit1 jit2

jit1: jit1.c
jit2: dynasm-driver.c jit2.h
	$(CC) $(CFLAGS) $(CPPFLAGS) -o jit2 dynasm-driver.c -DJIT=\"jit2.h\"

jit2.h: jit2.dasc
	lua dynasm/dynasm.lua jit2.dasc > jit2.h
