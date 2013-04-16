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
AC_CHECK_HEADERS([dis-asm.h], [libopcodes=-lopcodes], [AC_MSG_WARN(["missing binutils development files: disable disassemble support."], [1])])

# customize system type
AC_ARG_VAR([BFD_MACH], [bfd_mach used when disassembling code. See bfd.h for a list. (guessed if empty)])
AC_ARG_VAR([BFD_ARCH], [bfd_arch used when disassembling code. See bfd.h for a list. (guessed if empty)])
AC_ARG_VAR([CFLAGS_ARCH], [cflags used by cortex. -DLINUX32 or -DLINUX64, and -DMACHINE=EM_xxx. (guessed if empty)])

AC_CANONICAL_HOST
AC_CANONICAL_BUILD
AC_CANONICAL_TARGET
AS_CASE([$host],
		[alpha*-*],		[cpu_arch=alpha],
		[arm*-*],		[cpu_arch=arm],
		[avr-*],		[cpu_arch=avr32],
		[bfin*-*],		[cpu_arch=blackfin],
		[h8300-*],		[cpu_arch=h8300],
		[i?86-*],		[cpu_arch=i386],
		[ia64*-*],		[cpu_arch=ia64],
		[m32r*-*],		[cpu_arch=m32r],
		[m68ki*-*],		[cpu_arch=m68k],
		[microblaze*-*],	[cpu_arch=microblaze],
		[mips*-*],		[cpu_arch=mips],
		[powerpc*-*],		[cpu_arch=powerpc],
		[score*-*],		[cpu_arch=score],
		[sparc*-*],		[cpu_arch=sparc],
		[tic6x-*],		[cpu_arch=c6x],
		[tile*-*],		[cpu_arch=tile],
		[x86_64-*],		[cpu_arch=x86_64],
		[xtensa*-*],		[cpu_arch=xtensa],
		[cpu_arch=unknown]
)

AC_SUBST([libopcodes])
AC_SUBST([cpu_arch])

# Build output
AC_CONFIG_FILES([Makefile])
AC_OUTPUT

