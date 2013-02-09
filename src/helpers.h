/**
 * Copyright: Michael Cueno @ 2013
 *
 * This is my implementation of a basic linux shell program. It was assigned
 * as a project for my Operating Systems class at UIC. This file contains the
 * prototypes of all functions used in the program (in files statsh.cpp, helpers.cpp)
 * This program uses a Makefile to comile the executable statsh
 *
 * Contact: mcueno2@uic.edu
 */
#ifndef _STATSH_H
#define _STATSH_H

#include <stdio.h>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <vector>
#include "history.h"
using namespace std;

/* Helper defines for remembering which constant is read/write */
#define ARG_MAX 100 // Assuming no one will type more than 100 piped commands at once.
#define IN 0   // read end
#define OUT 1  // write end

/* returns a char* with the prompt text. TODO: implement user@pwd >*/
char* prompt();

/* Reads a line from stdin after printing out prompt, returns a char* of input line */
char* read_line(char* prompt);

/* Takes in an argument vector (array of char pointers) and execs argv[0] with arguments arg
 * The process that this is run in will become the executed command
 * Returns -1 if the exec failed
 * @param in: the file descriptor that the command should use as it's input
 */
void launch_command(int in, int out, char* argv, History* hist);

/* Commands is a char pointer to programs seperated by the '|' symbol. Functions will be forked and pipe to 
 * eachother, make sure to set stdin and stdout once this function runs to get output back to the terminal */
void parse_and_exec(char* commands, History* hist);

/* Loops on each piped command, creating a pipeline for each command, and pipeing the output of the 
   previous into the input of the next. The last command is redirected to stdout or a file if specified. 
   This function also waits for all childeren and puts usage stats in hist. */
/* IMPORTANT! Commands is a static pointer array and must be freed! */
void launch_pipeline(char** commands, History* hist);

/* Waits for the specifed pid and adds the usage stats to hist */
void wait_for(int pid, char* name, History* hist); 

/* check for statsh built-in commands like stats, or exit. Returns true if built in fuction was entered */
bool built_in_command(char* line, History* hist);

/* Takes char* input from user and returns a char* array of arguments seperated by @param delim.
 *  @param c: char** where the tokenized input will be stored */
void tokenize(char* input, char** c, const char* delim);

/* Helper test method for printing the arg array */
void print_char(char** x);

/* Matches the first argument from stdin with char* x. Returns true if they are equal*/
bool match(char** commands, char* x);

int getSize(char* x);


#endif
