#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include<sys/time.h>


#define SUFFLE_NUM	10000

void GenRecordSequence(int *list, int n);
void swap(int *a, int *b);

int main(int argc, char **argv)
{
	int *read_order_list;
	int num_of_records; 
	int diff;
	FILE *fp;
	char buf[250];
	struct timeval startTime, endTime;

	fp = fopen(argv[1],"rb");
	if(fp<0)
		fprintf(fp, "FILE OPEN ERROR!!");

	fread(&num_of_records, 1, sizeof(int),fp);

	read_order_list = (int*)malloc(sizeof(int)*num_of_records);


	GenRecordSequence(read_order_list, num_of_records);


	gettimeofday(&startTime,NULL);

	for(int i=0;i<num_of_records;i++){
		fseek(fp,read_order_list[i]*250,SEEK_SET);
		fread(buf,1,sizeof(buf),fp);
	}

	gettimeofday(&endTime,NULL);

	free(read_order_list);
	fclose(fp);

	diff = ((endTime.tv_sec - startTime.tv_sec)*1000000) + (endTime.tv_usec - startTime.tv_usec);

	printf("#records: %d elapsed_time: %d us\n", num_of_records,diff);

	return 0;
}

void GenRecordSequence(int *list, int n)
{
	int i, j, k;

	srand((unsigned int)time(0));

	for(i=0; i<n; i++)
	{
		list[i] = i;
	}
	
	for(i=0; i<SUFFLE_NUM; i++)
	{
		j = rand() % n;
		k = rand() % n;
		swap(&list[j], &list[k]);
	}
}

void swap(int *a, int *b)
{
	int tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;

}
