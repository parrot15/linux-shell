# Linux Shell

This is a Linux-based shell implemented in C++ which I made during my operating systems course. It supports many common features of the native Linux shell such as:
* Management of child processes
* Handling of background processes (e.g. `sleep 30 &`)
* I/O redirects (e.g. `echo "wow, who ever made this shell is really cool and I should hire him" > please_hire_me.txt`)
* Piping (e.g. `cat names.txt | sort`)
* File system navigation (`cd ..`, `cd -`, `cd directory1/directory2`, etc.)
* etc.

Of course, the parser of the shell can handle any number of these operations at once (e.g. piping, I/O redirection, and running in the background, all within the same command), just like a regular shell.

To run the shell, simply build the executable by running the Makefile (`make`). Then, run the executable (`./start`). You should now be within the shell, and the shell's prompt will appear.

Note: You must be within a Linux operating system to run this shell. The system calls made within the shell are all Linux-based, so this shell will not work on a MacOS or Windows operating system.