//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Tests unwinding where the signal handler is a VAPI function.
//
// `exit` is used as the signal handler and the unwinding is done in an atexit handler.
// `__builtin_debugtrap` is used because `raise` is also a VAPI function.

// REQUIRES: target={{.+}}-aix{{.*}}
// REQUIRES: has-filecheck

// ADDITIONAL_COMPILE_FLAGS: -fno-inline -fno-exceptions

// RUN: %{build}
// RUN: %{exec} %t.exe 2>&1 | FileCheck %s

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void my_atexit_handler(void) {
  fprintf(stderr, "Retrieve a cursor to `main` by stepping up.\n");
  // CHECK: Retrieve a cursor to `main`
  // TODO
  fprintf(stderr, "Resume `main` at the call to `trapper.\n");
  // CHECK: Resume `main`
  // TODO
}

void trapper(void) {
  __builtin_debugtrap();
  _Exit(EXIT_FAILURE);
}

int main(void) {
  if (atexit(my_atexit_handler) != 0) {
    perror("atexit");
    abort();
  }
  if (signal(SIGTRAP, exit) == SIG_ERR) {
    perror("signal");
    abort();
  }
  trapper();
  _Exit(0);
}
