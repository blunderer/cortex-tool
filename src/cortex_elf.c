/** \file struct cortex_elf.c
 * \brief cortex elf functions
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "cortex.h"
#include "cortex_elf.h"
#include "arch/cortex_arch.h"

#define max(a, b)		(((a)>(b))?(a):(b))
#define min(a, b)		(((a)<(b))?(a):(b))

#define ELF_DATA_ALIGN(a, d)	((long)(a) + d - ((long)(a) % (d)))

static long __cortex_fseek(struct cortex_elf *core, off_t offset)
{
	char __dummy[512];

	offset -= core->offset;

	while (offset) {
		size_t block = min(512, offset);
		size_t count = read(core->fd, __dummy, block);
		if (count > 0) {
			offset -= count;
			core->offset += count;
		} else {
			goto err_out;
		}
	}
err_out:
	return core->offset;
}

static long __cortex_elf_read(struct cortex_elf *core, void *buf, size_t count)
{
	unsigned long nbytes = 0;

	while (nbytes < count) {
		int ret = read(core->fd, buf, count - nbytes);
		if (ret < 0)
			break;
		nbytes += ret;
	}

	if (nbytes > 0)
		core->offset += nbytes;

	return nbytes;
}

static struct cortex_elf *cortex_elf_begin(int fd)
{
	struct cortex_elf *elf = calloc(1, sizeof(struct cortex_elf));
	if (!elf)
		goto out_err;

	elf->fd = fd;
	elf->offset = 0;

out_err:
	return elf;
}

static void cortex_elf_end(struct cortex_elf *core)
{
	if (!core)
		goto out_err;

	free(core->ehdr);
	free(core->phdr);

	if (core->fd >= 0)
		close(core->fd);

out_err:
	return;
}

static unsigned char *cortex_elf_getident(struct cortex_elf *core,
					  size_t * count)
{
	unsigned char *ident = NULL;

	if (!core->ehdr) {
		long count = 0;

		if (core->offset != 0) {
			fprintf(stderr, "%s: non fast forward\n", __FILE__);
			goto out_err;
		}

		core->ehdr = calloc(1, sizeof(ElfN_Ehdr));

		count = __cortex_elf_read(core, core->ehdr, sizeof(ElfN_Ehdr));
		if (count <= 0) {
			fprintf(stderr, "%s: cannot read file\n", __FILE__);
			goto out_err;
		} else if (count < (long)sizeof(ElfN_Ehdr)) {
			fprintf(stderr, "%s: truncated file\n", __FILE__);
			goto out_err;
		}

	}

	if (count)
		*count = sizeof(core->ehdr->e_ident);
	ident = core->ehdr->e_ident;

	return ident;
out_err:
	free(core->ehdr);
	core->ehdr = NULL;
	return NULL;
}

static ElfN_Ehdr *cortex_elf_getehdr(struct cortex_elf *core)
{
	ElfN_Ehdr *ehdr = NULL;
	if (!core->ehdr) {
		long count = 0;

		if (core->offset != 0) {
			fprintf(stderr, "%s: non fast forward\n", __FILE__);
			goto out_err;
		}

		core->ehdr = calloc(1, sizeof(ElfN_Ehdr));

		count = __cortex_elf_read(core, core->ehdr, sizeof(ElfN_Ehdr));
		if (count <= 0) {
			fprintf(stderr, "%s: cannot read file\n", __FILE__);
			goto out_err;
		} else if (count < (long)sizeof(ElfN_Ehdr)) {
			fprintf(stderr, "%s: truncated file\n", __FILE__);
			goto out_err;
		}

	}

	ehdr = core->ehdr;

	return ehdr;
out_err:
	free(core->ehdr);
	core->ehdr = NULL;
	return NULL;
}

static ElfN_Phdr *cortex_elf_getphdr(struct cortex_elf *core)
{
	ElfN_Phdr *phdr = NULL;
	if (!core->phdr) {
		long count = 0;

		if (core->offset > core->ehdr->e_phoff) {
			fprintf(stderr, "%s: non fast forward\n", __FILE__);
			goto out_err;
		}

		__cortex_fseek(core, core->ehdr->e_phoff);

		core->phdr = calloc(core->ehdr->e_phnum, sizeof(ElfN_Phdr));

		count =
		    __cortex_elf_read(core, core->phdr,
				      core->ehdr->e_phnum * sizeof(ElfN_Phdr));
		if (count <= 0) {
			fprintf(stderr, "%s: cannot read file\n", __FILE__);
			goto out_err;
		} else if (count <
			   (long)(core->ehdr->e_phnum * sizeof(ElfN_Phdr))) {
			fprintf(stderr, "%s: truncated file\n", __FILE__);
			goto out_err;
		}

	}

	phdr = core->phdr;

	return phdr;
out_err:
	free(core->phdr);
	core->phdr = NULL;
	return NULL;
}

static struct cortex_elf_data *cortex_elf_getdata(struct cortex_elf *core,
						  ElfN_Phdr * phdr)
{
	long count = 0;
	struct cortex_elf_data *elf_data = NULL;

	if (core->offset > phdr->p_offset) {
		fprintf(stderr, "%s: non fast forward (%d %d)\n", __FILE__,
			core->offset, phdr->p_offset);
		goto out_err;
	}

	__cortex_fseek(core, phdr->p_offset);

	elf_data = calloc(1, sizeof(struct cortex_elf_data));
	elf_data->d_buf = calloc(1, phdr->p_filesz);
	elf_data->d_size = phdr->p_filesz;
	elf_data->d_align = phdr->p_align;

	count = __cortex_elf_read(core, elf_data->d_buf, phdr->p_filesz);
	if (count <= 0) {
		fprintf(stderr, "%s: cannot read file\n", __FILE__);
		goto out_err;
	} else if (count < (long)phdr->p_filesz) {
		fprintf(stderr, "%s: truncated file\n", __FILE__);
		goto out_err;
	}

	return elf_data;
out_err:
	free(elf_data->d_buf);
	free(elf_data);
	elf_data = NULL;
	return NULL;
}

static int cortex_elf_count_thread(struct cortex_elf_data *pt_note)
{
	int thread_cnt = 0;
	long nhdr_end = (long)pt_note->d_buf + pt_note->d_size;

	ElfN_Nhdr *nhdr = (ElfN_Nhdr *) pt_note->d_buf;

	while ((long)nhdr < nhdr_end) {
		long align = max(4, pt_note->d_align);
		long note_total_size =
		    sizeof(ElfN_Nhdr) + nhdr->n_namesz + nhdr->n_descsz;
		long offset = (long)nhdr + note_total_size;

		switch (nhdr->n_type) {
		case NT_PRSTATUS:
			thread_cnt++;
			break;
		default:
			break;
		}

		nhdr = (ElfN_Nhdr *) ELF_DATA_ALIGN(offset, align);
	}
	return thread_cnt;
}

static struct cortex_proc_info *cortex_elf_parse_note(struct cortex_elf_data
						      *pt_note)
{
	int thread_cnt = 0;
	long nhdr_end = (long)pt_note->d_buf + (long)pt_note->d_size;

	ElfN_Nhdr *nhdr = (ElfN_Nhdr *) pt_note->d_buf;

	struct cortex_proc_info *proc = calloc(1, sizeof(*proc));
	if (proc == NULL) {
		return proc;
	}

	proc->nr_threads = cortex_elf_count_thread(pt_note);
	proc->threads = calloc(proc->nr_threads, sizeof(struct elf_prstatus *));

	if (proc->nr_threads == 0) {
		return NULL;
	}

	/* parse all notes depending on their type */
	while ((long)nhdr < nhdr_end) {
		long align = max(4, pt_note->d_align);
		long note_nhdr_size = sizeof(ElfN_Nhdr) + nhdr->n_namesz;
		long note_total_size = note_nhdr_size + nhdr->n_descsz;
		long desc = (long)nhdr + note_nhdr_size;
		long offset = (long)nhdr + note_total_size;

		switch (nhdr->n_type) {
		case NT_PRSTATUS:
			proc->threads[thread_cnt++] =
			    (struct elf_prstatus *)ELF_DATA_ALIGN(desc, align);
			break;
		case NT_PRPSINFO:
			proc->info =
			    (struct elf_prpsinfo *)ELF_DATA_ALIGN(desc, align);
			break;
		case NT_AUXV:
			proc->auxv =
			    (ElfN_auxv_t *) ELF_DATA_ALIGN(desc, align);
			break;
		default:
			break;
		}

		nhdr = (ElfN_Nhdr *) ELF_DATA_ALIGN(offset, align);
	}

	/* fill some global structure helpers */
	if (proc->info == NULL) {
		free(proc->threads);
		free(proc);
		proc = NULL;
		return NULL;
	}
	proc->pid = proc->info->pr_pid;
	proc->signum = proc->threads[0]->pr_cursig;

	return proc;
}

