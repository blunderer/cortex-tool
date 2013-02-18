
#ifndef _CORTEX_ELF_H_
#define _CORTEX_ELF_H_

/** \file cortex_elf.h
 * \brief cortex elf format common definition
 * \author Tristan Lelong <tristan.lelong@blunderer.org>
 * \date 2011 06 27
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <elf.h>
#include <sys/procfs.h>
#include <asm/ptrace.h>

/** \brief defines depending on cpu nr bits */
#if defined(LINUX32)
	/* select arch: 32 bit */
#define ElfN_Ehdr       Elf32_Ehdr
#define ElfN_Phdr       Elf32_Phdr
#define ElfN_Nhdr       Elf32_Nhdr
#define ElfN_Shdr       Elf32_Shdr
#define ElfN_Addr	Elf32_Addr
#define ElfN_Off 	Elf32_Off
#define ELFCLASSN	ELFCLASS32

#define elfN_getehdr	elf32_getehdr
#define elfN_getphdr	elf32_getphdr
#define elfN_getshdr	elf32_getshdr

#define ElfN_auxv_t	Elf32_auxv_t
#elif defined(LINUX64)
	/* select arch: 64 bit */
#define ElfN_Ehdr       Elf64_Ehdr
#define ElfN_Phdr       Elf64_Phdr
#define ElfN_Nhdr       Elf64_Nhdr
#define ElfN_Shdr       Elf64_Shdr
#define ElfN_Addr	Elf64_Addr
#define ElfN_Off 	Elf64_Off
#define ELFCLASSN	ELFCLASS64

#define elfN_getehdr	elf64_getehdr
#define elfN_getphdr	elf64_getphdr
#define elfN_getshdr	elf64_getshdr

#define ElfN_auxv_t	Elf64_auxv_t
#else
	/* default: */
#error "please specify 32b or 64b target using MODE=64 or MODE=32"
#endif

struct cortex_elf {
	int fd;
	unsigned long offset;

	ElfN_Ehdr *ehdr;
	ElfN_Phdr *phdr;
};

struct cortex_elf_data {
	size_t d_size;
	ElfN_Addr d_align;
	unsigned char *d_buf;
};

struct cortex_elf *cortex_elf_load_core(int elf_core_fd);
ElfN_Ehdr *cortex_elf_load_ehdr(struct cortex_elf *core);

struct cortex_proc_info *cortex_elf_parse(struct cortex_elf *core,
					  ElfN_Ehdr * ehdr);

void cortex_elf_cleanup_process_info(struct cortex_proc_info *info);
void cortex_elf_release_core(struct cortex_elf *core);

#endif /* _CORTEX_ELF_H_ */
