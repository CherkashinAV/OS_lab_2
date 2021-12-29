//pipe1[0] = 0;
//fd = 1;
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#define MAX_STR_LENGTH 128

int main(){
	int n;
		while(read(0, &n, sizeof(int)) > 0){
			char *str = malloc(MAX_STR_LENGTH);
			read(0, str, sizeof(char) * n);
			lseek(1, 0, SEEK_END);
			for (int i = 0; i < (strlen(str) - 1) / 2; i++){
				char l_symb = str[i];
				str[i] = str[strlen(str) - i - 2];
				str[strlen(str) - i - 2] = l_symb;
			}
			write(1, str, sizeof(char) * n);
			free(str);
		}
}