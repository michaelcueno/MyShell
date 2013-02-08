/** 
 * Helper methods for the statsh program 
 * Copyright @ Michael Cueno 2013
 */
 #include "statsh.h"

void get_input(char** c){
  string input;
  cout << "statsh ~> ";
  getline(cin, input);  
  c = tokenize(input, c, "|");
}

void printStats(vector<history*> history_list){
  cout << "Process Name: User time | System time" << endl;
  for(int i = 0; i<(int)history_list.size();i++){
    cout << history_list[i]->proc_name << ": " ;
    cout << history_list[i]->usage_stats.ru_utime.tv_sec + (history_list[i]->usage_stats.ru_utime.tv_usec / 1000000.0);
    cout << " | ";
    cout << history_list[i]->usage_stats.ru_utime.tv_sec + (history_list[i]->usage_stats.ru_utime.tv_usec / 1000000.0);
    cout << endl;
  }
}

char** tokenize(string input, char** c, const char* delim){
  char cstyle[input.length()+1];  
  strcpy(cstyle, input.c_str());    // put input into a char array

  /* Create the argument list of null terminated char arrays */
  int i =0;
  c[i] = strtok(cstyle, delim);
  while(c[i]){
    i++;
    c[i] = strtok(NULL, delim);
  }
  return c;
}

bool match(char** commands, string x){
  string input = string(commands[0]);
  if(input.compare(x)==0){
    return true;
  }
  return false;
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

int getSize(char* x){
  return (int)string(x).size();
}