#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "B+tree.h"
#include "scan_manager.h"
#include "defn.h"
#include "Stack.h"
#include "AM.h"

int compare_two_keys(char type_of_key, void * myvalue_of_key, void * Indexvalue_of_key , int op)
{
	//sygkrish anamesa se <= kai >   (kanones vivliou gia kleidia b+  Ki<=K<K(i+1) )
	if(op == 5)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key) <= 0)
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 <= k2)
                    return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);

                if(k1 <= k2)
                    return 1;
            }

		}
	}

	if(op == 3)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key) < 0)
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 < k2)
                    return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);

                if(k1 < k2)
                    return 1;
            }

		}
	}
	if(op == 4)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key) > 0)
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 > k2)
                	return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);
                if(k1 > k2)
                	return 1;
            }
		}
	}
	if(op == 6)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key) >= 0)
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 >= k2)
                	return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);
                if(k1 >= k2)
                	return 1;
            }
		}
	}
	if(op == 1)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key) == 0)
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 == k2)
                	return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);
                if(k1 == k2)
                	return 1;
            }
		}
	}
	if(op == 2)
	{
		if(type_of_key == STRING)
		{
			if(strcmp(myvalue_of_key,Indexvalue_of_key))
				return 1;
		}
		else
		{
            if(type_of_key ==INTEGER)
            {
                int k1 = *((int*) myvalue_of_key);
                int k2 = *((int*) Indexvalue_of_key);
                if(k1 != k2)
                	return 1;
            }
            else
            {
                float k1 = *((float*) myvalue_of_key);
                float k2 = *((float*) Indexvalue_of_key);
                if(k1 != k2)
                	return 1;
            }
		}
	}
	return 0;
}

void * found_value(int scanDesc,void * block,int i)
{
	void * data;
	//typwnw an isxyoun oi synthikes
	if(ArrayOfScans[scanDesc]->type_of_record == STRING)  //an h egrafh einai typou char*
    {
        data = (char*)malloc(ArrayOfScans[scanDesc]->size_of_record*sizeof(char));
        memcpy(data,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record)+ArrayOfScans[scanDesc]->size_of_key*sizeof(char),ArrayOfScans[scanDesc]->size_of_record*sizeof(char));

        return (char*)data;
    }
    else if(ArrayOfScans[scanDesc]->type_of_record == FLOAT) //egrafh einai  float
    {
        memcpy(&data,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record)+ArrayOfScans[scanDesc]->size_of_key*sizeof(char),sizeof(float));
        float f = *(float*)&data;
        return &f;
    }
    else{
        memcpy(&data,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record)+ArrayOfScans[scanDesc]->size_of_key*sizeof(char),sizeof(int));
        int i = *(int*)&data;
        return &i;//*(int*)&data;
    }
}

void * get_contition_entries_for_less(int scanDesc,void * block,void * temp_adress)
{
	void * value_of_key;
	int Num_of_Record_data,i;
	/* tupwnw apo to 1o block mexri kai to block prin ftasw sto block pou vrisketai h eggrafh */

    int nextBlock;
    memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
    while(block!=temp_adress)//oso exei epomeno
    {
        memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));//diavase posa Record exei
        for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)
            {
                value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
                memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
            {
                memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            if(ArrayOfScans[scanDesc]->type == STRING)
                   free(value_of_key);
         }
         ArrayOfScans[scanDesc]->DescLastEntry=0;
         if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, nextBlock, &block) < 0)               //diavazoume to epomeno block
             return -208;
         ArrayOfScans[scanDesc]->tempBlock=block;
     	 memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
     }

	return NULL;

}

