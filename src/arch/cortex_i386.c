/** \file cortex_i686.c
 * \brief cortex i386 specific functions
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

enum cortex_reg_id {
	reg_id_eax = 0,
	reg_id_ebx,
	reg_id_ecx,
	reg_id_edx,
	reg_id_ebp,
	reg_id_esp,
	reg_id_edi,
	reg_id_esi,
	reg_id_eip,
	reg_id_xcs,
	reg_id_xds,
	reg_id_xes,
	reg_id_xfs,
	reg_id_xgs,
	reg_id_xss,
	reg_id_eflags,
	reg_id_orig_eax,
};

static struct cortex_cpu_regs cortex_i386_cpu_regs[] = {
	{.name = "eax",.size = 4,},
	{.name = "ebx",.size = 4,},
	{.name = "ecx",.size = 4,},
	{.name = "edx",.size = 4,},
	{.name = "ebp",.size = 4,},
	{.name = "esp",.size = 4,},
	{.name = "edi",.size = 4,},
	{.name = "esi",.size = 4,},
	{.name = "eip",.size = 4,},
	{.name = "xcs",.size = 4,},
	{.name = "xds",.size = 4,},
	{.name = "xes",.size = 4,},
	{.name = "xfs",.size = 4,},
	{.name = "xgs",.size = 4,},
	{.name = "xss",.size = 4,},
	{.name = "eflags",.size = 4,},
	{.name = "orig_eax",.size = 4,},
};

static int cortex_i386_fill_regs(struct cortex_proc_info *info,
				 struct pt_regs *pr_regs)
{
	info->cpu_regs = cortex_i386_cpu_regs;

	info->cpu_regs[reg_id_eax].value = pr_regs->eax;
	info->cpu_regs[reg_id_ebx].value = pr_regs->ebx;
	info->cpu_regs[reg_id_ecx].value = pr_regs->ecx;
	info->cpu_regs[reg_id_edx].value = pr_regs->edx;
	info->cpu_regs[reg_id_ebp].value = pr_regs->ebp;
	info->cpu_regs[reg_id_esp].value = pr_regs->esp;
	info->cpu_regs[reg_id_edi].value = pr_regs->edi;
	info->cpu_regs[reg_id_esi].value = pr_regs->esi;
	info->cpu_regs[reg_id_eip].value = pr_regs->eip;
	info->cpu_regs[reg_id_xcs].value = pr_regs->xcs;
	info->cpu_regs[reg_id_xds].value = pr_regs->xds;
	info->cpu_regs[reg_id_xes].value = pr_regs->xes;
	info->cpu_regs[reg_id_xfs].value = pr_regs->xfs;
	info->cpu_regs[reg_id_xgs].value = pr_regs->xgs;
	info->cpu_regs[reg_id_xss].value = pr_regs->xss;
	info->cpu_regs[reg_id_eflags].value = pr_regs->eflags;
	info->cpu_regs[reg_id_orig_eax].value = pr_regs->orig_eax;

	return sizeof(cortex_i386_cpu_regs) / sizeof(struct cortex_cpu_regs);
}

static long cortex_i386_get_pc(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_eip].value;
}

static long cortex_i386_get_sp(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_esp].value;
}

static void *cortex_i386_unwind_init(struct cortex_proc_info *info,
				     struct cortex_stack_frame *frame)
{

	frame->pc = info->cpu_regs[reg_id_eip].value;
	frame->sp = info->cpu_regs[reg_id_esp].value;
	frame->bp = info->cpu_regs[reg_id_ebp].value;

	return NULL;
}

static long cortex_i386_unwind_next(struct cortex_proc_info *info,
				    struct cortex_stack_frame *frame,
				    void *data)
{
	struct cortex_stack_frame next;

	next.pc =
	    *(unsigned long *)(info->stack->d_buf + frame->bp -
			       info->sp_segm->p_vaddr + info->word_size);
	next.sp = frame->bp - info->word_size;
	next.bp =
	    *(unsigned long *)(info->stack->d_buf + frame->bp -
			       info->sp_segm->p_vaddr);

	memcpy(frame, &next, sizeof(struct cortex_stack_frame));

	if (frame->bp == 0)
		return 0;
	if (info->threads[0]->pr_pid == info->threads[0]->pr_pgrp)
		if (*(unsigned long *)
		    (info->stack->d_buf + frame->bp - info->sp_segm->p_vaddr) ==
		    0)
			return 0;

	return 1;
}

static int cortex_i386_get_word_size(void)
{
	return 4;
}

struct cortex_arch_ops cortex_arch_ops = {
	.fill_regs = cortex_i386_fill_regs,
	.get_pc = cortex_i386_get_pc,
	.get_sp = cortex_i386_get_sp,
	.get_word_size = cortex_i386_get_word_size,

	.unwind_init = cortex_i386_unwind_init,
	.unwind_next = cortex_i386_unwind_next,
};
