/** Main class for the statsh program 
 * Developed by: Michael Cueno 
 * copyright @2013 Michael Cueno 
 * Contanct: mcueno2@uic.edu
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

void printStats(vector<history*>);
char** tokenize(string input, char** tokens, const char* delim);         // Returns an array of null terminated null pointers
void printChar(char** x);

#define READ 0
#define WRITE 1

int main(int argc, char** argv){

  string input;                       // Input string for determining how large a char array we will need
  pid_t child;                        // Pid of child process
  int fd[2];                      // File descriptors 
  int cmd_index = 0; 

  cout << ("statsh ~> ");
  getline(cin, input);    // input now holds the line form stdin cin

  while(input.compare("exit")!=0){

    /* Create the commands list */
    char* commands[input.length()];
    tokenize(input, commands, "|");

    char* prog1[strlen(commands[0])];
    tokenize(commands[cmd_index], prog1, " ");
  
    /* create the pipe */
    if (pipe2(fd, 0) < 0){
      cout << "pipe failure" << endl;
      exit(0);
    }

    /* Fork the child to run the command in tockens[0] with arglist tokens */
    if((child = fork())<0){
      cout << "Could not create child process" << endl;
      exit(0);
    }

    if(child==0){  // Inside Child process 
      close(fd[READ]);
      dup2(fd[WRITE], WRITE); // write to write end of pipe
      close(fd[WRITE]);
    
      if(execvp(prog1[0], prog1) <= 0){
        cout << "Error in Exec" << endl;
        exit(0);
      }
    
    }else {   // Inside parent process
      close(fd[WRITE]);

      char* prog2[strlen(commands[1])];
      tokenize(commands[1], prog2, " ");
      waitpid(child, 0,0);

      int child2 = fork();
      if(child2==0){ /// next child 
      dup2(fd[READ], READ);
      close(fd[READ]);
      close(fd[WRITE]);
      dup2(WRITE, WRITE);  // else this is the last and write to stdout 
      }
      if(execvp(prog2[0], prog2) <= 0){
        cout << "error in exec2" << endl;
        exit(0);
      }
      waitpid(child2,0,0);
      close(fd[READ]); close(fd[WRITE]);


    }


    /* Program loop */
    cout << ("statsh ~> ");

    getline(cin, input);    // input now holds the line form stdin cin
  }

  return 0;
} /* End of main */



/** Helper method to print out stats */
void printStats(vector<history*> history_list){
      cout << "Process Name: User time | System time" << endl;
      for(int i = 0; i<history_list.size();i++){
        cout << history_list[i]->proc_name << ": " ;
        cout << history_list[i]->usage_stats.ru_utime.tv_sec + (history_list[i]->usage_stats.ru_utime.tv_usec / 1000000.0);
        cout << " | ";
        cout << history_list[i]->usage_stats.ru_utime.tv_sec + (history_list[i]->usage_stats.ru_utime.tv_usec / 1000000.0);
        cout << endl;
      }
}

char** tokenize(string input, char** tokens, const char* delim){
  char* cstyle = new char[input.length()+1];  
  strcpy(cstyle, input.c_str());    // put input into a char array

  /* Create the argument list of null terminated char arrays */
  int i =0;
  tokens[i] = strtok(cstyle, delim);
  while(tokens[i]){
    i++;
    tokens[i] = strtok(NULL, delim);
  }
  return tokens;
}


void printChar(char** x){
  int i = 0;
  while(x[i]){
    int j = 0;
    while(x[i][j]){
      cout << x[i][j]; 
    }
    cout << endl;
  }
}
