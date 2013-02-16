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

void launch_command(int in, int out, char* command, History* hist){
  /* Create the arg list for exec */
  dup2(in, IN);                       // Replace stdin with in
  dup2(out, OUT);                     // Replace stdout with out 
  close(in); close(out);
  char* argv[strlen(command)];
  tokenize(command, argv, " ");

  /* Fork of a proccess and exec */
  int pid;
  if((pid = fork())<0){ printf("%s", "Couldn't fork"); }
  if(pid==0){ // in child
    if(execvp(argv[0], argv)<0){
      printf("%s", "Error in exec()\n");
      exit(0);
    }
  }
  /* Wait for child to exit and record usage stats */
  wait_for(pid, argv[0], hist);
}

void wait_for(int pid, char* name, History* hist){
  int status;
  struct rusage usage;
  wait4(pid, &status, 0, &usage);
  History::Node node(name, usage);
  hist->add(node);
}

// cat < in_file > out_file 
void redirect(char** input, int* in, int* out){
  char* first_command = input[0];             
  char* last_command = input[num_commands(input)];
  char output_file[strlen(last_command)];
  char input_file[strlen(first_command)];
  bool redirect_in = false; bool redirect_out = false;

  if(first_command==last_command){            // Must do the output redirection first to maintain input's integrity
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
  }

  int size = strlen(first_command);
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
  else if(redirect_out && (*out = open(output_file, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR )) < 0 ){
    fprintf(stderr, "Could not open file\n");
    *out = dup(OUT);
  }
}

void parse_and_exec(char* input, History* hist){
  if(!input || strlen(input)<=0){ return; }   // error check
  static char* commands[ARG_MAX];             // Holds all commands seperated by pipeline symbol 
  tokenize(input, commands, "|");
  int in = dup(IN);
  int out = dup(OUT);
  redirect(commands, &in, &out);             // Returns input file descriptor or stdin
  launch_pipeline(in, out, commands, hist);
}

void launch_pipeline(int in, int out, char** commands, History* hist){
  int fd[2];                     // Pipeline file descriptors 
  int next = dup(in);            // Holds the output from the last child for input into the next child 
  int cmd_index = 0;             // For indexing into the commands array
  int pid[ARG_MAX];              // Child process IDs
  char* names[ARG_MAX];          // For holding onto the process names (ex: 'ls', 'grep'...)
  while(commands[cmd_index]){
    if(!commands[cmd_index+1]){  // This is the last command in the pipeline 
      launch_command(next, out, commands[cmd_index], hist); 
      cmd_index++; continue;
    }
    char* argv[strlen(commands[cmd_index])];
    tokenize(commands[cmd_index], argv, " ");
    names[cmd_index] = argv[0]; 
    pipe(fd);
    if((pid[cmd_index]=fork())<0){
      fprintf(stderr, "Could not fork!");
    }else if(pid[cmd_index]==0){ // Inside child code
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
    for(int i=0; !pid[i]; i++){ // wait for pids
      wait_for(pid[i], names[i], hist);
    }
}

void tokenize(char* input, char** c, const char* delim){
  /* Create the argument list of null terminated char arrays */
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
    // iterate over commands 
  }
  return i-1; 
}

void print_char(char** x){
  printf("%s", "NOW PRINTING CHARACTER ARRAY:") ;
  for(int i = 0 ; x[i]; i++){
    printf("%s", x[i]);
  }
}
