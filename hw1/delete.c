#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	long int offset;
	long int d_byte;
	char *buf[offset];
	char *buf2[999];
	int count;

	fp = fopen(argv[1], "r+");
	if(fp==NULL){
		printf("FILE ERROR!");
		return -1;
	}

	offset = atol(argv[2]);
	d_byte = atol(argv[3]);

	fread(buf, sizeof(char), offset, fp);

	fseek(fp, offset+d_byte, SEEK_SET);

	count = fread(buf2, sizeof(char), 999, fp);

	printf("%d\n", count);

	fclose(fp);

	//second try

	fp = fopen(argv[1], "w");
	fwrite(buf, sizeof(char), offset, fp);
	fclose(fp);

	fp = fopen(argv[1], "a");
	fwrite(buf2, sizeof(char), count, fp);


	fclose(fp);

	return 0;
}



