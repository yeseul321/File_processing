#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "person.h"


#define MAX_RECORD (HEADER_AREA_SIZE-4)/8


char recordbuf[MAX_RECORD_SIZE];
char pagebuf[PAGE_SIZE];
int header_buf[4];
int totalpagenum = 1;
int totalrecordnum = 0;
int headerpage = -1, headerrecord = -1;
int totalrecordlen;


int readPage(FILE *fp, char *pagebuf, int pagenum)
{
	fseek(fp, PAGE_SIZE*pagenum + 16, SEEK_SET);
	if (fread(pagebuf, PAGE_SIZE, 1, fp)==0)
		return 0;
	else
		return 1;
}


int writePage(FILE *fp, const char *pagebuf, int pagenum)
{
	fseek(fp, PAGE_SIZE*pagenum + 16, SEEK_SET);
	fwrite(pagebuf, PAGE_SIZE, 1, fp);

	return 1;
}

void pack(char *recordbuf, const Person *p)
{
	int len=0;
	sprintf(recordbuf, "%s#", p->id);
	len += strlen(p->id)+1;
	sprintf(recordbuf+len, "%s#", p->name);
	len += strlen(p->name)+1;
	sprintf(recordbuf+len, "%s#", p->age);
	len += strlen(p->age)+1;
	sprintf(recordbuf+len, "%s#", p->addr);
	len += strlen(p->addr)+1;
	sprintf(recordbuf+len, "%s#", p->phone);
	len += strlen(p->phone)+1;
	sprintf(recordbuf+len, "%s#", p->email);
}

void unpack(const char *recordbuf, Person *p)
{
	char *temp;
	temp = (char *)malloc(strlen(recordbuf) + 1);
	strcpy(temp, recordbuf);

	char *ptr = strtok(temp, "#");

	if(ptr == NULL || recordbuf[0] == '*'){
		memset(p, 0xFF, sizeof(Person));
		return;
	}

	strcpy(p->id, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->name, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->age, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->addr, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->phone, ptr);
	ptr = strtok(NULL, "#");
	strcpy(p->email, ptr);
	ptr = strtok(NULL, "#");

	free(temp);

}


void add(FILE *fp, const Person *p)
{
	int newheaderpage, newheaderrecord;

	int records;
	int beforelen = 0;
	int off, b_len;
	int n_len;

	int tf=0;


	memset(pagebuf, 0xFF, PAGE_SIZE);
	pack(recordbuf, p);

	n_len = strlen(recordbuf);

	if(headerpage == -1){
		tf=1;
	}
	else{
		readPage(fp,pagebuf,headerpage);
		memcpy(&b_len,pagebuf+8+headerrecord*8,sizeof(int));
		if(n_len>b_len)
			tf=2;
	}
	int cpage = headerpage;
	int crecord = headerrecord;
	int bpage, brecord;

	if(tf == 2){
		while(cpage!=-1&&crecord!=-1){
			bpage = cpage;
			brecord = crecord;

			memset(pagebuf, 0xFF, PAGE_SIZE);
			readPage(fp, pagebuf, cpage);

			memcpy(&off, pagebuf+4+crecord*8,sizeof(int));
			memcpy(&cpage,pagebuf+HEADER_AREA_SIZE+off+1, sizeof(int));
			memcpy(&crecord, pagebuf+HEADER_AREA_SIZE+off+5, sizeof(int));
		
			printf("cpage : %d crecord : %d\n", cpage, crecord);
			memset(pagebuf, 0xFF, PAGE_SIZE);
			readPage(fp, pagebuf, cpage);

			memcpy(&b_len,pagebuf+8+crecord*8,sizeof(int));
			printf("n_len : %d b_len : %d\n",n_len, b_len);
	
	                if(n_len <= b_len){
				memcpy(&off, pagebuf+4+crecord*8, sizeof(int));
				int a = cpage;

				memcpy(&cpage, pagebuf+HEADER_AREA_SIZE+off+1, sizeof(int));
				memcpy(&crecord, pagebuf+HEADER_AREA_SIZE+off+5, sizeof(int));
				memcpy(pagebuf+HEADER_AREA_SIZE+off, recordbuf, n_len);
				writePage(fp, pagebuf, a);

				readPage(fp, pagebuf, bpage);
				memcpy(&off, pagebuf+4+brecord*8, sizeof(int));
				memcpy(pagebuf+HEADER_AREA_SIZE+off+1, &cpage, sizeof(int));
				memcpy(pagebuf+HEADER_AREA_SIZE+off+5, &crecord, sizeof(int));
				writePage(fp, pagebuf, bpage);
				tf=3;
				break;
			}
		}
		if(tf!=3)
			tf=1;

	}

	else if(tf == 1){
		memset(pagebuf,0xFF, PAGE_SIZE);
  	        off = 0;
	        b_len = 0;

		if(readPage(fp,pagebuf,totalpagenum-1)==0){ //No DATA
			records = 0;
			totalrecordlen = 0;

		}
		else{
			memcpy(&records, pagebuf, sizeof(int));
			memcpy(&off, pagebuf+4+(records-1)*8,sizeof(int));
			memcpy(&b_len,pagebuf+8+(records-1)*8,sizeof(int));

		}

		totalrecordlen = off + b_len;

		if(records == MAX_RECORD || totalrecordlen + strlen(recordbuf) > DATA_AREA_SIZE)
		{
			memset(pagebuf, 0xFF, PAGE_SIZE);
			totalpagenum++;

			records = 0;
			totalrecordlen = 0;
			beforelen = 0;
		}

		records++;
		totalrecordnum++;
		int len = strlen(recordbuf);
		beforelen = totalrecordlen;
		totalrecordlen = totalrecordlen + len;

		memcpy(pagebuf, &records, sizeof(int));
		memcpy(pagebuf + 4 + (records-1) * 8, &beforelen, sizeof(int));
		memcpy(pagebuf + 8 +(records-1) * 8, &len, sizeof(int));

		memcpy(pagebuf + HEADER_AREA_SIZE + beforelen, recordbuf, len);
		writePage(fp, pagebuf, totalpagenum-1);
	}
	else if(tf==0){
		memset(pagebuf,0xFF, PAGE_SIZE);

		int r_len = strlen(recordbuf);
		readPage(fp, pagebuf, headerpage);
		memcpy(&off, pagebuf+4+(headerrecord)*8,sizeof(int));

		newheaderpage = pagebuf[HEADER_AREA_SIZE+off+1];
		newheaderrecord = pagebuf[HEADER_AREA_SIZE+off+5];

		memcpy(pagebuf+off+HEADER_AREA_SIZE, recordbuf, r_len);
		writePage(fp, pagebuf, headerpage);
		headerpage = newheaderpage;
		headerrecord = newheaderrecord;
	}


	memset(header_buf, 0xFF, 16);
	memcpy(&header_buf[0], &totalpagenum, sizeof(int));
	memcpy(&header_buf[1], &totalrecordnum, sizeof(int));
	memcpy(&header_buf[2], &headerpage, sizeof(int));
	memcpy(&header_buf[3], &headerrecord, sizeof(int));

	fseek(fp, 0, SEEK_SET);
	fwrite(header_buf,sizeof(int), 4, fp);

}

