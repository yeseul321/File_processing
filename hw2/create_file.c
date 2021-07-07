#include<stdio.h>
#include<string.h>
#include<stdlib.h>

typedef struct studentInfo
{
	char univ[10];
	char major[20];
	char stdid[10];
	char name[10];
	char addr[100];
	char other[100];
}Student;


int main(int argc, char **argv){
	FILE *fp = fopen(argv[2], "wb+");
	//char* rcdnum[4];
	int stunum = atoi(argv[1]);
	Student *student;

	if(fp<0){
		fprintf(stderr,"open error for %s\n", argv[2]);
		return -1;
	}

	//fprintf(fp, "%d\n", stunum);

	student = (Student*)malloc(sizeof(Student)*stunum);

	for(int i=0;i<stunum;i++){
		strcpy(student[i].univ,"soongsil");
		strcpy(student[i].major,"computer");
		strcpy(student[i].stdid,"12345678");
		strcpy(student[i].name,"KIM SSU");
		strcpy(student[i].addr,"seoul");
		strcpy(student[i].other,"hello i am SSU");
	}

	//rcdnum[] = argv[1];

	fwrite(&stunum,1,sizeof(int),fp);
	fclose(fp);

	fp = fopen(argv[2], "a+");
	if(fp<0){
                fprintf(stderr,"open error for %s\n", argv[2]);
                return -1;
        }

	fwrite(student,250,stunum,fp);
	//fseek(fp,0L,SEEK_SET);
	free(student);
	fclose(fp);
	return 0;
}
