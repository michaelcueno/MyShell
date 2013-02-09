#include "history.h"

History::History(){
}
History::~History(){
}

void History::add(Node node){
    history_list.push_back(node);
}

void History:: print_stats(){
  for(int i = 0; i<(int)history_list.size();i++){
    double user_time =  history_list[i].getUsage().ru_utime.tv_sec + (history_list[i].getUsage().ru_utime.tv_usec / 1000000.0);
    double sys_time = history_list[i].getUsage().ru_utime.tv_sec + (history_list[i].getUsage().ru_utime.tv_usec / 1000000.0);
    string name = history_list[i].getName();
    printf("%s : User Time: %f | System Time: %f\n", name.c_str(), user_time, sys_time);
  }
}
