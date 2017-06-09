// Driver file for DynASM-based JITs.

#include <assert.h>
#include <stdio.h>
#include <sys/mman.h>

#include "dynasm/dasm_proto.h"
#include "dynasm/dasm_x86.h"

void initjit(dasm_State **state, const void *actionlist);
void *jitcode(dasm_State **state);
void free_jitcode(void *code);

void initjit(dasm_State **state, const void *actionlist) {
  dasm_init(state, 1);
  dasm_setup(state, actionlist);
}

void *jitcode(dasm_State **state) {
  size_t size;
  int dasm_status = dasm_link(state, &size);
  assert(dasm_status == DASM_S_OK);

  // Allocate memory readable and writable so we can
  // write the encoded instructions there.
  char *mem = mmap(NULL, size + sizeof(size_t),
		   PROT_READ | PROT_WRITE,
                   MAP_ANON | MAP_PRIVATE, -1, 0);
  assert(mem != MAP_FAILED);

  // Store length at the beginning of the region, so we
  // can free it without additional context.
  *(size_t*)mem = size;
  void *ret = mem + sizeof(size_t);

  dasm_encode(state, ret);
  dasm_free(state);

  // Adjust the memory permissions so it is executable
  // but no longer writable.
  int success = mprotect(mem, size, PROT_EXEC | PROT_READ);
  assert(success == 0);

#ifndef NDEBUG
  // Write generated machine code to a temporary file.
  // View with:
  //  objdump -D -b binary -mi386 -Mx86-64 /tmp/jitcode
  // Or:
  //  ndisasm -b 64 /tmp/jitcode
  FILE *f = fopen("/tmp/jitcode", "wb");
  fwrite(ret, size, 1, f);
  fclose(f);
#endif

  return ret;
}

void free_jitcode(void *code) {
  void *mem = (char*)code - sizeof(size_t);
  int status = munmap(mem, *(size_t*)mem);
  assert(status == 0);
}
