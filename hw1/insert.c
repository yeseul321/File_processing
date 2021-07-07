#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
	FILE *fp;
	long int offset;
	int buf_num;
	char* buf[999];
	int count;

	fp = fopen(argv[1], "r+");
	if(fp==NULL)
	{
		printf("FILE ERROR!");
		return -1;
	}
	offset = atol(argv[2]);
	buf_num = strlen(argv[3]);

	fseek(fp, offset, SEEK_SET);

	while(feof(fp)==0)
	{
		count = fread(buf, sizeof(char), 999, fp);
	}

	fseek(fp, offset, SEEK_SET);
	fwrite(argv[3], sizeof(char), buf_num, fp);
	fwrite(buf, sizeof(char), count, fp);

	fclose(fp);

	return 0;
}

