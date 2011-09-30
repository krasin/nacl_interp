/*
 * Copyright (c) 2011 The Native Client Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

/*
 * This program provides a hack for running dynamically-linked NaCl
 * executables directly on a Linux host.  This standalone program
 * must be compiled with -static and with -Wl,-Ttext-segment=...
 * to place it at some address where no nexe will lie (1G is a good choice).
 * Then a symlink to this executable must be installed in the location
 * that appears in the PT_INTERP of a nexe for the platform in question.
 * That is, the appropriate one of:
 *      /lib/ld-nacl-x86-32.so.1
 *      /lib64/ld-nacl-x86-64.so.1
 *      /lib/ld-nacl-arm.so.1
 *
 * Thereafter, running a nexe will actually run this program, which will do:
 *      exec ${NACL_INTERP_LOADER} PLATFORM NEXE ARGS...
 * That is, NACL_INTERP_LOADER should be set in the environment to an
 * appropriate wrapper script that runs the right sel_ldr with the
 * right -B .../irt_core.nexe switch (and other appropriate switches,
 * usually -a among thme).
 *
 * PLATFORM will be x86_64, i[3456]86, etc. as seen in AT_PLATFORM.
 * If you aren't sure what your Linux system produces, try running:
 *      LD_SHOW_AUXV=1 /bin/true | fgrep AT_PLATFORM
 *
 * NEXE is the name of the original executable, and ARGS... are its
 * arguments (the first being its argv[0], i.e. program name).
 *
 * The wrapper script can use the PLATFORM argument to select the
 * appropriate sel_ldr et al to use.
 */

#include <elf.h>
#include <errno.h>
#include <error.h>
#include <linux/limits.h>
#include <link.h>
#include <locale.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ENVAR "NACL_INTERP_LOADER"

int main(int argc, char **argv, char **envp) {
  char *execfn = NULL;
  char *platform = NULL;
  bool secure = true;

  /*
   * Recover auxv.
   */
  char **ep = envp;
  while (*ep != NULL)
    ++ep;

  for (const ElfW(auxv_t) *av = (void *) (ep + 1); av->a_type != AT_NULL; ++av)
    switch (av->a_type) {
      case AT_EXECFN:
        execfn = (char *) av->a_un.a_val;
        break;
      case AT_PLATFORM:
        platform = (char *) av->a_un.a_val;
        break;
      case AT_SECURE:
        secure = av->a_un.a_val != 0;
        break;
    }

  if (execfn == NULL) {
    static char buf[PATH_MAX + 1];
    ssize_t n = readlink("/proc/self/exe", buf, sizeof buf - 1);
    if (n >= 0) {
      buf[n] = '\0';
      execfn = buf;
    } else {
      execfn = argv[0];
    }
  }

  if (secure)
    error(127, 0, "refusing secure exec of %s", execfn);

  if (platform == NULL) {
#if defined(__x86_64__)
    platform = "x86_64";
#elif defined(__i386__)
    platform = "i386";
#elif defined(__arm__)
    platform = "arm";
#else
# error "Don't know default platform!"
#endif
  }

  {
    char *loader = getenv(ENVAR);
    char *new_argv[argc + 4];

    if (loader == NULL)
      error(127, 0,
            "environment variable %s must be set to run a NaCl binary directly",
            ENVAR);

    new_argv[0] = loader;
    new_argv[1] = platform;
    new_argv[2] = execfn;
    memcpy(&new_argv[2], argv, (argc + 1) * sizeof(new_argv[0]));

    execve(loader, new_argv, envp);

    /*
     * With Ubuntu's libc6-dev-2.11.1-0ubuntu, a statically linked program
     * crashes in strerror.  So don't try to translate the errno code.
     */
    error(127, 0, "failed to execute %s (errno=%d)", loader, errno);
  }

  /*NOTREACHED*/
  return 127;
}
