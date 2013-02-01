/** Main class for the statsh program 
 * Developed by: Michael Cueno 
 * copyright @2013 Michael Cueno 
 * Contanct: mcueno2@uic.edu
 *
 */

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
using namespace std;

int main(int argc, char** argv){

  string input; 
  pid_t child;

  cout << ("statsh ~> ");
  getline(cin, input);    // input now holds the line form stdin cin
  
  while(input.compare("exit")!=0){
    char* cstyle = new char[input.length()+1];  
    char* tokens[(input.length()+1)/2];
    strcpy(cstyle, input.c_str());    // put input into a char array

    int i =0;

    tokens[i] = strtok(cstyle, " ");

    while(tokens[i]){
      i++;
      tokens[i] = strtok(NULL, " ");
    }

    // Now tokens is a char array of null terminated tokens 
  
    if((child = fork())<0){
      cout << "Could not create child process" << endl;
      exit(0);
    }

    if(child==0){  // Inside Child code 
    
      if(execvp(tokens[0], tokens) <= 0){
        cout << "Error in Exec" << endl;
        exit(0);
      }
    
    }else {
      waitpid(child, 0, 0);
      cout << ("statsh ~> ");
      getline(cin, input);    // input now holds the line form stdin cin
    }
  }

    return 0;
  }
