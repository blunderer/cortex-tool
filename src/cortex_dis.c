/** \file cortex_dis.c
 * \brief User OOPS disassembly functions
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

#ifdef HAVE_DIS_ASM
#include <dis-asm.h>

#include <stdarg.h>

#include "cortex.h"

static int cortex_dis_print_mute = 0;
static int cortex_dis_print_offset = 0;

static int (*print_insn_func) (bfd_vma, struct disassemble_info *) = NULL;

static void cortex_dis_fprintf_reset(void)
{
	cortex_dis_print_offset = 0;
}

static int cortex_dis_fprintf(void *stream, const char *format, ...)
{
	int ret = 0;

	if (!cortex_dis_print_mute) {
		va_list args;
		char *buffer = (char *)(stream + cortex_dis_print_offset);

		va_start(args, format);

		ret = vsprintf(buffer, format, args);
		cortex_dis_print_offset += ret;

		va_end(args);
	}

	return ret;
}

static void cortex_dis_set_arch(void)
{
	switch (BFD_ARCH) {
	case bfd_arch_i386:
		switch (BFD_MACH) {
		case bfd_mach_x86_64:
		case bfd_mach_i386_i386:
		default:
			print_insn_func = &print_insn_i386;
			break;
		}
		break;
	case bfd_arch_powerpc:
		switch (BFD_MACH) {
		case bfd_mach_ppc_e500:
		default:
			print_insn_func = &print_insn_big_powerpc;
			break;
		}
		break;
	case bfd_arch_mips:
		break;
	case bfd_arch_arm:
		switch (BFD_MACH) {
		case bfd_mach_arm_unknown:
		default:
			print_insn_func = &print_insn_little_arm;
			break;
		}
		break;
	case bfd_arch_unknown:
	default:
		break;

	}
}

void cortex_dis_process_buffer(FILE * output, unsigned char *buffer,
			       unsigned long len, unsigned long instr_context,
			       ElfN_Addr base, ElfN_Addr pc)
{
	unsigned long instr_context_start = (pc - base) - instr_context;
	unsigned long instr_context_end = 0;
	unsigned long instr_ptr = 0;
	char instruction_buffer[256] = "\0";
	disassemble_info disinfo;

	/* internal: set arch for disassembly ouput */
	cortex_dis_set_arch();

	/* we got to init the disassemble_info struct with machine
	 * special arch and mach: ouput will be done to buffer */
	init_disassemble_info(&disinfo, instruction_buffer,
			      (fprintf_ftype) cortex_dis_fprintf);

	/* mach info after init even if advised not to
	 * because init overrides mach value */
	disinfo.arch = BFD_ARCH;
	disinfo.mach = BFD_MACH;

	disinfo.buffer = buffer;
	disinfo.buffer_length = len;

	cortex_dis_print_mute = 1;

	/* iterate thru all instruction and display them */
	while (instr_ptr < len) {
		int size = 0;

		/* only display instructions around the current instruction pointer */
		if (!cortex_dis_print_mute) {
			if (instr_ptr > instr_context_end) {
				cortex_dis_print_mute = 1;
			}
		} else if (!instr_context_end) {
			if (instr_ptr >= instr_context_start) {
				instr_context_end =
				    (pc - base) + instr_context + 1;
				cortex_dis_print_mute = 0;
			}
		}

		if (print_insn_func) {
			cortex_dis_fprintf_reset();
			size = print_insn_func(instr_ptr, &disinfo);
		} else {
			size = 4;
		}

		if (!cortex_dis_print_mute) {
			int j = 0;

			/* display format is: <addr>: <hexcode> <asm> */
			fprintf(output, "  0x%08X: \t", base + instr_ptr);
			for (j = 0; j < size; j++) {
				fprintf(output, "%02x ", buffer[instr_ptr + j]);
			}
			for (j = size; j < 12; j++) {
				fprintf(output, "   ");	/* alignment for non fixed size opcodes */
			}

			if (base + instr_ptr == pc) {
				fprintf(output, "| => %s\n",
					instruction_buffer);
			} else {
				fprintf(output, "|    %s\n",
					instruction_buffer);
			}
		}

		instr_ptr += size;
	}
}
#else /* HAVE_DIS_ASM */
#include <stdio.h>
#include "cortex.h"

void cortex_dis_process_buffer(FILE * output, unsigned char *buffer,
			       unsigned long len, unsigned long instr_context,
			       ElfN_Addr base, ElfN_Addr pc)
{
	fprintf(output, "Unsupported\n");
}
#endif /* HAVE_DIS_ASM */
