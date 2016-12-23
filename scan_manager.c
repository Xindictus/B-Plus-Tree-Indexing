#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scan_manager.h"
#include "defn.h"

Scan_info ** ArrayOfScans;

int InsertScan(Scan_info ** ArrayOfScans,int maxscans, int filedesc,int op,void * value,char type_value,int size_key,char type_rec,int size_rec)
{

	int i;
	for (i = 0; i < maxscans; ++i)
	{
		if(ArrayOfScans[i] == NULL)											//kanoume eisagwgh sthn thesi i
		{

			ArrayOfScans[i] = (Scan_info*)malloc(sizeof(Scan_info));		//desmeysh xwrou gia thn thesi i tou pinaka

			ArrayOfScans[i]->fd = filedesc;									//kratame to anagnoristiko tou arxeiou

			ArrayOfScans[i]->oper = op;										//kratame ton telesth op

			ArrayOfScans[i]->type = type_value;								//kratame ton typo tou kleidiou

			ArrayOfScans[i]->size_of_key=size_key;							//kratame to size tou kleidiou

			ArrayOfScans[i]->type_of_record=type_rec;						//kratame ton typo tou record

			ArrayOfScans[i]->size_of_record=size_rec;						//kratame to size tou record

			if(type_value == STRING){												//an to kleidi einai string

				ArrayOfScans[i]->key = (char*)malloc(strlen(value) + 1);	//desmeyoume xwro gia to string

				strncpy(ArrayOfScans[i]->key, value, strlen(value) + 1);	//to antigrafoume

			}

			if(type_value == FLOAT)	{											//ama to kleidi einai typou float
				ArrayOfScans[i]->key=value;
			}
			if(type_value == INTEGER)												//ama to kleidi einai typou int
				ArrayOfScans[i]->key = value;

			ArrayOfScans[i]->DescLastEntry = 0;                                //anagnwristiko teleytaias eggrafhs
			ArrayOfScans[i]->first = 0;
			ArrayOfScans[i]->flag = 0;
			
			return i; 														//epistrefoume thn thesi tou pinaka

		}
	}
	return -1;
}

int Delete_Scan(Scan_info ** ArrayOfScans , int position)
{
	if(ArrayOfScans[position]!=NULL)
	{
		if(ArrayOfScans[position]->type == STRING)
			free(ArrayOfScans[position]->key);

		free(ArrayOfScans[position]);

		ArrayOfScans[position] = NULL;

		return 0;
	}

	return -1;
}

int ArrayOfScansIsFull(Scan_info ** ArrayOfScans ,int maxscans){

	int i;
	for (i = 0; i < maxscans; ++i)
	{
		if(ArrayOfScans[i] == NULL)
			return -1;
	}

	return 0;
}

int ArrayOfScansIsEmpty(Scan_info ** ArrayOfScans ,int maxscans){

	int i;
	for (i = 0; i < maxscans; ++i)
	{
		if(ArrayOfScans[i] != NULL)
			return -1;
	}

	return 0;
}


int OpenScansForThisFiledesc(Scan_info ** ArrayOfScans , int fileDesc , int maxscans){

	int i;
	for (i = 0; i < maxscans; ++i)
	{
		if(ArrayOfScans[i]!=NULL)
		{
			if(ArrayOfScans[i]->fd == fileDesc)
			return 0 ;
		}
	}

	return -1;
}

void free_scan_array(Scan_info ** ArrayOfScans,int maxscans)
{
	int i;
    for(i = 0; i < maxscans; i++)
    {
        if(ArrayOfScans[i] != NULL)
        {
        	if(ArrayOfScans[i]->type == STRING)
            	free(ArrayOfScans[i]->key);
            free(ArrayOfScans[i]);
            ArrayOfScans[i] = NULL;
        }
    }
    free(ArrayOfScans);
}