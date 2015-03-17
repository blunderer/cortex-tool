# CORTEX(1) #

## NAME ##
**cortex** - coredump to textreport converter

## SYNOPSIS ##
**cortex** _options_

## DESCRIPTION ##
This manual page documents briefly the **cortex** commands.

**cortex**  is  a  program  that  can  be  use for coredump automatic info extraction: it reads core ELF file, extract pertinent information and write them to a text file. This text file is lighter than a core dump but contains most required information for a developper to perform a bug analysis.

**cortex** will work only with ELF core dump. It supports several CPU architectures:

  * i386 / i486 / i586 / i686
  * x86\_64
  * arm

**cortex** will read a coredump from stdin (or input file ) and output useful information to stdout (or output file or output cmd ).

## OPTIONS ##
This program follow the usual GNU command line syntax, with long options starting with two dashes (`-').  A summary of options is included below.

  * -i, --input: core dump input file.  If this option is not present, stdin will be used.
  * -o, --output: text output file.  If this option is not present, stdout will be used.
  * -e, --exec: **cortex** output command.  Will call given command piping text output to stdin. If present, output option will be ignored
  * -f, --format: **cortex** output format.  Will select what to display and how to display it. See
  * -c, --context: disassemble context size.  Describe the number of bytes of disassembled context (default 40) Format section for more information.
  * -h, --help: Show summary of options.
  * -v, --version: Show program version and exit.

## OUTPUT ##
The text output contains some useful pieces of information about running process such as:

  * Name: The binary application filename
  * Arguments: All arguments used for launching the application
  * uid/gid: The user and group ID that of the crashed application
  * utime / cutime / stime / cstime: User / system times and cumulativ times
  * State: the state of app when coredump was generated: _R | S | D | T | Z | W_
  * Threads: The number of thread owned by application
  * Signo: The unix signal received and the PID of the thread that received it.
  * Registers: The content of CPU registers
  * Source code: The Hex code where crash occured (or asm if disassembly is possible);
  * Stack: The content of the last stack frame
  * Call trace: The current instruction pointer and return instruction pointers for each frames
  * Auxiliary vector: The auxiliary vector for this process.

## FORMAT ##
Custom format are:

  * gen: Generic information about process.
  * reg: CPU registers.
  * cod: Process instructions.
  * cal: Process call trace.
  * aux: Process auxiliary vector.
  * sta: Process current stack frame.

Predefined format are:

  * def: Default format. Equal to txt,gen,reg,cod,cal
  * all: Full info. Equal to txt,gen,reg,cod,cal,aux,sta

Output format are:

  * txt: Text output (default). Used to export a text file summary.
  * bin: Binary output. Used to export a stripped ELF file.

## EXAMPLES ##
First, you gotta be sure to have the coredump feature compiled in your kernel:
ELF\_CORE should be set to 'y' in kernel .config

`ulimit -c unlimited`

Ask coredump generation when application crashes with no size limit

`cortex < core > cortex.out`

Will extract useful information from core dump file core and output them to cortex.out

`cortex -f def < core > cortex.out`

Will do exactly the same as the previous example.

`cortex -f bin,cod < core > cortex.out`

Will extract the code segment that contains the instruction where the software crashed.

`echo "|cortex >> /var/log/cortex.log" > /proc/sys/kernel/core_pattern`

It is possible to automatically generate cortex output files when any application crashes thanks to the linux kernel:
You will generate text output in the /var/log/cortex.log file each time a coredump is generated

`echo "|cortex -e \"gzip > /var/log/%e_%p.cortex\"" > /proc/sys/kernel/core_pattern`

You can also generate compressed cortex file:
This will create a new file named /var/log/_app_`_`_pid_.cortex that is gzipped and that contains the crash dump.