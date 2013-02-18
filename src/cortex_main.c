/** \file cortex_main.c
 * \brief cortex main function
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include "config.h"
#include "cortex.h"
#include "cortex_elf.h"
#include "cortex_out.h"

static void cortex_version(void)
{
	printf("%s version %s\nreport bugs to %s\n",
	       PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_BUGREPORT);
	return;
}

static void cortex_usage(char *argv0)
{
	printf("Coredump log extractor\n\nusage: %s [OPTIONS]\nOPTIONS:\n"
	       "\t-i, --input\n\t\tcoredump input file. "
	       "If this option is not present, stdin will be used.\n"
	       "\t-o, --output\n\t\tcoredump input file. "
	       "If this option is not present, stdin will be used.\n"
	       "\t-e, --exec\n\t\tcommand. After the oops is generated, "
	       "call <command> with oops as stdin. incompatible with -o.\n"
	       "\t-f, --format\n\t\tComma separated list of section to display:\n"
	       "\t\t 'gen' for process generic information\n"
	       "\t\t 'reg' for cpu registers\n"
	       "\t\t 'cod' for process instructions\n"
	       "\t\t 'cal' for process call trace\n"
	       "\t\t 'aux' for process auxv\n"
	       "\t\t 'sta' for process stack\n"
	       "\t\tOutput format\n"
	       "\t\t 'txt' to export a text file summary (default)\n"
	       "\t\t 'bin' to export a stripped core file\n"
	       "\t\tOr predefined format\n"
	       "\t\t 'def' for txt,gen,cod,cal\n"
	       "\t\t 'all' for txt,gen,cod,cal,aux,sta\n"
	       "\t-c, --context\n\t\tDisassemble context size in bytes (default 40)\n"
	       "\t-v, --version\n\t\tShow program version and exit.\n"
	       "\t-h, --help\n\t\tShow this help and exit.\n", argv0);
	return;
}

static void cortex_no_input(int signum)
{
	printf("cannot read core file: no input\n");
	exit(1);
}

int main(int argc, char **argv)
{
	int ret = -1;
	int disassemble_ctx = 40;

	struct itimerval timer_wd;
	struct sigaction func_wd;

	struct cortex_elf *core = NULL;
	ElfN_Ehdr *ehdr = NULL;
	struct cortex_proc_info *info = NULL;

	int arg_count = 1;
	int elf_core_fd = 0;
	char *output_cmd = NULL;
	char *output_file = NULL;
	char *input_file = NULL;
	char *fmt = NULL;

	FILE *output = stdout;

	/* Parse all parameters */
	while (arg_count < argc) {
		if ((strcmp(argv[arg_count], "-i") == 0) ||
		    (strcmp(argv[arg_count], "--input") == 0)) {
			input_file = argv[++arg_count];
		} else if ((strcmp(argv[arg_count], "-o") == 0) ||
			   (strcmp(argv[arg_count], "--output") == 0)) {
			output_file = argv[++arg_count];
		} else if ((strcmp(argv[arg_count], "-e") == 0) ||
			   (strcmp(argv[arg_count], "--exec") == 0)) {
			output_cmd = argv[++arg_count];
		} else if ((strcmp(argv[arg_count], "-f") == 0) ||
			   (strcmp(argv[arg_count], "--format") == 0)) {
			fmt = argv[++arg_count];
		} else if ((strcmp(argv[arg_count], "-c") == 0)
			   || (strcmp(argv[arg_count], "--context") == 0)) {
			disassemble_ctx = atoi(argv[++arg_count]);
		} else if ((strcmp(argv[arg_count], "-v") == 0)
			   || (strcmp(argv[arg_count], "--version") == 0)) {
			cortex_version();
			exit(1);
		} else {
			cortex_usage(argv[0]);
			exit(1);
		}
		arg_count++;
	}

	/* If a filename is given, then open it. Else, we gonna use
	   the stdin as input stream. */
	if (input_file) {
		elf_core_fd = open(input_file, O_RDONLY);
		if (elf_core_fd < 0) {
			perror("cannot open core file");
			exit(1);
		}
	}

	/* try to parse output format */
	if (cortex_output_set_format(fmt) < 0) {
		goto out_err;
	}

	/* Install a sig handler for VT ALARM 
	   in case no input is provided to cortex */
	func_wd.sa_handler = cortex_no_input;
	func_wd.sa_flags = 0;
	sigaction(SIGALRM, &func_wd, NULL);

	/* start a timer in case the core is not sent 
	   to stdin (or input option) 2 seconds should be
	   more than enought */
	timer_wd.it_value.tv_sec = 2;
	timer_wd.it_value.tv_usec = 0;
	timer_wd.it_interval.tv_sec = 0;
	timer_wd.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer_wd, NULL);

	/* Here we start the load/parse of the elf core file. */
	core = cortex_elf_load_core(elf_core_fd);

	/* Here we can be sure that something was provided as input */
	timer_wd.it_value.tv_sec = 0;
	timer_wd.it_value.tv_usec = 0;
	timer_wd.it_interval.tv_sec = 0;
	timer_wd.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer_wd, NULL);

	if (core == NULL) {
		goto out_err;
	}

	ehdr = cortex_elf_load_ehdr(core);
	if (ehdr == NULL) {
		goto out_err;
	}

	/* elf core is valid: read it */
	info = cortex_elf_parse(core, ehdr);
	if (info == NULL) {
		goto out_err;
	}

	/* We have a correct elf loaded. Now let's parse
	   the output of cortex: cmd or file */
	if (output_cmd) {
		output = popen(output_cmd, "w");
	} else if (output_file) {
		output = fopen(output_file, "w");
	}

	if (!output) {
		perror("cannot open output");
		exit(1);
	}

	/* parsing is done. now write all we know about current
	 * process to the output stream */
	cortex_output_write_process(info, output, disassemble_ctx);

	/* we got all we want, so cleanup all ressources
	 * and byebye. */
	if (output != stdout) {
		if (output_cmd) {
			pclose(output);
		} else {
			fclose(output);
		}
	}

	ret = 0;
out_err:
	cortex_elf_cleanup_process_info(info);
	cortex_elf_release_core(core);
	close(elf_core_fd);

	return ret;
}
