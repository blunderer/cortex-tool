/** \file cortex_arm.c
 * \brief cortex arm specific functions
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

#include <stdlib.h>
#include <string.h>

#include "cortex.h"
#include "arch/cortex_arch.h"

#ifdef LINUX64
#define CORTEX_WORD_SIZE 8
#else
#define CORTEX_WORD_SIZE 4
#endif

enum cortex_reg_id {
	reg_id_r0 = 0,
	reg_id_r1,
	reg_id_r2,
	reg_id_r3,
	reg_id_r4,
	reg_id_r5,
	reg_id_r6,
	reg_id_r7,
	reg_id_r8,
	reg_id_r9,
	reg_id_r10,
	reg_id_fp,
	reg_id_ip,
	reg_id_sp,
	reg_id_lr,
	reg_id_pc,
	reg_id_cpsr,
	reg_id_orig_r0,
};

static struct cortex_cpu_regs cortex_arm_cpu_regs[] = {
	{.name = "r0",.size = CORTEX_WORD_SIZE},
	{.name = "r1",.size = CORTEX_WORD_SIZE},
	{.name = "r2",.size = CORTEX_WORD_SIZE},
	{.name = "r3",.size = CORTEX_WORD_SIZE},
	{.name = "r4",.size = CORTEX_WORD_SIZE},
	{.name = "r5",.size = CORTEX_WORD_SIZE},
	{.name = "r6",.size = CORTEX_WORD_SIZE},
	{.name = "r7",.size = CORTEX_WORD_SIZE},
	{.name = "r8",.size = CORTEX_WORD_SIZE},
	{.name = "r9",.size = CORTEX_WORD_SIZE},
	{.name = "r10",.size = CORTEX_WORD_SIZE},
	{.name = "fp",.size = CORTEX_WORD_SIZE},
	{.name = "ip",.size = CORTEX_WORD_SIZE},
	{.name = "sp",.size = CORTEX_WORD_SIZE},
	{.name = "lr",.size = CORTEX_WORD_SIZE},
	{.name = "pc",.size = CORTEX_WORD_SIZE},
	{.name = "cpsr",.size = CORTEX_WORD_SIZE},
	{.name = "orig_r0",.size = CORTEX_WORD_SIZE},
};

static int cortex_arm_fill_regs(struct cortex_proc_info *info,
				struct pt_regs *pr_regs)
{
	info->cpu_regs = cortex_arm_cpu_regs;

	info->cpu_regs[reg_id_r0].value = pr_regs->uregs[0];
	info->cpu_regs[reg_id_r1].value = pr_regs->uregs[1];
	info->cpu_regs[reg_id_r2].value = pr_regs->uregs[2];
	info->cpu_regs[reg_id_r3].value = pr_regs->uregs[3];
	info->cpu_regs[reg_id_r4].value = pr_regs->uregs[4];
	info->cpu_regs[reg_id_r5].value = pr_regs->uregs[5];
	info->cpu_regs[reg_id_r6].value = pr_regs->uregs[6];
	info->cpu_regs[reg_id_r7].value = pr_regs->uregs[7];
	info->cpu_regs[reg_id_r8].value = pr_regs->uregs[8];
	info->cpu_regs[reg_id_r9].value = pr_regs->uregs[9];
	info->cpu_regs[reg_id_r10].value = pr_regs->uregs[10];
	info->cpu_regs[reg_id_fp].value = pr_regs->uregs[11];
	info->cpu_regs[reg_id_ip].value = pr_regs->uregs[12];
	info->cpu_regs[reg_id_sp].value = pr_regs->uregs[13];
	info->cpu_regs[reg_id_lr].value = pr_regs->uregs[14];
	info->cpu_regs[reg_id_pc].value = pr_regs->uregs[15];
	info->cpu_regs[reg_id_cpsr].value = pr_regs->uregs[16];
	info->cpu_regs[reg_id_orig_r0].value = pr_regs->uregs[17];

	return sizeof(cortex_arm_cpu_regs) / sizeof(struct cortex_cpu_regs);
}

static long cortex_arm_get_pc(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_pc].value;
}

static long cortex_arm_get_sp(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_sp].value;
}

static void *cortex_arm_unwind_init(struct cortex_proc_info *info,
				    struct cortex_stack_frame *frame)
{
	ElfN_Addr *lr = malloc(sizeof(ElfN_Addr));

	*lr = info->cpu_regs[reg_id_lr].value;

	frame->pc = info->cpu_regs[reg_id_pc].value;
	frame->sp = info->cpu_regs[reg_id_sp].value;
	frame->bp = info->cpu_regs[reg_id_fp].value;

	return lr;
}

static long cortex_arm_unwind_next(struct cortex_proc_info *info,
				   struct cortex_stack_frame *frame, void *data)
{
	struct cortex_stack_frame next;
	ElfN_Addr *lr = (ElfN_Addr *) data;

	if (frame->bp == 0)
		return 0;

	if (frame->bp < frame->sp)
		return 0;

	next.pc = *lr;
	next.sp = frame->bp - info->word_size;

	if (next.pc == (unsigned long)info->cpu_regs[reg_id_lr].value) {
		next.bp =
		    *(unsigned long *)(info->stack->d_buf + next.sp -
				       info->sp_segm->p_vaddr);
	} else {
		next.bp =
		    *(unsigned long *)(info->stack->d_buf + next.sp -
				       info->word_size -
				       info->sp_segm->p_vaddr);
	}

	*lr =
	    *(unsigned long *)(info->stack->d_buf + frame->bp +
			       info->word_size - info->sp_segm->p_vaddr);

	memcpy(frame, &next, sizeof(struct cortex_stack_frame));

	if (next.pc == 0)
		return 0;
	return 1;
}

static void cortex_arm_unwind_exit(struct cortex_proc_info *info, void *data)
{
	ElfN_Addr *lr = (ElfN_Addr *) data;
	free(lr);
}

static int cortex_arm_get_word_size(void)
{
	return CORTEX_WORD_SIZE;
}

struct cortex_arch_ops cortex_arch_ops = {
	.fill_regs = cortex_arm_fill_regs,
	.get_pc = cortex_arm_get_pc,
	.get_sp = cortex_arm_get_sp,
	.get_word_size = cortex_arm_get_word_size,

	.unwind_init = cortex_arm_unwind_init,
	.unwind_next = cortex_arm_unwind_next,
	.unwind_exit = cortex_arm_unwind_exit,
};