void * get_last_less_entries(int scanDesc,void * block)
{
	void * value_of_key;
    int Num_of_Record_data;
    int flag =0;
    int i=ArrayOfScans[scanDesc]->DescLastEntry;
    /*typwnw to teleytaio block pou isxuei h synthikh < */
    memcpy(&Num_of_Record_data,block+sizeof(short)+sizeof(int),sizeof(int));
    while(flag == 0 && i<Num_of_Record_data)
     {
        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            printf("%s\n", ArrayOfScans[scanDesc]->key);
            if(strcmp(value_of_key, ArrayOfScans[scanDesc]->key) < 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
                flag = 1;
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if(compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,LESS_THAN))
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
                flag = 1;
        }
        if(ArrayOfScans[scanDesc]->type == STRING)
                free(value_of_key);
        i++;
    }


    return NULL;
}

void * get_last_less_or_equal_entries(int scanDesc,void * block)
{
	void * value_of_key;
    int Num_of_Record_data;
    int flag =0;
    int i=ArrayOfScans[scanDesc]->DescLastEntry;
    /*typwnw to teleytaio block pou isxuei h synthikh < */
    memcpy(&Num_of_Record_data,block+sizeof(short)+sizeof(int),sizeof(int));
    while(flag == 0 && i<Num_of_Record_data)
     {
        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            if(strcmp(value_of_key, ArrayOfScans[scanDesc]->key) <= 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
                flag = 1;
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if(compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,LESS_THAN_OR_EQUAL))
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
                flag = 1;
        }
        if(ArrayOfScans[scanDesc]->type == STRING)
                free(value_of_key);
        i++;
    }


    return NULL;
}

void * get_first_greater_entries(int scanDesc,void * block,int Num_of_Record_data)
{
    int i=0,j;
    void * value_of_key;


    /*psaxnw sto sygkekrimeno block mexri na vrw megalhteres egrafes*/
    while(ArrayOfScans[scanDesc]->flag == 0 && i<Num_of_Record_data)
    {
        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key) > 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i;
                ArrayOfScans[scanDesc]->flag = 1 ;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if( compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,GREATER_THAN))
			{
				ArrayOfScans[scanDesc]->DescLastEntry = i;
				ArrayOfScans[scanDesc]->flag = 1 ;
			}
        }
        if(ArrayOfScans[scanDesc]->type == STRING)
            free(value_of_key);
        i++;
    }
    /* typwnw ts megaliteres egrafes*/
    for (j = ArrayOfScans[scanDesc]->DescLastEntry; j < Num_of_Record_data; ++j)
    {

        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+j*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key) > 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = j+1;
                return (void*)found_value(scanDesc,block,j);
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+j*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if( compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,GREATER_THAN))
             {
             	ArrayOfScans[scanDesc]->DescLastEntry = j+1;
                return (void*)found_value(scanDesc,block,j);
             }
        }
        if(ArrayOfScans[scanDesc]->type == STRING)
            free(value_of_key);

    }

    return NULL;
}

void * get_first_greater_or_equal_entries(int scanDesc,void * block,int Num_of_Record_data)
{
    int i=0,j;
    void * value_of_key;

    if(ArrayOfScans[scanDesc]->type == STRING)
        value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));

    /*psaxnw sto sygkekrimeno block mexri na vrw megalhteres egrafes*/
    while(ArrayOfScans[scanDesc]->flag == 0 && i<Num_of_Record_data)
    {
        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key) >= 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i;
                ArrayOfScans[scanDesc]->flag = 1 ;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if( compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,GREATER_THAN_OR_EQUAL))
			{
				ArrayOfScans[scanDesc]->DescLastEntry = i;
				ArrayOfScans[scanDesc]->flag = 1 ;
			}
        }
        i++;
    }
    /* typwnw ts megaliteres egrafes*/
    for (j = ArrayOfScans[scanDesc]->DescLastEntry; j < Num_of_Record_data; ++j)
    {

        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+j*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key) >= 0)
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = j+1;
                return (void*)found_value(scanDesc,block,j);
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+j*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
            if( compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,GREATER_THAN_OR_EQUAL))
             {
             	ArrayOfScans[scanDesc]->DescLastEntry = j+1;
                return (void*)found_value(scanDesc,block,j);
             }
        }

    }

    if(ArrayOfScans[scanDesc]->type == STRING)
    {
        free(value_of_key);
    }

    return NULL;
}

