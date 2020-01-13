readme.txt

Command Line Interpreter - Cli.c

*Requires Listf.c, Calc.c

How to compile and run via command line:
	Open main program directory and type 'make' on command line
	Enter './Cli' to launch

Cli.c Functions:

*listf <specifier> : A reworking of the UNIX 'ls' command for printing out files and their respective metadata.
	Supported specifiers:
	-l : Include file metadata: File type, Permissions, Links, Owner, Group, Size, modification date
	-lm : Explicity request last modification date: syntax produces same result as -l
	-la : Produces -l with last access date
	-lc : Produces -l with creation date
	-lam : Produces -l with last access date and modification date
	-lac : Produces -l with last access date and creation date//
	-lmc : Produces -l with modification date and creation date
	-lmac : Produces -l with timestamps for last modification, access time and file creation

*calc < %d %c %d > : Supports basic calculations with provided integers and qualified operators:
	Supported Operators:
	/ : Division
	* : Multiplication
	+ : Addition
	- : Subtraction
	^ : Exponential

	To use: Calc function supports arguments passed in from command or you can simply type calc and then input 

*pwd : prints current working directory

*cd : Operates the same as UNIX command
