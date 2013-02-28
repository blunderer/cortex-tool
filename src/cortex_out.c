/** \file cortex_out.c
 * \brief cortex out function
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

#include <stdio.h>
#include <string.h>

#include "cortex.h"
#include "cortex_elf.h"
#include "cortex_dis.h"
#include "arch/cortex_arch.h"

static long cortex_output_fmt = 0;

static char *auxv_names[] = {
	"AT_NULL", "AT_IGNORE", "AT_EXECFD", "AT_PHDR", "AT_PHENT",
	"AT_PHNUM", "AT_PAGESZ", "AT_BASE", "AT_FLAGS", "AT_ENTRY",
	"AT_NOTELF", "AT_UID", "AT_EUID", "AT_GID", "AT_EGID",
	"AT_PLATFORM", "AT_HWCAP", "AT_CLKTCK", "AT_FPUCW", "AT_DCACHEBSIZE",
	"AT_ICACHEBSIZE", "AT_UCACHEBSIZE", "AT_IGNOREPPC", "AT_SECURE",
	"AT_BASE_PLATFORM",
	"AT_RANDOM", "", "", "", "",
	"", "AT_EXECFN", "AT_SYSINFO", "AT_SYSINFO_EHDR", "AT_L1I_CACHESHAPE",
	"AT_L1D_CACHESHAPE", "AT_L2_CACHESHAPE", "AT_L3_CACHESHAPE",
};

static void cortex_output_write_generic(struct cortex_proc_info *info,
					FILE * output)
{
	fprintf(output, "BUG: process %s<%d> ", info->info->pr_fname,
		info->info->pr_pid);

	if (info->threads[0]->pr_cursig) {
		fprintf(output, "received signum %d in thread %d\n",
			info->threads[0]->pr_cursig, info->threads[0]->pr_pid);
	} else {
		fprintf(output, "crashed\n");
	}

	fprintf(output, "  cmdline was %s\n", info->info->pr_psargs);

	fprintf(output, "  uid/gid: %d/%d\n", info->info->pr_uid,
		info->info->pr_gid);

	fprintf(output, "  utime/stime: %u.%u/%u.%u\n",
		info->threads[0]->pr_utime.tv_sec,
		info->threads[0]->pr_utime.tv_usec,
		info->threads[0]->pr_stime.tv_sec,
		info->threads[0]->pr_stime.tv_usec);

	fprintf(output, "  cutime/cstime: %u.%u/%u.%u\n",
		info->threads[0]->pr_cutime.tv_sec,
		info->threads[0]->pr_cutime.tv_usec,
		info->threads[0]->pr_cstime.tv_sec,
		info->threads[0]->pr_cstime.tv_usec);

	fprintf(output, "  state: %c\n", "RSDTZW"[info->info->pr_state]);
	fprintf(output, "  nr threads: %d\n", info->nr_threads);
}

static void cortex_output_write_registers(struct cortex_proc_info *info,
					  FILE * output)
{
	int i = 0;

	for (i = 0; i < info->cpu_regs_nr; i++) {
		if ((i % 4) == 0)
			fprintf(output, "  ");

		if (info->cpu_regs[i].size == 4) {
			fprintf(output, "%s:0x%08X  ", info->cpu_regs[i].name,
				info->cpu_regs[i].value);
		} else if (info->cpu_regs[i].size == 8) {
			fprintf(output, "%s:0x%016X  ", info->cpu_regs[i].name,
				info->cpu_regs[i].value);
		}

		if ((i % 4) == 3)
			fprintf(output, "\n");
	}

	if (i % 4)
		fprintf(output, "\n");
}

static void cortex_output_write_stack_frame(struct cortex_proc_info *info,
					    FILE * output)
{
	ElfN_Addr i;
	void *priv_data = NULL;
	unsigned char *stackframe = NULL;
	struct cortex_stack_frame frame = CORTEX_EMPTY_FRAME;

	if (info->stack) {
		fprintf(output, "Last stack frame:\n");
	} else {
		fprintf(output, "Last stack frame: unavailable\n");
		return;
	}

	stackframe = info->stack->d_buf;

	if (cortex_arch_ops.unwind_init)
		priv_data = cortex_arch_ops.unwind_init(info, &frame);

	if (frame.bp == 0)
		fprintf(output, "  <empty>\n");
	for (i = frame.bp; i >= frame.sp; i -= info->word_size) {
		ElfN_Addr *stack_val =
		    (ElfN_Addr *) (stackframe + i - info->sp_segm->p_vaddr);
		if (info->word_size == 4) {
			fprintf(output, "  0x%08x: %08x\n", i, *stack_val);
		} else if (info->word_size == 8) {
			fprintf(output, "  0x%016x: %016x\n", i, *stack_val);
		}
	}

	if (cortex_arch_ops.unwind_exit)
		cortex_arch_ops.unwind_exit(info, priv_data);

	return;
}

static void cortex_output_write_call_trace(struct cortex_proc_info *info,
					   FILE * output)
{
	void *priv_data = NULL;
	struct cortex_stack_frame frame = CORTEX_EMPTY_FRAME;

	if (info->stack) {
		fprintf(output, "Call trace:\n");
	} else {
		fprintf(output, "Call trace: unavailable\n");
		return;
	}

	if (cortex_arch_ops.unwind_init) {
		priv_data = cortex_arch_ops.unwind_init(info, &frame);
	} else {
		fprintf(output, "Unsupported\n");
		return;
	}

	if (cortex_arch_ops.unwind_next) {
		do {
			int next;
			int frame_id = 0;

			if (info->word_size == 4) {
				fprintf(output, "  #%d at 0x%08x", frame_id,
					frame.pc);
			} else if (info->word_size == 8) {
				fprintf(output, "  #%d at 0x%016x", frame_id,
					frame.pc);
			}

			next =
			    cortex_arch_ops.unwind_next(info, &frame,
							 priv_data);

			if (!next) {
				if (info->threads[0]->pr_pid !=
				    info->threads[0]->pr_pgrp)
					fprintf(output, " in <clone>\n");
				else
					fprintf(output, " in <main>\n");
				break;
			}
			fprintf(output, "\n");
			frame_id++;
		} while (1);
	}

	if (cortex_arch_ops.unwind_exit)
		cortex_arch_ops.unwind_exit(info, priv_data);

	return;
}

static void cortex_output_write_source_code(struct cortex_proc_info *info,
					    FILE * output, int ctx)
{
	if (info->code) {
		fprintf(output, "Code:\n");
		cortex_dis_process_buffer(output, info->code->d_buf,
					  info->code->d_size, ctx,
					  info->pc_segm->p_vaddr, info->pc);
	} else {
		fprintf(output, "Code unavailable\n");
	}
}

static void cortex_output_write_auxv(struct cortex_proc_info *info,
				     FILE * output)
{
	ElfN_auxv_t *auxv = info->auxv;
	fprintf(output, "Auxiliary vector:\n");

	while (auxv->a_type != AT_NULL) {
		fprintf(output, "  %s = 0x%x (%d)\n", auxv_names[auxv->a_type],
			auxv->a_un.a_val, auxv->a_un.a_val);
		auxv++;
	}
}

static void cortex_output_write_elf_core(struct cortex_proc_info *info,
					 FILE * output)
{
	long cursor = 0;
	char padding[16];
	long align_phdr[3] = { 0, 0, 0 };
	long align = info->word_size;

	long stack_offset = 0;

	ElfN_Ehdr ehdr;
	ElfN_Phdr phdr[3];

	memset(padding, 0, 16);

	/* prepare elf core header */
	memcpy(&ehdr, info->elf->ehdr, sizeof(ElfN_Ehdr));

	ehdr.e_phoff = sizeof(ElfN_Ehdr);
	ehdr.e_phentsize = sizeof(ElfN_Phdr);
	ehdr.e_phnum = 0;
	ehdr.e_shnum = 0;

	if (cortex_output_fmt &
	    (CORTEX_OUTPUT_FMT_GEN | CORTEX_OUTPUT_FMT_REG |
	     CORTEX_OUTPUT_FMT_AUX))
		ehdr.e_phnum++;
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_COD)
		ehdr.e_phnum++;
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_STA)
		ehdr.e_phnum++;

	/* write the ELF core header */
	fwrite(&ehdr, sizeof(ElfN_Ehdr), 1, output);

	cursor = sizeof(ElfN_Ehdr) + ehdr.e_phnum * sizeof(ElfN_Phdr);

	/* prepare elf core note segment */
	if (cortex_output_fmt &
	    (CORTEX_OUTPUT_FMT_GEN | CORTEX_OUTPUT_FMT_REG |
	     CORTEX_OUTPUT_FMT_AUX)) {
		memcpy(&phdr[0], info->note_segm, sizeof(ElfN_Phdr));

		if (phdr[0].p_align > 1) {
			align_phdr[0] =
			    (phdr[0].p_align -
			     cursor % phdr[0].p_align) % phdr[0].p_align;
		} else {
			align_phdr[0] = (align - cursor % align) % align;
		}
		phdr[0].p_offset = cursor + align_phdr[0];

		cursor += align_phdr[0] + phdr[0].p_filesz;
	}
	/* prepare elf core code segment */
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_COD) {
		memcpy(&phdr[1], info->pc_segm, sizeof(ElfN_Phdr));
		phdr[1].p_filesz = phdr[1].p_memsz;

		if (phdr[1].p_align > 1) {
			align_phdr[1] =
			    (phdr[1].p_align -
			     cursor % phdr[1].p_align) % phdr[1].p_align;
		} else {
			align_phdr[1] = (align - cursor % align) % align;
		}
		phdr[1].p_offset = cursor + align_phdr[1];

		cursor += align_phdr[1] + phdr[1].p_filesz;
	}
	/* prepare elf core stack segment */
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_STA) {
		unsigned long stack_align = 0;
		unsigned long long stack_reduced_size =
		    info->word_size + info->sp_segm->p_vaddr +
		    info->sp_segm->p_filesz - info->sp;

		memcpy(&phdr[2], info->sp_segm, sizeof(ElfN_Phdr));
		if (phdr[2].p_align > 1) {
			align_phdr[2] =
			    (phdr[2].p_align -
			     cursor % phdr[2].p_align) % phdr[2].p_align;
			stack_align =
			    (phdr[2].p_align -
			     stack_reduced_size % phdr[2].p_align) %
			    phdr[2].p_align;
		} else {
			align_phdr[2] = (align - cursor % align) % align;
			stack_align =
			    (align - stack_reduced_size % align) % align;
		}

		stack_reduced_size += stack_align;
		stack_offset = phdr[2].p_memsz - stack_reduced_size;

		phdr[2].p_filesz = phdr[2].p_memsz = stack_reduced_size;
		phdr[2].p_vaddr = info->sp - info->word_size - stack_align;
		phdr[2].p_offset = cursor + align_phdr[2];

		cursor += align_phdr[2] + phdr[2].p_filesz;
	}

	if (cortex_output_fmt &
	    (CORTEX_OUTPUT_FMT_GEN | CORTEX_OUTPUT_FMT_REG |
	     CORTEX_OUTPUT_FMT_AUX))
		fwrite(phdr + 0, sizeof(ElfN_Phdr), 1, output);
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_COD)
		fwrite(phdr + 1, sizeof(ElfN_Phdr), 1, output);
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_STA)
		fwrite(phdr + 2, sizeof(ElfN_Phdr), 1, output);

	/* write elf core note segment */
	if (cortex_output_fmt &
	    (CORTEX_OUTPUT_FMT_GEN | CORTEX_OUTPUT_FMT_REG |
	     CORTEX_OUTPUT_FMT_AUX)) {
		if (align_phdr[0])
			fwrite(padding, align_phdr[0], 1, output);
		fwrite(info->note->d_buf, 1, info->note->d_size, output);
	}
	/* write elf core code segment */
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_COD) {
		if (align_phdr[1])
			fwrite(padding, align_phdr[1], 1, output);
		fwrite(info->code->d_buf, 1, info->code->d_size, output);
	}
	/* write elf core stack segment */
	if (cortex_output_fmt & CORTEX_OUTPUT_FMT_STA) {
		if (align_phdr[2])
			fwrite(padding, align_phdr[2], 1, output);
		fwrite(info->stack->d_buf + stack_offset, 1,
		       info->stack->d_size - stack_offset, output);
	}
}

