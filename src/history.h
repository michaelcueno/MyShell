#ifndef _HISTORY_H
#define _HISTORY_H

#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <vector>
#include <string>
using namespace std;


class History {
public:
    class Node {
      string name;
      int pid; 
      struct rusage usage_stats;
    public:
        Node(char* name, struct rusage usage){
            this->name = string(name);
            this->usage_stats = usage;
        }
        string getName(){return this->name;}
        struct rusage getUsage(){return this->usage_stats;}
    };
private:
    vector<Node> history_list;
public:
    History();
    ~History();
    void add(Node node);
    void print_stats();
};

#endif
