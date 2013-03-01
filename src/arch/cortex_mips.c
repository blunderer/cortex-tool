/** \file cortex_mips.c
 * \brief cortex mips specific functions
 * \author Tristan Lelong <tristan.lelong@blunderer.org>
 * \date 2013 03 01
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
	init = -1,
#ifdef CONFIG_32BIT
	reg_id_pad0_0,
	reg_id_pad0_1,
	reg_id_pad0_2,
	reg_id_pad0_3,
	reg_id_pad0_4,
	reg_id_pad0_5,
#endif
	reg_id_regs_0,
	reg_id_regs_1,
	reg_id_regs_2,
	reg_id_regs_3,
	reg_id_regs_4,
	reg_id_regs_5,
	reg_id_regs_6,
	reg_id_regs_7,
	reg_id_regs_8,
	reg_id_regs_9,
	reg_id_regs_10,
	reg_id_regs_11,
	reg_id_regs_12,
	reg_id_regs_13,
	reg_id_regs_14,
	reg_id_regs_15,
	reg_id_regs_16,
	reg_id_regs_17,
	reg_id_regs_18,
	reg_id_regs_19,
	reg_id_regs_20,
	reg_id_regs_21,
	reg_id_regs_22,
	reg_id_regs_23,
	reg_id_regs_24,
	reg_id_regs_25,
	reg_id_regs_26,
	reg_id_regs_27,
	reg_id_regs_28,
	reg_id_regs_29,
	reg_id_regs_30,
	reg_id_regs_31,
	reg_id_cp0_status,
	reg_id_hi,
	reg_id_lo,
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	reg_id_acx,
#endif
	reg_id_cp0_badvaddr,
	reg_id_cp0_cause,
	reg_id_cp0_epc,
#ifdef CONFIG_MIPS_MT_SMTC
	reg_id_cp0_tcstatus,
#endif /* CONFIG_MIPS_MT_SMTC */
#ifdef CONFIG_CPU_CAVIUM_OCTEON
	reg_id_mpl_0,
	reg_id_mpl_1,
	reg_id_mpl_2,
	reg_id_mtp_0,
	reg_id_mtp_1,
	reg_id_mtp_2,
#endif
};

static struct cortex_cpu_regs cortex_mips_cpu_regs[] = {
#ifdef CONFIG_32BIT
	{.name = "pad0_0",.size = CORTEX_WORD_SIZE,},
	{.name = "pad0_1",.size = CORTEX_WORD_SIZE,},
	{.name = "pad0_2",.size = CORTEX_WORD_SIZE,},
	{.name = "pad0_3",.size = CORTEX_WORD_SIZE,},
	{.name = "pad0_4",.size = CORTEX_WORD_SIZE,},
	{.name = "pad0_5",.size = CORTEX_WORD_SIZE,},
#endif
	{.name = "regs_0",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_1",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_2",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_3",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_4",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_5",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_6",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_7",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_8",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_9",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_10",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_11",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_12",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_13",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_14",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_15",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_16",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_17",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_18",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_19",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_20",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_21",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_22",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_23",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_24",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_25",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_26",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_27",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_28",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_29",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_30",.size = CORTEX_WORD_SIZE,},
	{.name = "regs_31",.size = CORTEX_WORD_SIZE,},
	{.name = "cp0_status",.size = CORTEX_WORD_SIZE,},
	{.name = "hi",.size = CORTEX_WORD_SIZE,},
	{.name = "lo",.size = CORTEX_WORD_SIZE,},
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	{.name = "acx",.size = CORTEX_WORD_SIZE,},
#endif
	{.name = "cp0_badvaddr",.size = CORTEX_WORD_SIZE,},
	{.name = "cp0_cause",.size = CORTEX_WORD_SIZE,},
	{.name = "cp0_epc",.size = CORTEX_WORD_SIZE,},
#ifdef CONFIG_MIPS_MT_SMTC
	{.name = "cp0_tcstatus",.size = CORTEX_WORD_SIZE,},
#endif /* CONFIG_MIPS_MT_SMTC */
#ifdef CONFIG_CPU_CAVIUM_OCTEON
	{.name = "mpl_0",.size = CORTEX_WORD_SIZE,},
	{.name = "mpl_1",.size = CORTEX_WORD_SIZE,},
	{.name = "mpl_2",.size = CORTEX_WORD_SIZE,},
	{.name = "mtp_0",.size = CORTEX_WORD_SIZE,},
	{.name = "mtp_1",.size = CORTEX_WORD_SIZE,},
	{.name = "mtp_2",.size = CORTEX_WORD_SIZE,},
#endif
};

