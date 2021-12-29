//Variant 22
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_FILE_NAME_LENGTH 30
#define MAX_STR_LENGTH 128
#define MAX_RAND 10000

int main(){
	srand(time(NULL));

	int fd1;
	int fd2;

	char *fname1 = malloc(MAX_FILE_NAME_LENGTH);
	read(0, fname1, MAX_FILE_NAME_LENGTH - 1);
	fname1[strlen(fname1) - 1] = '\0';

	if ((fd1 = open(fname1, O_CREAT | O_WRONLY, S_IRWXU)) < 0){
		perror("File open/creation error");
		exit(-1);
	}

	char *fname2 = malloc(MAX_FILE_NAME_LENGTH);
	read(0, fname2, MAX_FILE_NAME_LENGTH - 1);
	fname2[strlen(fname2) - 1] = '\0';

	if ((fd2 = open(fname2, O_CREAT | O_WRONLY, S_IRWXU)) < 0){
		perror("File open/creation error");
		exit(-1);
	}

	int pipe1[2];
	int pipe2[2];
	pipe(pipe1);
	pipe(pipe2);

	int id1 = fork();
	int id2;

	if (id1 == -1){
		perror("Fork error");
		return -1;
	}
	else if(id1 == 0){ //first child
		close(pipe1[1]);
		if(dup2(fd1, 1) < 0){
			perror("Dup problem fd");
			return -2;
		}
		if(dup2(pipe1[0], 0) < 0){
			perror("Dup problem pipe1[0]");
			return -2;
		}
		if(execl("child", "", NULL) == -1){
			perror("Execl problem");
			return -3;
		}
	}
	else{ //parent
		if ((id2 = fork()) == 0){ //second child
			close(pipe2[1]);
			if(dup2(fd2, 1) < 0){
				perror("Dup problem fd");
				return -2;
			}
			if(dup2(pipe2[0], 0) < 0){
				perror("Dup problem pipe2[0]");
				return -2;
			}
			if(execl("child", "", NULL) == -1){
				perror("Execl problem");
				return -3;
			}
		}
		else if (id2 == -1){
			perror("Fork error");
			return -1;
		}
		else{
			bool eof = false;
			char symb;
			for(;;){
				char *str = malloc(MAX_STR_LENGTH);
				int i = 0;
				symb = ' ';
				for(;;i++){
					if (read(0, &symb, sizeof(char)) <= 0){
						if (strlen(str) != 0)
							str[i] = '\n';
						eof = true;
						break;
					}
					str[i] = symb;
					if (symb == '\n'){
						break;
					}
				}
				int r = rand() % (MAX_RAND + 1);
				int n = strlen(str);
				if (r < 0.8 * MAX_RAND){
					write(pipe1[1], &n, sizeof(int));
					write(pipe1[1], str, sizeof(char) * n);
				}
				else{
					write(pipe2[1], &n, sizeof(int));
					write(pipe2[1], str, sizeof(char) * n);
				}
				free(str);
				if(eof)
					break;
			}
			close(pipe1[0]);
			close(pipe1[1]);
			close(pipe2[0]);
			close(pipe2[1]);
		}
	}
}
