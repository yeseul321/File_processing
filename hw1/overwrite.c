#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[])
{
	FILE *fp;
	long int offset;
	int buf;

	fp = fopen(argv[1],"r+");
	if(fp==NULL){
		printf("FILE ERROR");
		return -1;
	}

	offset = atol(argv[2]);
	printf("%ld\n", offset);

	buf = strlen(argv[3]);
	printf("%d\n", buf);

	fseek(fp, offset, SEEK_SET);

	fwrite(argv[3], 1, buf, fp);

	fclose(fp);


	return 0;
}