static int cortex_mips_fill_regs(struct cortex_proc_info *info,
				 struct pt_regs *pr_regs)
{
	info->cpu_regs = cortex_mips_cpu_regs;

#ifdef CONFIG_32BIT
	info->cpu_regs[reg_id_pad0_0].value = pr_regs->pad0[0];
	info->cpu_regs[reg_id_pad0_1].value = pr_regs->pad0[1];
	info->cpu_regs[reg_id_pad0_2].value = pr_regs->pad0[2];
	info->cpu_regs[reg_id_pad0_3].value = pr_regs->pad0[3];
	info->cpu_regs[reg_id_pad0_4].value = pr_regs->pad0[4];
	info->cpu_regs[reg_id_pad0_5].value = pr_regs->pad0[5];
#endif
	info->cpu_regs[reg_id_regs_0].value = pr_regs->regs[0];
	info->cpu_regs[reg_id_regs_1].value = pr_regs->regs[1];
	info->cpu_regs[reg_id_regs_2].value = pr_regs->regs[2];
	info->cpu_regs[reg_id_regs_3].value = pr_regs->regs[3];
	info->cpu_regs[reg_id_regs_4].value = pr_regs->regs[4];
	info->cpu_regs[reg_id_regs_5].value = pr_regs->regs[5];
	info->cpu_regs[reg_id_regs_6].value = pr_regs->regs[6];
	info->cpu_regs[reg_id_regs_7].value = pr_regs->regs[7];
	info->cpu_regs[reg_id_regs_8].value = pr_regs->regs[8];
	info->cpu_regs[reg_id_regs_9].value = pr_regs->regs[9];
	info->cpu_regs[reg_id_regs_10].value = pr_regs->regs[10];
	info->cpu_regs[reg_id_regs_11].value = pr_regs->regs[11];
	info->cpu_regs[reg_id_regs_12].value = pr_regs->regs[12];
	info->cpu_regs[reg_id_regs_13].value = pr_regs->regs[13];
	info->cpu_regs[reg_id_regs_14].value = pr_regs->regs[14];
	info->cpu_regs[reg_id_regs_15].value = pr_regs->regs[15];
	info->cpu_regs[reg_id_regs_16].value = pr_regs->regs[16];
	info->cpu_regs[reg_id_regs_17].value = pr_regs->regs[17];
	info->cpu_regs[reg_id_regs_18].value = pr_regs->regs[18];
	info->cpu_regs[reg_id_regs_19].value = pr_regs->regs[19];
	info->cpu_regs[reg_id_regs_20].value = pr_regs->regs[20];
	info->cpu_regs[reg_id_regs_21].value = pr_regs->regs[21];
	info->cpu_regs[reg_id_regs_22].value = pr_regs->regs[22];
	info->cpu_regs[reg_id_regs_23].value = pr_regs->regs[23];
	info->cpu_regs[reg_id_regs_24].value = pr_regs->regs[24];
	info->cpu_regs[reg_id_regs_25].value = pr_regs->regs[25];
	info->cpu_regs[reg_id_regs_26].value = pr_regs->regs[26];
	info->cpu_regs[reg_id_regs_27].value = pr_regs->regs[27];
	info->cpu_regs[reg_id_regs_28].value = pr_regs->regs[28];
	info->cpu_regs[reg_id_regs_29].value = pr_regs->regs[29];
	info->cpu_regs[reg_id_regs_30].value = pr_regs->regs[30];
	info->cpu_regs[reg_id_regs_31].value = pr_regs->regs[31];
	info->cpu_regs[reg_id_cp0_status].value = pr_regs->cp0_status;
	info->cpu_regs[reg_id_hi].value = pr_regs->hi;
	info->cpu_regs[reg_id_lo].value = pr_regs->lo;
#ifdef CONFIG_CPU_HAS_SMARTMIPS
	info->cpu_regs[reg_id_acx].value = pr_regs->acx;
#endif
	info->cpu_regs[reg_id_cp0_badvaddr].value = pr_regs->cp0_badvaddr;
	info->cpu_regs[reg_id_cp0_cause].value = pr_regs->cp0_cause;
	info->cpu_regs[reg_id_cp0_epc].value = pr_regs->cp0_epc;
#ifdef CONFIG_MIPS_MT_SMTC
	info->cpu_regs[reg_id_cp0_tcstatus].value = pr_regs->cp0_tcstatus;
#endif /* CONFIG_MIPS_MT_SMTC */
#ifdef CONFIG_CPU_CAVIUM_OCTEON
	info->cpu_regs[reg_id_mpl_0].value = pr_regs->mpl[0];
	info->cpu_regs[reg_id_mpl_1].value = pr_regs->mpl[1];
	info->cpu_regs[reg_id_mpl_2].value = pr_regs->mpl[2];
	info->cpu_regs[reg_id_mtp_0].value = pr_regs->mtp[0];
	info->cpu_regs[reg_id_mtp_1].value = pr_regs->mtp[1];
	info->cpu_regs[reg_id_mtp_2].value = pr_regs->mtp[2];
#endif

	return sizeof(cortex_mips_cpu_regs) / sizeof(struct cortex_cpu_regs);
}

static long cortex_mips_get_pc(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_cp0_epc].value;
}

static long cortex_mips_get_sp(struct cortex_cpu_regs *cpu_regs)
{
	return cpu_regs[reg_id_regs_29].value;
}

static int cortex_mips_get_word_size(void)
{
	return CORTEX_WORD_SIZE;
}

struct cortex_arch_ops cortex_arch_ops = {
	.fill_regs = cortex_mips_fill_regs,
	.get_pc = cortex_mips_get_pc,
	.get_sp = cortex_mips_get_sp,
	.get_word_size = cortex_mips_get_word_size,
};
