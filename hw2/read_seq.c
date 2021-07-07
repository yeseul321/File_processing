#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
	struct timeval startTime, endTime;
	struct stat file_info;
	int diff;
	int bufnum;
	int rcdnum;

	char buf[250];

	FILE *fp = fopen(argv[1],"rb");
	if(fp<0){
		fprintf(stderr,"open error for %s\n", argv[1]);
		return -1;
	}
	stat(argv[1],&file_info);
	bufnum = file_info.st_size/250;

	fread(&rcdnum, 1, sizeof(int),fp);

	gettimeofday(&startTime, NULL);

	for(int i=0;i<bufnum;i++){
		fread(buf,250,1,fp);
	}

	gettimeofday(&endTime,NULL);

	diff = ((endTime.tv_sec - startTime.tv_sec)*1000000) + (endTime.tv_usec - startTime.tv_usec);

	printf("#records: %d elapsed_time: %d us\n",rcdnum,diff);

	fclose(fp);



	return 0;
}
