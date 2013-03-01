
# X86 Architecture
ARCH_x86_64		= x86_64
CFLAGS_x86_64		= -DLINUX64 -DMACHINE=EM_X86_64
BFD_ARCH_x86_64		= bfd_arch_i386
BFD_MACH_x86_64		= bfd_mach_x86_64

ARCH_i386		= i386
CFLAGS_i386		= -DLINUX32 -DMACHINE=EM_386
BFD_ARCH_i386		= bfd_arch_i386
BFD_MACH_i386		= bfd_mach_i386_i386

# Powerpc Architecture
ARCH_powerpc		= powerpc
CFLAGS_powerpc		= -DLINUX32 -DMACHINE=EM_PPC
BFD_ARCH_powerpc	= bfd_arch_powerpc
BFD_MACH_powerpc	= bfd_mach_ppc

ARCH_powerpc64		= powerpc64 
CFLAGS_powerpc64	= -DLINUX64 -DMACHINE=EM_PPC64
BFD_ARCH_powerpc64	= bfd_arch_powerpc
BFD_MACH_powerpc64	= bfd_mach_ppc64

# MIPS Architecture
ARCH_mips		= mips
CFLAGS_mips		= -DLINUX32 -DMACHINE=EM_MIPS
BFD_ARCH_mips		= bfd_arch_mips
BFD_MACH_mips		= bfd_mach_r6000

ARCH_mips64		= mips64 
CFLAGS_mips64		= -DLINUX64 -DMACHINE=EM_MIPS
BFD_ARCH_mips64		= bfd_arch_mips
BFD_MACH_mips64		= bfd_mach_r4000

# ARM Architecture
ARCH_arm		= arm
CFLAGS_arm		= -DLINUX32 -DMACHINE=EM_ARM
BFD_ARCH_arm		= bfd_arch_arm
BFD_MACH_arm		= bfd_mach_arm_unknown

ARCH_arm64		= arm
CFLAGS_arm64		= -DLINUX64 -DMACHINE=EM_ARM
BFD_ARCH_arm64		= bfd_arch_arm
BFD_MACH_arm64		= bfd_mach_arm_unknown

