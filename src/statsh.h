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

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <vector>
using namespace std;

/* Struct for holding relevant information pretaining to the history of the command run 
 * within the shell for stats reporting when user enters "stats" as a command 
 */
struct history{
  struct rusage usage_stats;
  char* proc_name;
};

/** Prints shell prompt, gets user input, returns char pointer array where each location
	refers to a command. In other words, it seperates input by the '|' pipe symbol and 
	stores the start of each individual command (or job) in a char pointer array */
char** get_input();
/* This function will output the statistics of the current shell (user time/system time) */
void printStats(vector<history*>);

/* Takes string input from user and returns a char* array of arguments seperated by @param delim.
 *  @param c: char** where the tokenized input will be stored */
char** tokenize(string input, char** c, const char* delim); 

/* Helper test method for printing the arg array */ 
void printChar(char** x);

/* Matches the first argument from stdin with string x. Returns true if they are equal*/
bool match(char** commands, string x);

int getSize(char* x);

/* Helper defines for remembering which constant is read/write */
#define IN 0   // read end
#define OUT 1  // write end