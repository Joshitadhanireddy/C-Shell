**My Custom Shell**


This project is a custom shell written in C, designed to provide a simplified yet powerful command-line interface. It is my OSN (Operating Systems & Networks) mini-project-1.

**To Compile**

make

**To Run**

./myprogram

**Commands**

*hop*
 
 to hop from directory to directory

 supports ..  , ~ , - , etc. (as well as no arguments and multiple arguments simultaneously)

 *log*

log -> displays our shell_log

log execute -> executes a particular command from shell_log

log purge -> clears shell_log

*reveal*

flags -> -a , -l (and any combination of them)

displays all files in specified/current directory with/without specific info

*seek*

flags -> -e , -f, -d

to get files whose name begins with a certain prefix

 *proclore*
 
 to get info about any process

 with no arguments, prints info of shell

 *aliases*

 create your own aliases for commands

 mention them in .myshrc before running shell
 (cannot be edited dynamically)

 Piping and redirection cannot be included within alias itself!

i.e.

allowed :

alias home = echo "hello"
command : home | wc

not allowed :

 alias home = echo "hello" | wc 

*piping and redirection*

Piping -> |

Redirection -> > , <, >>

Piping with background processes will consider the command directly preceeding it as a background process and not the whole pipe!

For custom commands (hop, reveal, etc.) , only output redirection is allowed!

*activities*

prints all bg processes(currently running / stopped)

Also prints processes that have been terminated by SIGTERM! 

*ping*

sends a signal (modulo 32) to a process w given pid

*ctrl-c*

Interrupts foreground process

*ctrl-z*

Pushes foreground process to background and stops it

*ctrl-d*

Kills all processes in shell and logs out

*fg*

brings bg process(running/stopped) to foreground and gives it control

*bg*

changes stopped bg process to running

*neonate*

w -n and time

prints the most recently created process pid (in system) every time seconds

stops when x key is pressed

*iMan*

for all linux bash commands, gives the man page for any particular command 

**Additional commands and info**

This shell supports background and foreground processes for shell commands (sleep, echo, etc.)

This shell supports foreground processes only for custom functions!

you can also combine commands with ;

Use & at the end of command to run background processes


**General Assumptions**

Assuming absolute path length/username length/filename length etc. of maximum 4096 characters for all functionalities.

Maximum of 100 aliases only can be made.

Maximum of 15 log entries(after which it replaces existing entries).

Maximum of 10 pipes in a single command

Maximum of 100 bg processes running/stopped in a single session (for activities())

Entries in reveal : 1024 * 256

Erroneous log commands are also stored!





