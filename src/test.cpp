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
using namespace std;

int main(int argc, char** argv){

  char* test[3];
  char ls[3]; strcpy(ls, "ls\0");
  char p[3]; strcpy(p, "..\0");
  test[0] = ls;
  test[1] = p;

  execvp(test[0], test);

  return 0;
}