void * get_contition_entries_for_greater(int scanDesc,void * block){

	void * value_of_key;
	int Num_of_Record_data,i;
	/* tupwnw apo to 1o block mexri kai to block prin ftasw sto block pou vrisketai h eggrafh */

    int nextBlock;
    memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
    while(nextBlock!=-1)//oso exei epomeno
    {
        memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));//diavase posa Record exei
        for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)
            {

                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
            {

                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return(void*)found_value(scanDesc,block,i);
            }
         }
         ArrayOfScans[scanDesc]->DescLastEntry = 0;
         if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, nextBlock, &block) < 0)               //diavazoume to epomeno block
             return -208;
         ArrayOfScans[scanDesc]->tempBlock=block;
     	 memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
     }
     if(nextBlock == -1)
     {
     	memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));//diavase posa Record exei
        for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)
            {
                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return (void*)found_value(scanDesc,block,i);
            }
            else
            {
                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                return(void*)found_value(scanDesc,block,i);
            }
         }
     }
	return NULL;
}


void * get_contition_entries_for_not_equal(int scanDesc,void * block){

	int Num_of_Record_data,i;
    void * value_of_key;

    if(ArrayOfScans[scanDesc]->type == STRING)
        value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));


	//edw kanw grammikh anazhthsh kai apla den typwnw ayta pou einai isa
    int nextBlock;
    memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
    while(nextBlock != -1)//oso exei epomeno
    {
        memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));//diavase posa Record exei
        for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)
            {
            
                memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
                if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key))
                {
                	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                    return (void*)found_value(scanDesc,block,i);

                }
            }
            else
            {
                memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
                if(compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,NOT_EQUAL))
                {
                	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
					return (void*)found_value(scanDesc,block,i);
                }
            }
         }
         ArrayOfScans[scanDesc]->DescLastEntry = 0;

         if(BF_ReadBlock(ArrayOfScans[scanDesc]->fd, nextBlock, &block) < 0)               //diavazoume to epomeno block
             return -208;
         ArrayOfScans[scanDesc]->tempBlock=block;
         memcpy(&nextBlock,block+sizeof(short),sizeof(int)); //blepo an to block ayto deixnei se epomeno block gia data
     }
     if(nextBlock == -1)
     {
        memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));//diavase posa Record exei
        for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)
            {
                value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
                memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
                if(strcmp(value_of_key,ArrayOfScans[scanDesc]->key))
                {
                	ArrayOfScans[scanDesc] = i+1;
					return (void*)found_value(scanDesc,block,i);
                }
            }
            else
            {
                memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));
                if(compare_two_keys(ArrayOfScans[scanDesc]->type,&value_of_key,ArrayOfScans[scanDesc]->key,NOT_EQUAL))
                {
                	ArrayOfScans[scanDesc]->DescLastEntry = i+1;
					return (void*)found_value(scanDesc,block,i);
                }
            }
         }
     }

    if(ArrayOfScans[scanDesc]->type == STRING)
        free(value_of_key);
	return NULL;
}


