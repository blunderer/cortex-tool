/** \file cortex_x86_64.c
 * \brief cortex x86_64 specific functions
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

#include <stdlib.h>
#include <string.h>

#include "cortex_elf.h"
#include "arch/cortex_arch.h"

enum cortex_reg_id {
	reg_id_rax = 0,
	reg_id_rbx,
	reg_id_rcx,
	reg_id_rdx,
	reg_id_rbp,
	reg_id_rsp,
	reg_id_rsi,
	reg_id_rdi,
	reg_id_rip,
	reg_id_r8,
	reg_id_r9,
	reg_id_r10,
	reg_id_r11,
	reg_id_r12,
	reg_id_r13,
	reg_id_r14,
	reg_id_r15,
	reg_id_cs,
	reg_id_ss,
	reg_id_orig_rax,
	reg_id_eflags,
};

static struct cortex_cpu_regs cortex_x86_64_cpu_regs[] = {
	{.name = "rax",.size = 8},
	{.name = "rbx",.size = 8},
	{.name = "rcx",.size = 8},
	{.name = "rdx",.size = 8},
	{.name = "rbp",.size = 8},
	{.name = "rsp",.size = 8},
	{.name = "rsi",.size = 8},
	{.name = "rdi",.size = 8},
	{.name = "rip",.size = 8},
	{.name = "r8",.size = 8},
	{.name = "r9",.size = 8},
	{.name = "r10",.size = 8},
	{.name = "r11",.size = 8},
	{.name = "r12",.size = 8},
	{.name = "r13",.size = 8},
	{.name = "r14",.size = 8},
	{.name = "r15",.size = 8},
	{.name = "cs",.size = 8},
	{.name = "ss",.size = 8},
	{.name = "orig_rax ",.size = 8},
	{.name = "eflags",.size = 8},
};

static int cortex_x86_64_fill_regs(struct cortex_proc_info *info,
				   struct pt_regs *pr_regs)
{
	info->cpu_regs = cortex_x86_64_cpu_regs;

	info->cpu_regs[reg_id_rax].value = pr_regs->rax,
	    info->cpu_regs[reg_id_rbx].value = pr_regs->rbx;
	info->cpu_regs[reg_id_rcx].value = pr_regs->rcx;
	info->cpu_regs[reg_id_rdx].value = pr_regs->rdx;
	info->cpu_regs[reg_id_rbp].value = pr_regs->rbp;
	info->cpu_regs[reg_id_rsp].value = pr_regs->rsp;
	info->cpu_regs[reg_id_rsi].value = pr_regs->rsi;
	info->cpu_regs[reg_id_rdi].value = pr_regs->rdi;
	info->cpu_regs[reg_id_rip].value = pr_regs->rip;
	info->cpu_regs[reg_id_r8].value = pr_regs->r8;
	info->cpu_regs[reg_id_r9].value = pr_regs->r9;
	info->cpu_regs[reg_id_r10].value = pr_regs->r10;
	info->cpu_regs[reg_id_r11].value = pr_regs->r11;
	info->cpu_regs[reg_id_r12].value = pr_regs->r12;
	info->cpu_regs[reg_id_r13].value = pr_regs->r13;
	info->cpu_regs[reg_id_r14].value = pr_regs->r14;
	info->cpu_regs[reg_id_r15].value = pr_regs->r15;
	info->cpu_regs[reg_id_cs].value = pr_regs->cs;
	info->cpu_regs[reg_id_ss].value = pr_regs->ss;
	info->cpu_regs[reg_id_orig_rax].value = pr_regs->orig_rax;
	info->cpu_regs[reg_id_eflags].value = pr_regs->eflags;

	return sizeof(cortex_x86_64_cpu_regs) / sizeof(struct cortex_cpu_regs);
}

static long cortex_x86_64_get_pc(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_rip].value;
}

static long cortex_x86_64_get_sp(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_rsp].value;
}

static void *cortex_x86_64_unwind_init(struct cortex_proc_info *info,
				       struct cortex_stack_frame *frame)
{

	frame->pc = info->cpu_regs[reg_id_rip].value;
	frame->sp = info->cpu_regs[reg_id_rsp].value;
	frame->bp = info->cpu_regs[reg_id_rbp].value;

	return NULL;
}

static long cortex_x86_64_unwind_next(struct cortex_proc_info *info,
				      struct cortex_stack_frame *frame,
				      void *data)
{
	struct cortex_stack_frame next;

	if (frame->bp == 0)
		return 0;

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
	return 1;
}

static int cortex_x86_64_get_word_size(void)
{
	return 8;
}

struct cortex_arch_ops cortex_arch_ops = {
	.fill_regs = cortex_x86_64_fill_regs,
	.get_pc = cortex_x86_64_get_pc,
	.get_sp = cortex_x86_64_get_sp,
	.get_word_size = cortex_x86_64_get_word_size,

	.unwind_init = cortex_x86_64_unwind_init,
	.unwind_next = cortex_x86_64_unwind_next,
};
