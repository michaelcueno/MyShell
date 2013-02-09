/**
 * This class models everything that has to do with a single command
 */

class Command{
private:
	char* argv[]; 			// argv for execvp()
	int fd[2]; 				// File descritors 
	int next; 				// Next file descriptor 
	int pid;				// Child process ID
public:
	Command();

	/**
	 * Creates the pipeline and replaces stdin and stdout with respective ends of the pipe
	 */
	void connect_pipes();
	void redirect_to_file();
	void redirect_from_file();
	int get_next_fd();
	void launch();
}