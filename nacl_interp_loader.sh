#!/bin/sh
# Copyright (c) 2011 The Native Client Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Usage: nacl_interp_loader.sh PLATFORM NEXE ARGS...

# Assumes this file is sitting in the source tree.
# This should be changed for some proper SDK installation setup.
NACL_DIR=`dirname "$0"`/../../../..

case "$1" in
i?86)
  arch=x86-32
  libdir=lib32
  ;;
x86_64)
  arch=x86-64
  libdir=lib64
  ;;
arm|v7l)
  arch=arm
  libdir=lib32
  ;;
*)
  echo >&2 "$0: Do not recognize architecture \"$1\""
  exit 127
  ;;
esac

shift

SEL_LDR="$NACL_DIR/scons-out/opt-linux-${arch}/staging/sel_ldr"
IRT="$NACL_DIR/scons-out/nacl_irt-${arch}/staging/irt_core.nexe"
RTLD="$NACL_DIR/toolchain/linux_x86/x86_64-nacl/${libdir}/runnable-ld.so"
LIBDIR="$NACL_DIR/toolchain/linux_x86/x86_64-nacl/${libdir}"

exec "$SEL_LDR" -a -S -B "$IRT" -- \
  "$RTLD" --library-path $LIBDIR "$@"
