#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
	printf("============test1===========\n");
	FILE *fp1, *fp2;
	fp1 = fopen(argv[1],"r");
	fp2 = fopen(argv[2],"w");
	if(fp1==NULL || fp2==NULL){
		printf("ERROR!!\n");
		return 0;
	}
	char buf[10];
	int ret=0;
	while(ret = fread(buf, sizeof(char), sizeof(buf), fp1)){
		fwrite(buf, 1, ret, fp2);
	}


	fclose(fp1);
	fclose(fp2);
	return 0;
}

