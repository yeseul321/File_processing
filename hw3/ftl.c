#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "blockmap.h"

int free_pbn;
int other_pbn=0;
int addmt[BLOCKS_PER_DEVICE];

int dd_read(int ppn, char *pagebuf);
int dd_write(int ppn, char *pagebuf);
int dd_erase(int pbn);

typedef struct
{
	int lbn;
	int lsn;
	int pbn;
	char something[SPARE_SIZE-12];
} Spare;


void ftl_open()
{
	int tf;
	char *page_buf;
	Spare *spare;
	int lbn;

	page_buf = (char*)malloc(PAGE_SIZE);
	spare = (Spare*)malloc(SPARE_SIZE);


	for (int i = 0; i < BLOCKS_PER_DEVICE; i++) { 
		dd_read(i*PAGES_PER_BLOCK,page_buf);

		memcpy(spare,page_buf+SECTOR_SIZE,SPARE_SIZE);

		if (spare->lbn == 0xff) { 
			tf = 0;
			break;
		}
	}
	if (tf == 0) { //first open
		for (int i = 0; i < BLOCKS_PER_DEVICE; i++) {
			addmt[i] = -1;
		}
		 free_pbn = BLOCKS_PER_DEVICE-1;

	}
	else { //back-up
		
		for (int i = 0; i < DATAPAGES_PER_DEVICE; i++)
		{
			dd_read(i, page_buf);
			memcpy(spare,page_buf+SECTOR_SIZE,SPARE_SIZE);

			if (spare->lbn == 0xff)
			{
				continue;
			}
			else{
				memcpy(spare, page_buf+SECTOR_SIZE, SPARE_SIZE);
				lbn = spare->lbn;
				addmt[lbn] = spare->pbn;
			}
		}
		for(int i=0;i<BLOCKS_PER_DEVICE;i++){

                        if(addmt[i] == free_pbn){
                                free_pbn--;
                                i=-1;
                        }

	}

	return;

	}
	}

void ftl_read(int lsn, char *sectorbuf)
{
	int lbn, offset, ppn;
	char *pagebuf;
	pagebuf = (char *)malloc(PAGE_SIZE);

	lbn = lsn / PAGES_PER_BLOCK;
	offset = lsn % PAGES_PER_BLOCK;
	ppn = (addmt[lbn] * PAGES_PER_BLOCK) + offset;

	dd_read(ppn, pagebuf);
	memcpy(sectorbuf, pagebuf, SECTOR_SIZE);

	free(pagebuf);

	return;
}

void ftl_write(int lsn, char *sectorbuf)
{
	char *pagebuf;
	Spare *spare;

	int	lbn;
	int	offset;
	int	ppn;
	int	pbn;

	lbn = lsn / PAGES_PER_BLOCK;
	offset = lsn % PAGES_PER_BLOCK;
	pagebuf = (char*)malloc(PAGE_SIZE);
	spare = (Spare *)malloc(SPARE_SIZE);


	if(addmt[lbn]<0){
		addmt[lbn] = free_pbn;
	}
	pbn = addmt[lbn];
	ppn = pbn*PAGES_PER_BLOCK + offset;

	dd_read(ppn,pagebuf);
	//get spare data
	memcpy(spare,pagebuf + SECTOR_SIZE, SPARE_SIZE);

	if(spare->lsn < 0){ //first write

		spare->lsn = lsn;
                spare->lbn = lbn;
                spare->pbn = pbn;

		memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
		memcpy(pagebuf + SECTOR_SIZE,spare,SPARE_SIZE);

		dd_write(ppn, pagebuf);
		
		for(int i=0;i<BLOCKS_PER_DEVICE;i++){

			if(addmt[i] == free_pbn){
				free_pbn--;
				i=-1;	
			}
		}
		
	}
	else //not first write.
	{
		for(int i=0;i<PAGES_PER_BLOCK;i++)
		{
			ppn = pbn * PAGES_PER_BLOCK + i;
			dd_read(ppn,pagebuf);
			memcpy(spare, pagebuf + SECTOR_SIZE, SPARE_SIZE);

			if(spare->lsn>=0)
			{
				if(spare->lsn == lsn)
				{
					memcpy(pagebuf, sectorbuf, SECTOR_SIZE);
					spare->lbn = lbn;
                        		spare->lsn = lsn;
                       			spare->pbn = pbn;

					memcpy(pagebuf+SECTOR_SIZE, spare, SPARE_SIZE);
				}
			}
			ppn = free_pbn*PAGES_PER_BLOCK + i;
			dd_write(ppn,pagebuf);
		}

			int erase_pbn = addmt[lbn];
			addmt[lbn] = free_pbn;
			free_pbn = erase_pbn;

			dd_erase(free_pbn);
	}
	free(pagebuf);
	free(spare);

	return;
}

void ftl_print()
{
	printf("lbn pbn\n");
	for (int i = 0; i < BLOCKS_PER_DEVICE; ++i)
		printf(" %d   %d\n", i, addmt[i]);
	printf("free block's pbn = %d\n", free_pbn);
	return;
}
