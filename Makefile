
CFLAGS=-O3 -g -std=gnu99 -Ithird_party -D DASM_VERSION=10300

all: jit1 jit2 jit3

jit1: jit1.c

jit2: dynasm-driver.h jit2.c
	$(CC) $(CFLAGS) $(CPPFLAGS) dynasm-driver.h jit2.c -o jit2
jit2.c: jit2.dasc
	lua third_party/dynasm/dynasm.lua jit2.dasc > jit2.c

jit3: dynasm-driver.h jit3.c
	$(CC) $(CFLAGS) $(CPPFLAGS) dynasm-driver.h jit3.c -o jit3
jit3.c: jit3.dasc
	lua third_party/dynasm/dynasm.lua jit3.dasc > jit3.c

clean:
	rm -f jit1 jit2 jit3
