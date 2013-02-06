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
  vector<history*> history_list;      // Vector of old commands run along with their rusage structs
  int pipefd[2];                      // File descriptors 
  int i = 0;                          // index variable for loops
  int cmd_index = 0;              // index for the commands array

  cout << ("statsh ~> ");
  getline(cin, input);    // input now holds the line form stdin cin
  
  while(input.compare("exit")!=0){

    /* Check for stats command from user and show appropriate stats if appropriate */
    if(input.compare("stats")==0){
      printStats(history_list);
    }else{

      /* create the pipe */
      if (pipe2(pipefd, 0) < 0){
        cout << "pipe failure" << endl;
        exit(0);
      }

      /* Create the commands list */
      char* commands[input.length()];
      tokenize(input, commands, "|");

      while(commands[cmd_index]) { // For each command 

        char* tokens[strlen(commands[cmd_index])];
        tokenize(commands[cmd_index], tokens, " ");
      
      
        /* Fork the child to run the command in tockens[0] with arglist tokens */
        if((child = fork())<0){
          cout << "Could not create child process" << endl;
          exit(0);
        }

        if(child==0){  // Inside Child process 
        
          if(execvp(tokens[0], tokens) <= 0){
            cout << "Error in Exec" << endl;
            exit(0);
          }
        
        }else {   // Inside parent process
          int *status = new int;
          struct rusage usage;
          wait4(child, status, 0, &usage);

          history *record = new history;
          record->usage_stats = usage;
          record->proc_name = tokens[0];
          history_list.push_back(record);
        }
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
