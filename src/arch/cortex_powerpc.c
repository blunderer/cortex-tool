/** \file cortex_i686.c
 * \brief cortex powerpc specific functions
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
static int cortex_word_size = 8;
#else
static int cortex_word_size = 4;
#endif

enum cortex_reg_id {
	reg_id_gpr0 = 0,
	reg_id_gpr1,
	reg_id_gpr2,
	reg_id_gpr3,
	reg_id_gpr4,
	reg_id_gpr5,
	reg_id_gpr6,
	reg_id_gpr7,
	reg_id_gpr8,
	reg_id_gpr9,
	reg_id_gpr10,
	reg_id_gpr11,
	reg_id_gpr12,
	reg_id_gpr13,
	reg_id_gpr14,
	reg_id_gpr15,
	reg_id_gpr16,
	reg_id_gpr17,
	reg_id_gpr18,
	reg_id_gpr19,
	reg_id_gpr20,
	reg_id_gpr21,
	reg_id_gpr22,
	reg_id_gpr23,
	reg_id_gpr24,
	reg_id_gpr25,
	reg_id_gpr26,
	reg_id_gpr27,
	reg_id_gpr28,
	reg_id_gpr29,
	reg_id_gpr30,
	reg_id_gpr31,
	reg_id_nip,
	reg_id_msr,
	reg_id_orig_gpr3,
	reg_id_ctr,
	reg_id_link,
	reg_id_xer,
	reg_id_ccr,
#ifdef LINUX64
	reg_id_softe,
#else
	reg_id_mq,
#endif
	reg_id_trap,
	reg_id_dar,
	reg_id_dsisr,
	reg_id_result,
};

static struct cortex_cpu_regs cortex_powerpc_cpu_regs[] = {
	{.name = "gpr0",.size = 4,},
	{.name = "gpr1",.size = 4,},
	{.name = "gpr2",.size = 4,},
	{.name = "gpr3",.size = 4,},
	{.name = "gpr4",.size = 4,},
	{.name = "gpr5",.size = 4,},
	{.name = "gpr6",.size = 4,},
	{.name = "gpr7",.size = 4,},
	{.name = "gpr8",.size = 4,},
	{.name = "gpr9",.size = 4,},
	{.name = "gpr10",.size = 4,},
	{.name = "gpr11",.size = 4,},
	{.name = "gpr12",.size = 4,},
	{.name = "gpr13",.size = 4,},
	{.name = "gpr14",.size = 4,},
	{.name = "gpr15",.size = 4,},
	{.name = "gpr16",.size = 4,},
	{.name = "gpr17",.size = 4,},
	{.name = "gpr18",.size = 4,},
	{.name = "gpr19",.size = 4,},
	{.name = "gpr20",.size = 4,},
	{.name = "gpr21",.size = 4,},
	{.name = "gpr22",.size = 4,},
	{.name = "gpr23",.size = 4,},
	{.name = "gpr24",.size = 4,},
	{.name = "gpr25",.size = 4,},
	{.name = "gpr26",.size = 4,},
	{.name = "gpr27",.size = 4,},
	{.name = "gpr28",.size = 4,},
	{.name = "gpr29",.size = 4,},
	{.name = "gpr30",.size = 4,},
	{.name = "gpr31",.size = 4,},
	{.name = "nip",.size = 4,},
	{.name = "msr",.size = 4,},
	{.name = "orig_gpr3",.size = 4,},
	{.name = "ctr",.size = 4,},
	{.name = "link",.size = 4,},
	{.name = "xer",.size = 4,},
	{.name = "ccr",.size = 4,},
#ifdef LINUX64
	{.name = "softe",.size = 4,},
#else
	{.name = "mq",.size = 4,},
#endif
	{.name = "trap",.size = 4,},
	{.name = "dar",.size = 4,},
	{.name = "dsisr",.size = 4,},
	{.name = "result",.size = 4,},
};

static int cortex_powerpc_fill_regs(struct cortex_proc_info *info,
				    struct pt_regs *pr_regs)
{
	info->cpu_regs = cortex_powerpc_cpu_regs;

	info->cpu_regs[reg_id_gpr0].value = pr_regs->gpr[0];
	info->cpu_regs[reg_id_gpr1].value = pr_regs->gpr[1];
	info->cpu_regs[reg_id_gpr2].value = pr_regs->gpr[2];
	info->cpu_regs[reg_id_gpr3].value = pr_regs->gpr[3];
	info->cpu_regs[reg_id_gpr4].value = pr_regs->gpr[4];
	info->cpu_regs[reg_id_gpr5].value = pr_regs->gpr[5];
	info->cpu_regs[reg_id_gpr6].value = pr_regs->gpr[6];
	info->cpu_regs[reg_id_gpr7].value = pr_regs->gpr[7];
	info->cpu_regs[reg_id_gpr8].value = pr_regs->gpr[8];
	info->cpu_regs[reg_id_gpr9].value = pr_regs->gpr[9];
	info->cpu_regs[reg_id_gpr10].value = pr_regs->gpr[10];
	info->cpu_regs[reg_id_gpr11].value = pr_regs->gpr[11];
	info->cpu_regs[reg_id_gpr12].value = pr_regs->gpr[12];
	info->cpu_regs[reg_id_gpr13].value = pr_regs->gpr[13];
	info->cpu_regs[reg_id_gpr14].value = pr_regs->gpr[14];
	info->cpu_regs[reg_id_gpr15].value = pr_regs->gpr[15];
	info->cpu_regs[reg_id_gpr16].value = pr_regs->gpr[16];
	info->cpu_regs[reg_id_gpr17].value = pr_regs->gpr[17];
	info->cpu_regs[reg_id_gpr18].value = pr_regs->gpr[18];
	info->cpu_regs[reg_id_gpr19].value = pr_regs->gpr[19];
	info->cpu_regs[reg_id_gpr20].value = pr_regs->gpr[20];
	info->cpu_regs[reg_id_gpr21].value = pr_regs->gpr[21];
	info->cpu_regs[reg_id_gpr22].value = pr_regs->gpr[22];
	info->cpu_regs[reg_id_gpr23].value = pr_regs->gpr[23];
	info->cpu_regs[reg_id_gpr24].value = pr_regs->gpr[24];
	info->cpu_regs[reg_id_gpr25].value = pr_regs->gpr[25];
	info->cpu_regs[reg_id_gpr26].value = pr_regs->gpr[26];
	info->cpu_regs[reg_id_gpr27].value = pr_regs->gpr[27];
	info->cpu_regs[reg_id_gpr28].value = pr_regs->gpr[28];
	info->cpu_regs[reg_id_gpr29].value = pr_regs->gpr[29];
	info->cpu_regs[reg_id_gpr30].value = pr_regs->gpr[30];
	info->cpu_regs[reg_id_gpr31].value = pr_regs->gpr[31];
	info->cpu_regs[reg_id_nip].value = pr_regs->nip;
	info->cpu_regs[reg_id_msr].value = pr_regs->msr;
	info->cpu_regs[reg_id_orig_gpr3].value = pr_regs->orig_gpr3;
	info->cpu_regs[reg_id_ctr].value = pr_regs->ctr;
	info->cpu_regs[reg_id_link].value = pr_regs->link;
	info->cpu_regs[reg_id_xer].value = pr_regs->xer;
	info->cpu_regs[reg_id_ccr].value = pr_regs->ccr;
#ifdef LINUX64
	info->cpu_regs[reg_id_softe].value = pr_regs->softe;
#else
	info->cpu_regs[reg_id_mq].value = pr_regs->mq;
#endif
	info->cpu_regs[reg_id_trap].value = pr_regs->trap;
	info->cpu_regs[reg_id_dar].value = pr_regs->dar;
	info->cpu_regs[reg_id_dsisr].value = pr_regs->dsisr;
	info->cpu_regs[reg_id_result].value = pr_regs->result;

	return sizeof(cortex_powerpc_cpu_regs) / sizeof(struct cortex_cpu_regs);
}

static long cortex_powerpc_get_pc(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_nip].value;
}

static long cortex_powerpc_get_sp(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_gpr1].value;
}

static int cortex_powerpc_get_word_size(void)
{
	return cortex_word_size;
}

struct cortex_arch_ops cortex_arch_ops = {
	.fill_regs = cortex_powerpc_fill_regs,
	.get_pc = cortex_powerpc_get_pc,
	.get_sp = cortex_powerpc_get_sp,
	.get_word_size = cortex_powerpc_get_word_size,

	.unwind_init = NULL,
	.unwind_next = NULL,
	.unwind_exit = NULL,
};