int Scan_for_found_value_in_Index(int scanDesc,short bitDesc,void * block)
{
    int Num_of_Index_data,errno,i,position,pointer;
    void * value_of_key;
    //float f= ArrayOfScans[scanDesc]->key;

    if(ArrayOfScans[scanDesc]->type == STRING)
        value_of_key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key);

    while(bitDesc == 0)                                                                 //oso eimaste se komvo
    {
    	memcpy(&Num_of_Index_data,block+sizeof(short),sizeof(int));

        if(Num_of_Index_data == 0)                                                      //den exoume katholou zeygaria
        {
            errno=-11;
            return errno;
        }

        //diavazoume kai elegxoyme gia to prwto kleidi tou komvou-index pou vriskomaste
        if(ArrayOfScans[scanDesc]->type == STRING)                            //an o typos tou kleidiou einai char
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(ArrayOfScans[scanDesc]->type,ArrayOfScans[scanDesc]->key,value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
            {
                memcpy(&pointer,block+sizeof(short)+sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                //diavazw to block pou tha metatopistoume
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                    return -208;

                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                continue;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(ArrayOfScans[scanDesc]->type,ArrayOfScans[scanDesc]->key,&value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
            {
               memcpy(&pointer,block+sizeof(short)+sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
               //diavazw to block pou tha metatopistoume
               if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                   return -208;

                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                continue;
           }
       }

        //diavazoume kai elegxoyme gia to teleytaio kleidi tou komvou-index pou vriskomaste
        if(ArrayOfScans[scanDesc]->type == STRING)                            //an o typos tou kleidiou einai char
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+(Num_of_Index_data -1)*(sizeof(int) + ArrayOfScans[scanDesc]->size_of_key),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(ArrayOfScans[scanDesc]->type,(void*)ArrayOfScans[scanDesc]->key,value_of_key,GREATER_THAN)) //an to kleidi einai <= tou 1ou kleidiou
            {
                memcpy(&pointer,block+sizeof(short)+sizeof(int)+(Num_of_Index_data )*(sizeof(int)+ArrayOfScans[scanDesc]->size_of_key),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                //diavazw to block pou tha metatopistoume
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                    return -208;

                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                continue;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+(Num_of_Index_data -1)*2*sizeof(int),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)

            if(compare_two_keys(ArrayOfScans[scanDesc]->type,ArrayOfScans[scanDesc]->key,&value_of_key,GREATER_THAN)) //an to kleidi einai <= tou 1ou kleidiou
            {
               memcpy(&pointer,block+sizeof(short)+sizeof(int)+(Num_of_Index_data)*2*sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
               //diavazw to block pou tha metatopistoume
               if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                   return -208;

                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                continue;
           }
       }

        //diavazoume kai elegxoume gia ta ypoloipa zeygaria
        for (i = 1; i < Num_of_Index_data ; ++i)
        {
            if(ArrayOfScans[scanDesc]->type == STRING)                            //an o typos tou kleidiou einai char
            {
                memcpy(value_of_key,block+sizeof(short)+sizeof(int)+i*(sizeof(int) + ArrayOfScans[scanDesc]->size_of_key)+sizeof(int),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
                if(compare_two_keys(ArrayOfScans[scanDesc]->type,(void*)ArrayOfScans[scanDesc]->key,value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
                {
                    memcpy(&pointer,block+sizeof(short)+sizeof(int)+i*(sizeof(int)+ArrayOfScans[scanDesc]->size_of_key),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                    //diavazw to block pou tha metatopistoume
                    if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                        return -208;

                    memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                    break;
                }
            }
            else
            {
                memcpy(&value_of_key,block+sizeof(short)+sizeof(int)+i*(sizeof(int) + ArrayOfScans[scanDesc]->size_of_key)+sizeof(int),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
                if(compare_two_keys(ArrayOfScans[scanDesc]->type,ArrayOfScans[scanDesc]->key,&value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
                {
                   memcpy(&pointer,block+sizeof(short)+sizeof(int)+i*(sizeof(int)+ArrayOfScans[scanDesc]->size_of_key),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                   //diavazw to block pou tha metatopistoume
                   if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd,pointer, &block) < 0)
                       return -208;

                    memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                    break;
               }
           }
        }
    }
    if(ArrayOfScans[scanDesc]->type == STRING)
       free(value_of_key);

    return pointer;
}

int Scan_for_found_value_with_stack(Stack * stackptr,int fd,char type,int size,void * value,short bitDesc,void * block){



	int Num_of_Index_data,errno,i,position,pointer;
	void * value_of_key;

    if(type == STRING) //an o typos kleidiou einai string
        value_of_key = (char*)malloc(size*sizeof(char));

    while(bitDesc == 0)                                                                 //oso eimaste se komvo
    {
    	 memcpy(&Num_of_Index_data,block+sizeof(short),sizeof(int));
         


        if(Num_of_Index_data == 0)                                                      //den exoume katholou zeygaria
        {
            errno=-11;
            return errno;
        }

        //diavazoume kai elegxoyme gia to prwto kleidi tou komvou-index pou vriskomaste
        if(type == STRING)                            //an o typos tou kleidiou einai char
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int),size*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(type,value,value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
            {
                memcpy(&pointer,block+sizeof(short)+sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                
                if(pointer <= 0)
                {
                    printf("diavazw lathos deikth gia epomeno block\n");
                    return -1;
                }
                //diavazw to block pou tha metatopistoume
                if (BF_ReadBlock(fd,pointer, &block) < 0)
                    return -208;
                push(stackptr, pointer);
                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo            
                if(bitDesc == 1)
                    pop(stackptr);
                continue;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(type,value,&value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
            {
               memcpy(&pointer,block+sizeof(short)+sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
 
                if(pointer <= 0)
                {
                    printf("diavazw lathos deikth gia epomeno block\n");
                    return -1;
                }             
               //diavazw to block pou tha metatopistoume
               if (BF_ReadBlock(fd,pointer, &block) < 0)
                   return -208;
                push(stackptr, pointer);
                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                if(bitDesc == 1)
                    pop(stackptr);
                continue;
           }
       }

        //diavazoume kai elegxoyme gia to teleytaio kleidi tou komvou-index pou vriskomaste
        if(type == STRING)                            //an o typos tou kleidiou einai char
        {
            memcpy(value_of_key,block+sizeof(short)+2*sizeof(int)+(Num_of_Index_data-1)*(size+sizeof(int)),size*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
            if(compare_two_keys(type,value,value_of_key,GREATER_THAN)) //an to kleidi einai <= tou 1ou kleidiou
            {
                memcpy(&pointer,block+sizeof(short)+sizeof(int)+(Num_of_Index_data)*(sizeof(int)+size),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
                //diavazw to block pou tha metatopistoume
                
                if(pointer <= 0)
                {
                    printf("diavazw lathos deikth gia epomeno block\n");
                    return -1;
                }
                if (BF_ReadBlock(fd,pointer, &block) < 0)
                    return -208;
                push(stackptr, pointer);
                
            
                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                if(bitDesc == 1)
                    pop(stackptr);
                continue;
            }
        }
        else
        {
            memcpy(&value_of_key,block+sizeof(short)+2*sizeof(int)+(Num_of_Index_data-1)*(2*sizeof(int)),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)

            if(compare_two_keys(type,value,&value_of_key,GREATER_THAN)) //an to kleidi einai <= tou 1ou kleidiou
            {
               memcpy(&pointer,block+sizeof(short)+sizeof(int)+(Num_of_Index_data)*2*sizeof(int),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
               //diavazw to block pou tha metatopistoume
                
                if(pointer <= 0)
                {
                    printf("diavazw lathos deikth gia epomeno block\n");
                    return -1;
                }
                    
               if (BF_ReadBlock(fd,pointer, &block) < 0)
                   return -208;
                push(stackptr, pointer);
                memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                if(bitDesc == 1)
                    pop(stackptr);
                continue;
           }
       }

        //diavazoume kai elegxoume gia ta ypoloipa zeygaria
        for (i = 1; i < Num_of_Index_data ; ++i)
        {
            if(type == STRING)                            //an o typos tou kleidiou einai char
            {
                memcpy(value_of_key,block+sizeof(short)+sizeof(int)+i*(sizeof(int) + size)+sizeof(int),size*sizeof(char));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
                if(compare_two_keys(type,value,value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
                {
                    memcpy(&pointer,block+sizeof(short)+sizeof(int)+i*(sizeof(int)+size),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block

                    if(pointer <= 0)
                    {
                        printf("diavazw lathos deikth gia epomeno block\n");
                        return -1;
                    }
                    //diavazw to block pou tha metatopistoume
                    if (BF_ReadBlock(fd,pointer, &block) < 0)
                        return -208;
                    push(stackptr, pointer);
                    memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                    if(bitDesc == 1)
                        pop(stackptr);
                    break;
                }
            }
            else
            {
                memcpy(&value_of_key,block+sizeof(short)+sizeof(int)+i*(sizeof(int) + size)+sizeof(int),sizeof(int));        //diavazw to kleidi tou 1o zeugariou (deikths-kleidi)
                if(compare_two_keys(type,value,&value_of_key,LESS_THAN_OR_EQUAL)) //an to kleidi einai <= tou 1ou kleidiou
                {
                   memcpy(&pointer,block+sizeof(short)+sizeof(int)+i*(sizeof(int)+size),sizeof(int));//diavazw ton deikth pou deixnei se pio kato block
            
                    if(pointer <= 0)
                    {
                        printf("diavazw lathos deikth gia epomeno block\n");
                        return -1;
                    }
                                   //diavazw to block pou tha metatopistoume
                   if (BF_ReadBlock(fd,pointer, &block) < 0)
                       return -208;
                    push(stackptr, pointer);
                    memcpy(&bitDesc,block,sizeof(short));//elegxw an einai komvos-fyllo
                    if(bitDesc == 1)
                        pop(stackptr);
                    break;
               }
           }
        }
    }
    if(type == STRING)
        free(value_of_key);
   
    return pointer;
}

int Go_to_the_most_left_leafNode(int fd,short bitDesc,void * block)
{
	int pointer;
    while(bitDesc == 0)//oso eimai se komvo eyrethriou
    {
        memcpy(&pointer,block+sizeof(short)+sizeof(int),sizeof(int)); //diavazw apo to 1o zeygari to 1o pointer
        if (BF_ReadBlock(fd, pointer, &block) < 0)//diavazw to block pou deixnei o pointer
            return -208;
        memcpy(&bitDesc,block,sizeof(short));
    }
    return pointer;
}

void * Scan_for_found_value_in_Data(int scanDesc,void * block)
{
	int Num_of_Record_data,i;
	void * key;
	memcpy(&Num_of_Record_data,block+sizeof(int)+sizeof(short),sizeof(int));        //diavazoume poses egrafes exei to block

    key = (char*)malloc(ArrayOfScans[scanDesc]->size_of_key*sizeof(char));

    for (i = ArrayOfScans[scanDesc]->DescLastEntry; i < Num_of_Record_data; ++i)
    {
        //diavazw to kleidi apo ton komvo fyllo
        if(ArrayOfScans[scanDesc]->type == STRING)
        {
            
            memcpy(key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),ArrayOfScans[scanDesc]->size_of_key*sizeof(char));
            //sygkrish Isothtas
            
            if(strcmp(key,ArrayOfScans[scanDesc]->key) == 0)//sygkrinw an vrhka to zhtoumeno
            {
            	ArrayOfScans[scanDesc]->DescLastEntry = i+1;  //thn epomenh fora tha anazhthsh apo thn i+1 eggrafh tou block
                return found_value(scanDesc,block,i);

            }
        }
        else
        {
            memcpy(&key,block+sizeof(short)+2*sizeof(int)+i*(ArrayOfScans[scanDesc]->size_of_key+ArrayOfScans[scanDesc]->size_of_record),sizeof(int));

			if(compare_two_keys(ArrayOfScans[scanDesc]->type,&key,ArrayOfScans[scanDesc]->key,EQUAL))
			{
                ArrayOfScans[scanDesc]->DescLastEntry = i+1;
                    return (void*)found_value(scanDesc,block,i);
			}
        }
    }
        if(ArrayOfScans[scanDesc]->type == STRING)
            free(key);

    return NULL;
}
