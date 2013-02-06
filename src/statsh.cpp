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

#define IN 0   // read end
#define OUT 1  // write end

int main(int argc, char** argv){

  string input;                       // Input string for determining how large a char array we will need
  pid_t child;                        // Pid of child process
  vector<history*> history_list;      // Vector of old commands run along with their rusage structs
  int pipefd[2];                      // File descriptors (pipefd[0] = Read end, pipefd[1] = write end)
  int i = 0;                          // index variable for loops
  int cmd_index = 0;              // index for the commands array

  cout << ("statsh ~> ");
  getline(cin, input);    // input now holds the line form stdin cin
  
  while(input.compare("exit")!=0){

    /* Check for stats command from user and show appropriate stats if appropriate */
    if(input.compare("stats")==0){
      printStats(history_list);
    }else{

      int stdout = dup2(1);   // save a copy of stdout for last child 
      /* Create the commands list */
      char* commands[input.length()];
      tokenize(input, commands, "|");

      while(commands[cmd_index]) { // For each command 

        /* create the pipe */
        if (pipe2(pipefd, 0) < 0){
          cout << "pipe failure" << endl;
          exit(0);
        }
        /* So now I have 4 file desciptors, stdin => stdout and pipefd[0] => pipefd[1]
         * The parent needs to be able to send bytes to the child via this descriptor, 
         * thus, the parent replaces stdout with pipefd[OUT] while the child holds pipefd[IN]
         * The child then writes to stdout, this is not good.. We need the child to write to 
         * a new? file descriptor called next possibly and the next child will hold the next[IN]
         */

        /* Create the specific job char array */
        char* tokens[strlen(commands[cmd_index])];
        tokenize(commands[cmd_index], tokens, " ");
      
      
        /* Fork the child to run the command in tockens[0] with arglist tokens */
        if((child = fork())<0){
          cout << "Could not create child process" << endl;
          exit(0) ;
        }

        if(child==0){  // Inside Child process 
          close(pipefd[OUT]); // close write end (child will always write to stdout)
          dup2(pipefd[IN], IN);  // set stdin to read end of pipe
          close(pipefd[IN]);
        
          if(execvp(tokens[0], tokens) <= 0){
            cout << "Error in Exec" << endl;
            exit(0);
          }
        } 
        /* In the parent */
        close(pipefd[IN]); // close write end (Should take input from stdin (since child writes to stdout))
        dup2(pipefd[OUT],OUT); // set stdout to write end of file descriptor 
        close(pipefd[OUT]);


        int *status = new int;
        struct rusage usage;
        wait4(child, status, 0, &usage);
        
        history *record = new history;
        record->usage_stats = usage;
        record->proc_name = tokens[0];
        history_list.push_back(record);

      cmd_index++;
      }

    }
    /* Program loop */
    cmd_index = 0;
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