void delete(FILE *fp, const char *id)
{
	Person p;
	int ret = 0;
	int i=0,j=0;
	int off = 0, len = 0;

	for(i=0; i<totalpagenum; i++){
		memset(pagebuf, 0xFF, PAGE_SIZE);
		readPage(fp, pagebuf, i);

		for(j=0; j<MAX_RECORD; j++){
			memcpy(&off, pagebuf+4+j*8,sizeof(int));
	                memcpy(&len,pagebuf+8+j*8,sizeof(int));
			memset(recordbuf, 0xFF, len);
			memcpy(recordbuf, pagebuf+off+HEADER_AREA_SIZE, len);
			unpack(recordbuf, &p);
			
			if(strcmp(p.id, id)==0){
				ret = 1;
				break;
			}
		}
		if(ret == 1)
			break;
	}

	if(ret == 0){
		printf("that file is not exists.\n");
		return;
	}

	memcpy(recordbuf, "*", 1);
	memcpy(recordbuf+1, &headerpage, sizeof(int));
	memcpy(recordbuf+5, &headerrecord, sizeof(int));

	memset(pagebuf, 0xFF, PAGE_SIZE);
	readPage(fp, pagebuf, i);
	memcpy(pagebuf+off+HEADER_AREA_SIZE, recordbuf, len);
	writePage(fp, pagebuf, i);

	headerpage = i;
	headerrecord = j;

	memset(header_buf, 0xFF, PAGE_SIZE);
	memcpy(&header_buf[0], &totalpagenum, sizeof(int));
	memcpy(&header_buf[1], &totalrecordnum, sizeof(int));
	memcpy(&header_buf[2], &headerpage, sizeof(int));
	memcpy(&header_buf[3], &headerrecord, sizeof(int));

	fseek(fp, 0, SEEK_SET);
	fwrite(header_buf, sizeof(int), 4, fp);

}

int main(int argc, char *argv[])
{
	FILE *fp; 
	Person p;
	
	memset(pagebuf, 0xFF, PAGE_SIZE);
	memset(header_buf,0,8);
	memset(header_buf+8, 0xFF, 8);

	if(access(argv[2], F_OK) < 0){
		fp = fopen(argv[2], "w+");
		memcpy(&header_buf[0], &totalpagenum, sizeof(int));
		memcpy(&header_buf[1], &totalrecordnum, sizeof(int));
		memcpy(&header_buf[2], &headerpage, sizeof(int));
		memcpy(&header_buf[3], &headerrecord, sizeof(int));
		
		fwrite(header_buf, sizeof(int), 4, fp);
	}
	else{
		fp = fopen(argv[2], "r+");
		fread(header_buf, sizeof(int), 4, fp);
		totalpagenum = header_buf[0];
		totalrecordnum = header_buf[1];
		headerpage = header_buf[2];
		headerrecord = header_buf[3];
	}

	if(argc < 3){
		fprintf(stderr, "usage : %s <option> <record file name> <field value>\n", argv[0]);
		return 0;
	}

	if(!strcmp(argv[1], "a")){
		if(argc < 8){
			fprintf(stderr, "please input all value list\n");
			return 0;
		}

		strcpy(p.id, argv[3]);
		strcpy(p.name, argv[4]);
		strcpy(p.age, argv[5]);
		strcpy(p.addr, argv[6]);
		strcpy(p.phone, argv[7]);
		strcpy(p.email, argv[8]);

		add(fp, &p);
	}
	else if(!strcmp(argv[1], "d")){
		delete(fp, argv[3]);
	}
	else
		printf("Check your option\n");

	return 1;
}