int cortex_output_set_format(char *fmt)
{
	long fmt_len = 0;

	if (fmt == NULL) {
		cortex_output_fmt = CORTEX_OUTPUT_FMT_DEF;
		return 0;
	}

	fmt_len = strlen(fmt);

	while (*fmt) {
		if (fmt_len < 3) {
			cortex_output_fmt = 0;
			fprintf(stderr, "truncated format string %s\n", fmt);
			return -1;
		}

		if (strncmp(fmt, "gen", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_GEN;
		} else if (strncmp(fmt, "reg", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_REG;
		} else if (strncmp(fmt, "cod", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_COD;
		} else if (strncmp(fmt, "cal", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_CAL;
		} else if (strncmp(fmt, "aux", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_AUX;
		} else if (strncmp(fmt, "sta", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_STA;
		} else if (strncmp(fmt, "def", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_DEF;
		} else if (strncmp(fmt, "all", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_ALL;
		} else if (strncmp(fmt, "bin", 3) == 0) {
			cortex_output_fmt |= CORTEX_OUTPUT_FMT_BIN;
		} else if (strncmp(fmt, "bin", 3) == 0) {
			cortex_output_fmt &= ~CORTEX_OUTPUT_FMT_BIN;
		} else {
			fprintf(stderr, "unknown fmt %s\n", fmt);
			return -1;
		}

		fmt += 3;
		fmt_len -= 3;

		if (fmt_len && *fmt == ',') {
			fmt++;
			fmt_len--;
		}
	}

	return 0;
}

void cortex_output_write_process(struct cortex_proc_info *info, FILE * output,
				 int ctx)
{
	if ((cortex_output_fmt & CORTEX_OUTPUT_FMT_BIN) == 0) {
		fprintf(output,
			"\n8<--------------------------------------------------------------------------\n");

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_GEN)
			cortex_output_write_generic(info, output);

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_REG)
			cortex_output_write_registers(info, output);

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_COD)
			cortex_output_write_source_code(info, output, ctx);

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_CAL)
			cortex_output_write_call_trace(info, output);

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_AUX)
			cortex_output_write_auxv(info, output);

		if (cortex_output_fmt & CORTEX_OUTPUT_FMT_STA)
			cortex_output_write_stack_frame(info, output);

		fprintf(output, "\n");
	} else {
		cortex_output_write_elf_core(info, output);
	}
}
