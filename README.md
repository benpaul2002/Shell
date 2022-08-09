#Shell
The entire program is split into 4 different files - 
    1. func.h
        This file contains the definitions of major functions of the program like cd or ls.
    2. caller.h
        This file contains the functions that are used to call the major functions in func.h. For example, cd_caller() in caller.h calls cd_func() in func.h
    3. user.h
        This file contains some functions that are called frequently in the main program like getting full path.
    4. main.c
        The main file.

This program has path as a global string. path represents the current path in the execution of the program. So, in the beginning it would be same as the program's path. Doing cd or any other path-chaning command will change path accordingly.
Whenever a process is executed in the background, it is added to an array of structs called bg. The struct stores pid, command name, status, and the job number.

Major functions -
    1. cd
        Changes the path global string according to the arguments.
        cd - is implemented using the global string prevpath.
        Before the function is returned, the resulting path directory is opened using opendir. This ensures the validity of the path. If it is an invalid path, then the changes are reverted.
    
    2. pwd
        Displays the present working directory using the path variable.

    3. echo
        Just prints the arguments that were given to it.
    
    4. ls
        Opens the path that is specified in arguments using struct dirent and readdir() function. struct stat is used to output the extra info for ls -l.

    5. pinfo
        Uses the /proc/pid/stat and /proc/pid/exe files to print the info. 
        The 3rd value in /proc/pid/stat gives the process status while the 23rd value gives the virtual memory. The 8th value is used to tell whether the process is running in the foreground or background.
        Doing readlink() on the /proc/pid/exe file gives the path of the process.
        
    6. jobs
		Sorts all the jobs in the bg array alphabetically using a temporary array. It uses /proc/pid/stat file to check if the job's status.
    	
    7. sig
		Sends the specified signal to the specified job number.
    
    8. fg
		Sends the SIGCONT signal to the specified job number and uses tcsetpgrp to bring the job to the foreground. waitpid is also called.
		
	9. bg
		Sends the SIGCONT signal to the specified job number to change its status to running. waitpid is not called here.

Other systems commands were implemented using the execvp() and fork() functions.
        
All the caller functions mainly check for the validity of arguments.

I/O redirection was done using dup() and dup2() functions.
Piping was done using the pipe() function.
