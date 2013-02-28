/** \file cortex.h
 * \brief User OOPS header
 * \author Tristan Lelong <tristan.lelong@blunderer.org>
 * \date 2011 06 27
 *
 * This segment is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This segment is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this segment; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CORTEX_H_
#define _CORTEX_H_

#include "cortex_elf.h"

/** \mainpage
 *
 * \section intro INTRODUCTION
 * \b cortex is a program that can be use for coredump auto info extraction: OOPS generation for user applications.<br>
 * It will read a coredump from \em stdin (or input file) and output useful information to \em stdout (or output file).
 *
 * \subsection options OPTIONS
 * Refer to the man page for usage.
 *
 * \subsection content OOPS CONTENT
 * The OOPS output contains some useful pieces of information about running process such as:
 * - \b Name: The binary application filename
 * - \b Arguments: All arguments used for launching the application
 * - \b uid/gid: The user and group ID that of the crashed application
 * - \b utime / cutime / stime / cstime: User / system times and cumulativ times
 * - \b State: the state of app when coredump was generated: [R | S | D | T | Z | W]
 * - \b Threads: The number of thread owned by application
 * - \b Signo: The unix signal received and the PID of the thread that received it.
 * - \b Registers: The content of CPU registers
 * - \b Backtrace: The content of stack for all frames
 * - \b Source code: The Hex code where crash occured (or asm if disassembly is possible);
 * - \b Last stack frame:
 * - \b Auxiliary vector:
 *
 * \subsection elf_specific ELF SPECIFIC FUNCTIONS
 * This section describe all architecture dependant functions.<br>
 * These functions are implemented for each arch in a file called cortex_<arch>.c<br>
 * when configuring cortex, the --host option is used to choose the targeted architecture.<br>
 * The struct cortex_arch_ops cortex_arch_ops variable contains pointer to all
 * those functions.
 * list:
 * - \ref int fill_regs(struct cortex_proc_info *info, struct pt_regs *pr_regs)
 * - \ref long get_pc(struct cortex_cpu_regs *cpu_regs)
 * - \ref long get_sp(struct cortex_cpu_regs *cpu_regs)
 * - \ref int get_word_size(void)
 * - \ref void *unwind_init(struct cortex_proc_info *info, struct cortex_stack_frame *frame)
 * - \ref long unwind_next(struct cortex_proc_info *info, struct cortex_stack_frame *frame, void *data)
 * - \ref void unwind_exit(struct cortex_proc_info *info, void *data)
 */

#define STACK_FRAME_MAX 256
#define CORTEX_EMPTY_FRAME	{0, 0, 0}

#define CORTEX_OUTPUT_FMT_GEN		0x0002
#define CORTEX_OUTPUT_FMT_REG		0x0004
#define CORTEX_OUTPUT_FMT_COD		0x0008
#define CORTEX_OUTPUT_FMT_CAL		0x0010
#define CORTEX_OUTPUT_FMT_AUX		0x0020
#define CORTEX_OUTPUT_FMT_STA		0x0040
#define CORTEX_OUTPUT_FMT_ALL		0x007E
#define CORTEX_OUTPUT_FMT_DEF		0x001E
#define CORTEX_OUTPUT_FMT_BIN		0x0001
#define CORTEX_OUTPUT_FMT_TXT		0x0000

/** \struct cortex_proc_info
 ** \brief generic process info
 *
 * We store here all process info that do not depends on arch
 */
struct cortex_proc_info {
	int pid;		/*!< the PID of the process */
	int signum;		/*!< the sig number received by the process if any */
	int activ_id;		/*!< index of the thread that was active */
	int nr_threads;		/*!< the number of thread created by the process */
	int word_size;		/*!< size of a void* for target arch */

	struct cortex_elf *elf;	/*!< ELF core descriptor */

	ElfN_Phdr *note_segm;	/*!< note segment */
	struct cortex_elf_data *note;	/*!< note segment data */

	ElfN_Addr pc;		/*!< instruction pointer */
	ElfN_Phdr *pc_segm;	/*!< code segment */
	struct cortex_elf_data *code;	/*!< code segment data */

	ElfN_Addr sp;		/*!< stack pointer */
	ElfN_Phdr *sp_segm;	/*!< stack segment */
	struct cortex_elf_data *stack;	/*!< stack segment data */

	ElfN_auxv_t *auxv;	/*!< mapping of auxv table structure */
	struct elf_prpsinfo *info;	/*!< mapping of generic elf info structure */
	struct elf_prstatus **threads;	/*!< mapping of all thread elf infos */

	long cpu_regs_nr;	/*!< cpu registers numbers */
	struct cortex_cpu_regs *cpu_regs;	/*!< cpu registers (arch dependent) */
};

struct cortex_stack_frame {
	ElfN_Addr pc;
	ElfN_Addr sp;
	ElfN_Addr bp;
};

int cortex_elf_perform_check(struct cortex_elf *core);

void cortex_elf_release_core(struct cortex_elf *core);

struct cortex_elf *cortex_elf_load_core(int fd);

#endif /* _CORTEX_H_ */
