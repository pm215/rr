/* -*- Mode: C; tab-width: 8; c-basic-offset: 2; indent-tabs-mode: nil; -*- */

#include "rrutil.h"

static void breakpoint(void) {}

int main(int argc, char* argv[]) {
  pid_t child;
  int status;
  char* p;

  p = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS,
           -1, 0);
  test_assert(p != MAP_FAILED);

  *p = 'a';

  if ((child = fork()) == 0) {
    while (*(char*)p == 'a') {
      sched_yield();
    }
    return 0;
  }

  test_assert(0 == mprotect(p, PAGE_SIZE, PROT_READ));

  breakpoint();

  test_assert(0 == mprotect(p, PAGE_SIZE, PROT_READ | PROT_WRITE));

  *p = *p + 1;

  test_assert(*p == 'b');
  test_assert(child == waitpid(child, &status, 0));
  test_assert(0 == status);

  atomic_puts("EXIT-SUCCESS");

  return 0;
}
