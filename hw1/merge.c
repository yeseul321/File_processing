#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *argv[])
{
	FILE *fp1;
	FILE *fp2;
	FILE *fp3;
	char *buf[sizeof(char)*10];
	int ret1, ret2;

	fp1 = fopen(argv[1],"a"); //adding the text from the EOF of the file
	fp2 = fopen(argv[2],"r");
	fp3 = fopen(argv[3],"r");

	if(fp2==NULL||fp3==NULL){
		printf("FILE ERROR");
		return -1;
	}
	while(feof(fp2) == 0){
		ret1 = fread(buf, sizeof(char), sizeof(buf), fp2);
		fwrite(buf, sizeof(char), ret1, fp1);
	}
	while(feof(fp3) == 0){
		ret2 = fread(buf, sizeof(char), sizeof(buf), fp3);
                fwrite(buf, sizeof(char), ret2, fp1);
        }

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);

	printf("success!\n");
	return 0;
}


