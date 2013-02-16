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
#include <fcntl.h>
using namespace std;

int main(int argc, char** argv){

  char file[30]; strcpy(file, "Makefile\0");
  char file_name[30];
  strncpy(file_name, file, strlen(file)-1);

  int thing =  open(file, O_RDONLY);

  cout << thing << endl;

  return 0;
}