static int cortex_check_ident(struct cortex_elf *core)
{
	unsigned char *e_ident = cortex_elf_getident(core, NULL);

	if (!e_ident) {
		return -1;
	}

	/* check magic */
	if (e_ident[EI_MAG0] != ELFMAG0) {
		printf("elf: wrong magic[0].\n");
		return -1;
	}
	if (e_ident[EI_MAG1] != ELFMAG1) {
		printf("elf: wrong magic[1].\n");
		return -1;
	}
	if (e_ident[EI_MAG2] != ELFMAG2) {
		printf("elf: wrong magic[2].\n");
		return -1;
	}
	if (e_ident[EI_MAG3] != ELFMAG3) {
		printf("elf: wrong magic[3].\n");
		return -1;
	}

	/* check elf file class */
	if (e_ident[EI_CLASS] != ELFCLASSN) {
		printf("elf: file doesn't have a compatible class.\n");
		return -1;
	}
	/* check elf file class */
	if (e_ident[EI_VERSION] != EV_CURRENT) {
		printf("elf: file doesn't have a compatible version.\n");
		return -1;
	}

	return 0;
}

static struct cortex_elf_data *cortex_load_segment(struct cortex_elf *core,
						   ElfN_Ehdr * ehdr,
						   ElfN_Phdr * phdr)
{
	int i = 0;
	struct cortex_elf_data *data = NULL;
	ElfN_Off offset = 0;

	if (!phdr)
		goto err_out;

	offset = phdr[0].p_offset;

	while ((phdr[i].p_offset == offset) && (!phdr[i].p_filesz)
	       && (i < ehdr->e_phnum)) {
		i++;
	}

	if (phdr[i].p_offset == offset) {
		data = cortex_elf_getdata(core, phdr + i);
	}

err_out:
	return data;
}

