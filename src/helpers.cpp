/**
 * Helper methods for the statsh program
 * Copyright @ Michael Cueno 2013
 */
#include "helpers.h"
#include "history.h"
#include <sys/time.h>
#include <sys/resource.h>


char* prompt(){
  static char output[100];
  sprintf(output, "statsh~> ");
  return output;
}

bool built_in_command(char* input, History *hist){
  if(strcmp(input, "exit")==0){
    exit(0);
  }
  if(strcmp(input, "stats")==0){
    hist->print_stats();
    return true;
  }
  return false;
}

char* read_line(char* prompt){
  printf("%s",prompt);
  static char* line = NULL;  // Needs to be freed in main!!
  size_t s = 0;
  getline(&line, &s, stdin);
  line[ strlen(line) - 1 ] = '\0'; // get rid of pesky newline character
  return line;
}

int launch_command(int in, int out, char** argv){
  /* Create the arg list for exec */
  dup2(in, IN);                       // Replace stdin with in
  dup2(out, OUT);                     // Replace stdout with out 
  close(in); close(out);

  /* Fork of a process and exec */
  int pid;
  if((pid = fork())<0){ printf("%s", "Couldn't fork"); }
  if(pid==0){ // in child
    if(execvp(argv[0], argv)<0){
      printf("%s", "Error in exec()\n");
      exit(0);
    }
  }
  return pid;
}

void wait_for(int pid, char* name, History* hist){
  int status;
  struct rusage usage;
  wait4(pid, &status, 0, &usage);
  History::Node node(name, pid, usage);
  hist->add(node);
}

void check_background(vector<proc>* background, History* hist){
  vector<proc>::iterator i = background->begin();
  for(; i != background->end() && !background->empty(); ++i){
   struct rusage usage; 
    int status = wait4(i->pid, 0, WNOHANG, &usage);
    if(status<0){ fprintf(stderr, "Something went wrong with a background process"); }
    else if(status==0){                       // Background process not finished yet 
      /* do nothing */
    }else{                                    // Background process finished 
      // Also needs ot add to history 
      History::Node h(i->name, i->pid, usage);
      hist->add(h);
      background->erase(i);
    }
  }
}

// cat < in_file > out_file 
void redirect(char** input, int* in, int* out){
  char* first_command = input[0];             
  char* last_command = input[num_commands(input)];
  char output_file[strlen(last_command)];
  char input_file[strlen(first_command)];
  bool redirect_in = false; bool redirect_out = false;

  /** Parse the last command first because of the case when last_command==first_command
    * In this case, parsing the line with precedence to '<' would make the rest of the line 
    * null and we would loose the '>' redirect if there was one. But since a '>' can only come 
    * after a '<', if we parse for '>' first we will be fine 
    */
  int size = strlen(last_command);          
  for(int i=0; i<size; i++){                  
    if(last_command[i]=='>'){
      redirect_out = true; 
      last_command[i] = '\0';               // Cut out the file name part so it doesn't run as a program 
      i++;
      for(int j=0; i<size+1; i++, j++){     // Need size+1 since we skipped the '>'
        if(last_command[i]==' '){ i++; }    // Skip whitespace
        output_file[j] = last_command[i];   // Copy output filename 
      }
    }
  }

  size = strlen(first_command);
  for(int i=0; i<size; i++){                  // Now walk through input for a '<' redirect
    if(first_command[i]=='<'){
      redirect_in = true;
      first_command[i] = '\0';                // Set to null so that exec only gets the program name and arguments
      i++;                                    // Skip over null byte
      for(int j=0; i<size+1; i++, j++){
        if(first_command[i]==' '){ i++; }     // Skip whitespace (Note, this will turn what should be errors into concatonated file names)
        input_file[j] = first_command[i];   // Copy the contents of first_command after the < symbol into file_name
      }
    }
  }

  if(redirect_in && strlen(input_file)<=0){
    fprintf(stderr, "Syntax error, no file specified\n");
  }
  else if(redirect_in && (*in = open(input_file, O_RDONLY)) < 0 ){
    fprintf(stderr, "Could not open file\n");
    *in = dup(IN);
  }
  if(redirect_out && strlen(output_file)<=0){
    fprintf(stderr, "Syntax error, no file specified\n");
  }
  else if(redirect_out && (*out = open(output_file, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR )) < 0 ){
    fprintf(stderr, "Could not open file\n");
    *out = dup(OUT);
  }
}

