/**
 * Copyright @ Michael Cueno 2013
 *
 * Simple history class implementing a vector of previously run processes in the shell. 
 * Holds a name, pid and usage stats for each process run to be reported whenever
 * the user enters 'stats' into the main program. 
 */
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
        Node(char* name, int pid, struct rusage usage){
            this->name = string(name);
            this->usage_stats = usage;
            this->pid = pid;
        }
        Node(string name, int pid, struct rusage usage){
            this->name = name;
            this->usage_stats = usage; 
            this->pid = pid; 
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
