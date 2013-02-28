
#ifndef _CORTEX_ARCH_H_
#define _CORTEX_ARCH_H_

/** \file cortex_arch.h
 * \brief cortex arch common definition
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
#include <stdio.h>
#include <asm/ptrace.h>

#include "cortex.h"

#define REG_NAME_SZ	16

extern struct cortex_arch_ops cortex_arch_ops;

struct cortex_cpu_regs {
	char name[REG_NAME_SZ];
	size_t size;
	long value;
};

struct cortex_arch_ops {
	int (*fill_regs) (struct cortex_proc_info * info,
			  struct pt_regs * pr_regs);
	int (*get_word_size) (void);
	long (*get_sp) (struct cortex_cpu_regs * cpu_regs);
	long (*get_pc) (struct cortex_cpu_regs * cpu_regs);
	long (*unwind_next) (struct cortex_proc_info * info,
			     struct cortex_stack_frame * frame, void *data);
	void (*unwind_exit) (struct cortex_proc_info * info, void *data);
	void *(*unwind_init) (struct cortex_proc_info * info,
			      struct cortex_stack_frame * frame);
};

#endif /* _CORTEX_ARCH_H_ */
