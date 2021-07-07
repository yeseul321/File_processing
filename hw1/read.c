#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[]){
	FILE *rfp;

        rfp = fopen(argv[1],"r");

	if(rfp==NULL){
                printf("FILE ERROR!!");
                return -1;
        }
	
	long int offset = atol(argv[2]);
	
	int rbyte = atoi(argv[3]);

	fseek(rfp, offset, SEEK_SET);

	char* buf;
	int count = rbyte/sizeof(char);
	buf = (char*)malloc(sizeof(char)*count);

	fread(buf, sizeof(char), count, rfp);
	fwrite(buf, sizeof(char), count, stdout);

	fclose(rfp);
	free(buf);
	printf("\n");

	return 0;
}

