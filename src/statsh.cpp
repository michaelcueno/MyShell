/**
 * Main method for statsh program (linux shell)
 * Copyright @ Michael Cueno 2013
 */
#include "helpers.h"
#include "history.h"

// History list containing commands run and user/sys times for each 
int main(int argc, char** argv){

  // Keeps track of command history 
  History *hist = new History();

  // Copies of stdin and stdout for last command
  int stdin = dup(IN);
  int stdout = dup(OUT);

  vector<int> background; 

  while(true){

    char* input = read_line(prompt());

    // Check if user entered 'stats' or 'exit'
    if(!built_in_command(input, hist)){

      // lets execute one command 
      startPipeline(input, hist, background);

      dup2(stdin, IN);
      dup2(stdout, OUT);

    }

    free(input);
  }


  delete hist;

  return 0;
} /* End of main */