static ElfN_Phdr *cortex_find_segment_type(ElfN_Phdr * phdr, ElfN_Ehdr * ehdr,
					   uint32_t type)
{
	int i = 0;
	ElfN_Phdr *segm = NULL;

	for (i = 0; i < ehdr->e_phnum; i++) {
		if (phdr[i].p_type == type) {
			segm = phdr + i;
			break;
		}
	}

	return segm;
}

static ElfN_Phdr *cortex_find_segment_vaddr(ElfN_Phdr * phdr, ElfN_Ehdr * ehdr,
					    ElfN_Addr vaddr)
{
	int i = 0;
	ElfN_Phdr *segm = NULL;

	for (i = 0; i < ehdr->e_phnum; i++) {
		ElfN_Addr vaddr_start = phdr[i].p_vaddr;
		ElfN_Addr vaddr_end = phdr[i].p_vaddr + phdr[i].p_memsz;

		if ((vaddr_start < vaddr) && (vaddr < vaddr_end)) {
			segm = phdr + i;
			break;
		}
	}

	return segm;
}

struct cortex_proc_info *cortex_elf_parse(struct cortex_elf *core,
					  ElfN_Ehdr * ehdr)
{
	struct pt_regs *pr_regs;
	struct cortex_elf_data *data = NULL;
	struct cortex_proc_info *info = NULL;

	ElfN_Phdr *note = 0;
	ElfN_Phdr *phdr = cortex_elf_getphdr(core);

	/* retrieve generic information about the process
	   registers... */
	note = cortex_find_segment_type(phdr, ehdr, PT_NOTE);
	if (note == NULL)
		goto err_out;
	data = cortex_load_segment(core, ehdr, note);
	if (data == NULL)
		goto err_out;
	info = cortex_elf_parse_note(data);
	if (info == NULL)
		goto err_out;

	info->note_segm = note;
	info->note = data;
	info->elf = core;

	pr_regs = (struct pt_regs *)info->threads[0]->pr_reg;
	info->word_size = cortex_arch_ops.get_word_size();
	info->cpu_regs_nr = cortex_arch_ops.fill_regs(info, pr_regs);

	/* Then look for the segment that contains
	   the instruction pointer */
	info->pc = cortex_arch_ops.get_pc(info->cpu_regs);
	info->pc_segm = cortex_find_segment_vaddr(phdr, ehdr, info->pc);
	info->code = cortex_load_segment(core, ehdr, info->pc_segm);

	/* Finally, load the stack segment */
	info->sp = cortex_arch_ops.get_sp(info->cpu_regs);
	info->sp_segm = cortex_find_segment_vaddr(phdr, ehdr, info->sp);
	info->stack = cortex_load_segment(core, ehdr, info->sp_segm);

	return info;
err_out:
	fprintf(stderr, "Cannot read segment PT_NOTE\n");
	return NULL;
}

struct cortex_elf *cortex_elf_load_core(int fd)
{
	struct cortex_elf *core = NULL;

	core = cortex_elf_begin(fd);

	if (cortex_check_ident(core) < 0) {
		cortex_elf_end(core);
		core = NULL;
	}

	return core;
}

ElfN_Ehdr *cortex_elf_load_ehdr(struct cortex_elf * core)
{
	ElfN_Ehdr *ehdr = cortex_elf_getehdr(core);
	if (ehdr == NULL) {
		cortex_elf_end(core);
		goto out_err;
	}

	/* check elf file type */
	if (ehdr->e_type != ET_CORE) {
		printf("elf: not a core file.\n");
		return NULL;
	}

	/* check elf machine type */
	if (ehdr->e_machine != MACHINE) {
		printf("elf: wrong machine. Got %d but expect %d\n",
		       ehdr->e_machine, MACHINE);
		return NULL;
	}
out_err:
	return ehdr;
}

void cortex_elf_cleanup_process_info(struct cortex_proc_info *info)
{
	if (info) {
		if (info->stack)
			free(info->stack->d_buf);
		if (info->note)
			free(info->note->d_buf);
		if (info->code)
			free(info->code->d_buf);
		free(info->stack);
		free(info->note);
		free(info->code);
		free(info->threads);
		free(info);
	}
}

void cortex_elf_release_core(struct cortex_elf *core)
{
	cortex_elf_end(core);
	free(core);
}
