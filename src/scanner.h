/** Scanner class for grabbing input from user via command line
 * @RETURNS char** an array of null terminated strings, indexed by the delimiter 
 * Uses cin and cout
 *
 * copyright: Michael Cueno 
 * contact: mcueno2@uic.edu
 **/
#include <iostream>
#include <string>
using namespace std; 

class Scanner{
  public: 
    Scanner();
    ~Scanner();
    char** getLine();
    void setDelimiter(char d);

  private: 
    char delimiter;
    int sizeNeeded(string s);
};
