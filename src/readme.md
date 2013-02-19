## Overview 
This is an implementation of the bash shell, written for my Operating Systems class at UIC in c++. It handles multiple pipelined commands and supports file redirection. 

## Install 
To install run 'make statsh' 
!Only works on linux!

## Usage
The program will keep track of the usage stats for each command run and executed. This is accomplished by using wait4(). By typing 'stats' the user can view a list of previously run commands and their corresponding usage stats (User time, system time). 
Type 'exit' to close the program. 
Background processes can be run by adding a '&' at the end of any command. The execution of the specified program will occur in the background, and each time the user enters a new line on the prompt, they program will check the list of background processes to see if any have finished in which case it will add their usage stats to the history list. 

##Copyright: Michael Cueno @2013
