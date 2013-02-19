/**
 * Copyright: Michael Cueno @ 2013  (mcueno2)
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
#include <fcntl.h>
#include "history.h"
using namespace std;

/* Helper defines for remembering which constant is read/write */
#define CMD_MAX 100 // Assuming no one will type more than 100 piped commands at once.
#define IN 0   // read end
#define OUT 1  // write end

struct proc {   // Struct for keeping a pid and it's executable name together
	string name; 
	int pid; 
};

/** returns a char* with the prompt text (userid ~>) */
char* prompt();

/** Reads a line from stdin after printing out prompt, returns a char* of input line */
char* read_line(char* prompt);

/** Takes in an argument vector (array of char pointers) and execs argv[0] with arguments argv
 * Returns pid of forked proccess or -1 if failed 
 *   @param in: the file descriptor that the command should use as it's input
 */
int launch_command(int in, int out, char** argv);

/** Parses input and determines if the first command redirects from a file 
 * or if the last command redirects to an output file. 
 * Returns: void, sets params in and out to be the file descriptor of the redirection
 * files if they exist, or stdin / stdout if no redirection */
void redirect(char** input, int* in, int* out);

/** This function should be run prior to issuing a new prompt, it checks the list of background
	processes to see if the status has changed in any of them by using wait() with the WNOHANG option.
	If a process has fininished, it is removed from the @param background list and added to the 
	@param hist History list. 
  */
void check_background(vector<proc>* background, History* hist);

/** Parses commands and checks if the last command has a & at the end, if it does, method returns true
  * Otherwise returns false; 
  */ 
bool is_background(char** commands); 

/** 'Bootstrap' for the program. Commands is a char pointer to programs seperated by the '|' symbol.
 *  Functions will be forked and piped to eachother if they are seperated by a '|' symbol.
 *  This function will change stdin and stdout depending on needs of redirection, be sure to reset
 *  stdin and stdout before issuing a new prompt!
 */
void parse_and_exec(char* commands, History* hist, vector<proc>* background);

/** Loops on each piped command, creating a pipeline for each command, and pipeing the output of the 
   previous into the input of the next. The last command is redirected to stdout or a file if specified. 
   This function also waits for all childeren and puts usage stats in hist. */
/* IMPORTANT! Commands is a static pointer array and must be freed! */
void launch_pipeline(int in, int out, char** commands, History* hist, vector<proc>* background, bool is_bg);

/** Waits for the specifed pid and adds the usage stats to hist */
void wait_for(int pid, char* name, History* hist); 

/* check for statsh built-in commands like stats, or exit. Returns true if built in fuction was entered */
bool built_in_command(char* line, History* hist);

/* Takes char* input from user and returns a char* array of arguments seperated by @param delim.
 * char* input will be changed! Function can only be called once on any input argument! 
 *  @param c: char** where the tokenized input will be stored */
void tokenize(char* input, char** c, const char* delim);

/* Returns the number of valid pointers in the char** input array */
int num_commands(char** input); 

/* Helper test method for printing the arg array */
void print_char(char** x);

/* Matches the first argument from stdin with char* x. Returns true if they are equal*/
bool match(char** commands, char* x);

#endif