void parse_and_exec(char* input, History* hist, vector<proc>* background){
  if(!input || strlen(input)<=0){ return; }   // error check
  static char* commands[ARG_MAX];             // Holds all commands seperated by pipeline symbol 
  tokenize(input, commands, "|");
  int in = dup(IN);
  int out = dup(OUT);
  redirect(commands, &in, &out);             // Returns input file descriptor or stdin
  launch_pipeline(in, out, commands, hist, background);
}

bool is_background(char** commands){
  char* last_command = commands[num_commands(commands)];
  int size = strlen(last_command);
  for(int i = 0; i<size; i++){
    if(last_command[i]=='&'){
      last_command[i] = '\0';
      return true;               // TODO: Should check if & is at the end of the input, for now I think its ok
    }
  }
  return false; 
}

void launch_pipeline(int in, int out, char** commands, History* hist, vector<proc>* background){
  int fd[2];                     // Pipeline file descriptors 
  int next = dup(in);            // Holds the output from the last child for input into the next child 
  int cmd_index = 0;             // For indexing into the commands array
  int pid[ARG_MAX]={0};          // Child process IDs
  char* names[ARG_MAX];          // For holding onto the process names (ex: 'ls', 'grep'...)
  bool backgr = is_background(commands); // set boolean to see if user entered '&' for background process
  while(commands[cmd_index]){
    char* argv[strlen(commands[cmd_index])];
    tokenize(commands[cmd_index], argv, " ");
    names[cmd_index] = argv[0];
    if(!commands[cmd_index+1]){  // This is the last command in the pipeline 
      pid[cmd_index] = launch_command(next, out, argv); 
      cmd_index++; continue;
    }
    pipe(fd);
    if((pid[cmd_index]=fork())<0){
      fprintf(stderr, "Could not fork!");
    }else if(pid[cmd_index]==0){  // Inside child code
      close(fd[IN]);              // Child doesn't need pipe input 
      dup2(next, IN);             // The input should come from the output of the last child 
      dup2(fd[OUT], OUT);         // Child should write to write end of pipe
      close(fd[OUT]); 
      close(next);
      if(execvp(argv[0], argv)){  // If exec returned then something went wrong 
        fprintf(stderr, "Error in exec()\n");
        exit(0);
      }
    }                            // Inside parent code
    close(fd[OUT]);               // Parent doesn't write to pipe
    dup2(fd[IN], next);           // Connect last output to next 
    close(fd[IN]); 
    cmd_index++;
  }

  for(int i=0; pid[i]!=0; i++){    // wait for pids
    if(!backgr)
      wait_for(pid[i], names[i], hist);
    else{
      proc p;
      p.pid = pid[i];
      p.name = string(names[i]);
      background->push_back(p);
      check_background(background, hist);
    }
  }
}


void tokenize(char* input, char** c, const char* delim){
  int i =0;
  c[i] = strtok(input, delim);
  while(c[i]){
    i++;
    c[i] = strtok(NULL, delim);
  }
}

int num_commands(char** input){
  int i; 
  for(i=0; input[i]; i++){
    /* iterate over commands */
  }
  return i-1; 
}

void print_char(char** x){
  printf("%s", "NOW PRINTING CHARACTER ARRAY:") ;
  for(int i = 0 ; x[i]; i++){
    printf("%s", x[i]);
  }
}
