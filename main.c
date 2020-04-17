#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#define BUF_MAX_SIZE 10 // read for one time


/**
 * @brief Open text file & read data per 10 bytes
 * @param int pid
 * @return int
 */
int read_data( int pid){
	int fd = 0;
	ssize_t read_byte = 0;
	char buffer[BUF_MAX_SIZE];
	memset( buffer, 0, sizeof(buffer));

	fd = open( "input.txt", O_RDONLY);
	if( fd < 0){
		printf("Fail to open input.txt\n");
		return fd;
	}

	while(( read_byte = read( fd, buffer, sizeof( buffer) + 1)) > 0){
		printf("pid: %d | %s\n", pid, buffer);
		memset( buffer, 0, sizeof(buffer));
	}

	return pid;
}

/**
 * @brief Process data for using in forked processes
 * @param long pid
 * @return int
 */
int process_data( long pid){
	int rv = 0;
	rv = read_data( pid);
	if( rv < 0){
		return rv;
	}
	return pid;
}

/**
 * @brief 2 child & 1 parent processes for file IO
 * @param int argc
 * @param char** argv
 * @return int
 */
int main( int argc, char **argv)
{
	if( argc != 2){
		printf("Wrong Parameters!\n");
		printf("Usage: ./main (num of child processes)\n");
		exit(-1);
	}

	const int child_process_num = ( const int)( atoi( argv[ 1]));
	int rv = 0, i = 0;
	pid_t pid;
	pid_t child_pids[ child_process_num];

	for( ; i < child_process_num; i++){
		pid = fork();
		if( pid == 0){
			child_pids[ i] = getpid();
			printf("parent: %ld | child start: %ld\n", ( long)( getppid()), ( long)( child_pids[ i]));
			rv = process_data( child_pids[ i]);
			if( rv < 0){
				printf("Fail to process data: %ld\n", ( long)( child_pids[ i]));
				exit( -1);
			}
			sleep( 1);
			exit( i);
		}
		else if( pid == -1){
			perror("FORK FAIL");
			exit(-1);
		}
	}

	if ( pid > 0) {
		pid_t parent_pid = getpid();
		printf("parent start: %ld\n", ( long)( parent_pid));

		rv = process_data( parent_pid);
		if(rv < 0){
			printf("Fail to process data: %ld\n", ( long)( parent_pid));
			exit( -1);
		}

		pid_t child_pid = 0;
		for ( i = 0; i < child_process_num; i++){
			int status = 0;
			child_pid = wait( &status);
			printf("parent: %ld | child finish: %ld | exit status: %d\n", ( long)( parent_pid), ( long)( child_pid), WEXITSTATUS( status));
			status = 0;
		}
	}
}
