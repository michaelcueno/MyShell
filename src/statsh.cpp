/**
 * Main method for statsh program (linux shell)
 * Copyright @ Michael Cueno 2013
 */
#include "statsh.h"

int main(int argc, char** argv){

  pid_t child;                        // Pid of child process
  vector<history*> history_list;      // Vector of old commands run along with their rusage structs
  int fd[2];                      // File descriptors (fd[0] = Read end, fd[1] = write end)
  int next;                        // next file descriptor for connecting child processes
  int cmd_index = 0;                  // index for the commands array
  char* commands[100];                    // Holds the input line, indexed by commands seperated by a '|'

  //TODO Change to current user instead of statsh
  // input now holds the line form stdin cin
  get_input(commands);
 
  /* Main program loop */
  while(!match(commands, "exit")){

    /* Check for stats command from user and show appropriate stats if appropriate */
    if(match(commands, "stats")){
      printStats(history_list);
    }else{


      if(pipe2(fd, 0) < 0){
        cout << "Could not create pipe" << endl; exit(0);
      }

      while(commands[cmd_index]) { // For each command 

        /* create the pipe */
        if (pipe2(fd, 0) < 0){
          cout << "pipe failure" << endl; exit(0);
        }
        /* So now I have 4 file desciptors, stdin => stdout and fd[0] => fd[1]
         * The parent needs to be able to send bytes to the child via this descriptor, 
         * thus, the parent replaces stdout with fd[OUT] while the child holds fd[IN]
         * The child then writes to stdout, this is not good.. We need the child to write to 
         * a new? file descriptor called next possibly and the next child will hold the next[IN]
         */

        /* Create the specific job char array */

        char** tokens;
        tokens = tokenize(string(commands[cmd_index]), tokens, " ");
      
      
        /* Fork the child to run the command in tockens[0] with arglist tokens */
        if((child = fork())<0){
          cout << "Could not create child process" << endl;
          exit(0) ;
        }

        if(child==0){  // Inside Child process 
          dup2(fd[IN], IN);  // set stdin to read end of pipe
          dup2(fd[OUT], OUT); // set stdout to write end of pipe
          close(fd[IN]);
          close(fd[OUT]);
        
          if(execvp(tokens[0], tokens) <= 0){
            cout << "Error in Exec" << endl;
            exit(0);
          }
        } 
        /* In the parent */
        close(fd[IN]); // close write end (Should take input from stdin (since child writes to stdout))
        dup2(fd[OUT],OUT); // set stdout to write end of file descriptor 
        close(fd[OUT]);


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
    get_input(commands);
  }

  return 0;
} /* End of main */


