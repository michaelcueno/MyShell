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

  // Copies of stdin and stdout
  int stdin = dup(IN);
  int stdout = dup(OUT);

  vector<proc> *background = new vector<proc>(); 

  // Main program loop
  while(true){
    // Check for any completed background processes
    check_background(background, hist);

    // Get user input
    char* input = read_line(prompt());

    // Check if user entered 'stats' or 'exit'
    if(!built_in_command(input, hist)){

      parse_and_exec(input, hist, background);

      // Reset for next line input 
      dup2(stdin, IN);
      dup2(stdout, OUT);
    }
    free(input);
  } /* End main loop */
  // Free memory  
  delete hist;
  return 0;
} /* End of main */
