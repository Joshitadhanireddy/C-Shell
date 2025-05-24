**My Custom Shell**


This project is a custom shell written in C, designed to provide a simplified yet powerful command-line interface. 

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


**Additional commands and info**

This shell supports background and foreground processes for shell commands (sleep, echo, etc.)

you can also combine commands with ;

Use & to run background processes


**General Assumptions**

Assuming absolute path length/username size/buffer size etc. of maximum 4096 characters for all functionalities

