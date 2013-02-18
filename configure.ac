# Process this file with autoconf to produce a configure script.
AC_PREREQ([2.61])
AC_INIT([cortex], [0.1], [tristan.lelong@blunderer.org])
AC_CONFIG_SRCDIR([src/])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC
AC_CHECK_TOOL(MKDIR, mkdir)
AC_CHECK_TOOL(INSTALL, install)

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h sys/procfs.h asm/ptrace.h sys/stat.h], [], [AC_MSG_ERROR(["some header are missing."], [1])])
AC_CHECK_HEADERS([elf.h], [], [AC_MSG_ERROR(["missing libc development files."], [1])])
AC_CHECK_HEADERS([dis-asm.h], [], [AC_MSG_ERROR(["missing binutils development files."], [1])])

# set system type
AC_ARG_VAR([BFD_MACH], [bfd_mach used when disassembling code. See bfd.h for a list. (guessed if empty)])
AC_ARG_VAR([BFD_ARCH], [bfd_arch used when disassembling code. See bfd.h for a list. (guessed if empty)])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
AS_CASE([$host],
		[x86_64-*],	[cpu_arch=x86_64],
		[i?86-*],	[cpu_arch=i386],
		[arm*-*],	[cpu_arch=arm],
		[ppc-*],	[cpu_arch=powerpc],
		[ppc64-*],	[cpu_arch=powerpc64],
		[powerpc-*],	[cpu_arch=powerpc],
		[powerpc64-*],	[cpu_arch=powerpc64],
		[cpu_arch=unknown]
)
AC_SUBST([cpu_arch])

# Build output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

