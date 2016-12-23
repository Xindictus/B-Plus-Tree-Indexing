#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BF.h"
#include "defn.h"
#include "file_manager.h"
#include "scan_manager.h"
#include "B+tree.h"
#include "Stack.h"
#include "AM.h"

int AM_errno;

void AM_Init( void )
{
    BF_Init();
    int i;

    AM_errno = 0;

    /***************************************************
    arxikopoioume ton pinaka diaxirisis anixton arxion
    ***************************************************/
    fileManager = malloc(MAXOPENFILES*sizeof(File_info*));
    for(i = 0; i < MAXOPENFILES; i++)
    {
        fileManager[i] = NULL;
    }

    /***************************************************
    arxikopoioume ton pinaka diaxeirishs sarwsewn
    ***************************************************/
    ArrayOfScans=malloc(MAXSCANS*sizeof(Scan_info*));
    for (i = 0 ; i < MAXSCANS ; i++)
    {
        ArrayOfScans[i]=NULL;
    }
}

void AM_Close( void )
{
    /***************************************************
    kanoume free ton pinaka diaxirisis anixton arxion
    ***************************************************/
    free_array(fileManager, MAXOPENFILES);
    free_scan_array(ArrayOfScans,MAXSCANS);
}

int AM_CreateIndex(char *fileName, char attrType1, int attrLength1, char attrType2, int attrLength2)
{
    int fileDesc, blockNo = 0, data_attr1, data_attr2, index_attr1, index_attr2, data_MAX, index_MAX, root_Num = -1;
    char file_characteristic[] = "B+";
    void* block;

    /***************************************************
    Elegxos iparksis arxioy
    ***************************************************/
    if ( (fileDesc = BF_OpenFile(fileName)) >= 0)
    {
        AM_errno = FILE_EXISTS;
        return AME_EOF;
    }

    /***************************************************
    Checking attribute 1
    ***************************************************/
    if( attrType1 == INTEGER || attrType1 == FLOAT )
    {
        if(attrLength1 != 4)
        {
            AM_errno = LEN_ATTR_1;
            return AME_EOF;
        }
        /***************************************************
        GETTING THE SIZE OF ATTR1
        ***************************************************/
        data_attr1 = attrLength1;
        index_attr1 = attrLength1;

    }
    else if ( attrType1 == STRING )
    {
        if(attrLength1 < 1 || attrLength1 > 255)
        {
            AM_errno = LEN_ATTR_1;
            return AME_EOF;
        }
        /***************************************************
        GETTING THE SIZE OF ATTR1
        ***************************************************/
        data_attr1 = sizeof(char)*attrLength1;
        index_attr1 = sizeof(char)*attrLength1;
    }
    else
    {
        AM_errno = TYPE_ATTR_1;
        return AME_EOF;
    }

    /***************************************************
    Checking attribute 2
    ***************************************************/
    if( attrType2 == INTEGER || attrType2 == FLOAT )
    {
        if(attrLength2 != 4)
        {
            AM_errno = LEN_ATTR_2;
            return AME_EOF;
        }
        /***************************************************
        GETTING THE SIZE OF ATTR2
        ***************************************************/
        data_attr2 = attrLength2;
    }
    else if ( attrType2 == STRING )
    {
        if(attrLength2 < 1 || attrLength2 > 255)
        {
            AM_errno = LEN_ATTR_2;
            return AME_EOF;
        }
        /***************************************************
        GETTING THE SIZE OF ATTR2
        ***************************************************/
        data_attr2 = sizeof(char)*attrLength2;
    }
    else
    {
        AM_errno = TYPE_ATTR_2;
        return AME_EOF;
    }
    index_attr2 = sizeof(int);

    /***************************************************
    Dimiourgia arxiou
    ***************************************************/
    if (BF_CreateFile(fileName) < 0)
    {
        AM_errno = FILE_CREATION;
        return AME_EOF;
    }
    /***************************************************
    Anoigma arxiou
    ***************************************************/
    if ( (fileDesc = BF_OpenFile(fileName)) < 0)
    {
        AM_errno = FILE_OPEN;
        return AME_EOF;
    }

    /***************************************************
    Allocation of 1st block
    ***************************************************/
    if (BF_AllocateBlock(fileDesc)<0)
    {
        AM_errno = AL_BLOCK;
        return AME_EOF;
    }

    /***************************************************
    Diavasma 1ou block
    ***************************************************/
    if (BF_ReadBlock(fileDesc, 0, &block) < 0)
    {
        AM_errno = RD_BLOCK;
        return AME_EOF;
    }

    /***************************************************
    Thetoume to xaraktiristiko gia b+ files
    ***************************************************/
    memcpy(block, file_characteristic,sizeof(CHARACTERISTIC_SIZE));

    /***************************************************
    Apothikeuoume ta stoixeia toy record
    ***************************************************/
    memcpy(block+CHARACTERISTIC_SIZE,&attrType1,sizeof(char));
    memcpy(block+CHARACTERISTIC_SIZE+sizeof(char),&attrLength1,sizeof(int));
    memcpy(block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char),&attrType2,sizeof(char));
    memcpy(block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char)*2,&attrLength2,sizeof(int));

    /***************************************************
    Defining max records for each type of block
    ***************************************************/
    data_MAX = (BLOCK_SIZE - sizeof(short) - sizeof(int)*2 - data_attr1- data_attr2)/(data_attr1+data_attr2);
    index_MAX = (BLOCK_SIZE - sizeof(short) - sizeof(int)*2 - index_attr1 - index_attr2)/(index_attr1+index_attr2);

    memcpy(block+CHARACTERISTIC_SIZE+sizeof(int)*2+sizeof(char)*2,&data_MAX,sizeof(int));
    memcpy(block+CHARACTERISTIC_SIZE+sizeof(int)*3+sizeof(char)*2,&index_MAX,sizeof(int));
    memcpy(block+CHARACTERISTIC_SIZE+sizeof(int)*4+sizeof(char)*2,&root_Num,sizeof(int));

    if (BF_WriteBlock(fileDesc,0) < 0)
    {
        AM_errno = WR_BLOCK;
        return AME_EOF;
    }

    if (BF_CloseFile(fileDesc) < 0)
    {
        AM_errno = FILE_CLOSE;
        return AME_EOF;
    }

    printf("\nB+ File Created!\n");
    return AME_OK;
}

int AM_DestroyIndex(char *fileName)
{
    int i, fileDesc;

    /***************************************************
    mia for opou gia kathe fileName ston pinaka, kalei tin closeindex
    ***************************************************/
    while( (fileDesc = delete_open_fileName(fileManager, MAXOPENFILES, fileName)) != -1  )
    {
        if (AM_CloseIndex(fileDesc) < 0)
        {
            AM_errno = FILE_CLOSE;
            return AME_EOF;
        }
    }

    /***************************************************
    remove file from filesystem
    ***************************************************/
    if(remove(fileName) == 0)
    {
        printf("\n%s file deleted successfully.\n", fileName);
        return AME_OK;
    }
    else
    {
        AM_errno = -300;
        AM_PrintError("Error Destroying File\n");
        return AME_EOF;
    }
}

int AM_OpenIndex (char *fileName)
{
    int fileDesc, pos, attrLength1, attrLength2, data_MAX, index_MAX;
    char *file_check, attrType1, attrType2;
    void *block;

    /***************************************************
    elegxoume an ston pinaka exoun anixti idi 20 arxia
    ***************************************************/
    if(count_open_files(fileManager, MAXOPENFILES) == MAXOPENFILES)
    {
        AM_errno = MAXOPENFILES_ERROR;
        return AME_EOF;
    }

    if ( (fileDesc = BF_OpenFile(fileName)) < 0)
    {
        AM_errno = FILE_OPEN;
        return AME_EOF;
    }

    if (BF_ReadBlock(fileDesc, 0, &block) < 0)
    {
        AM_errno = RD_BLOCK;
        return AME_EOF;
    }

    file_check = (char*) malloc (CHARACTERISTIC_SIZE);

    /***************************************************
    elegxoume an to arxio prokeitai gia b+

    an dn einai b+ epistrefoume -1
    ***************************************************/
    memcpy(file_check,block,CHARACTERISTIC_SIZE);
    if(strcmp(file_check, "B+") != 0)
    {
        if (BF_CloseFile(fileDesc) < 0)
        {
            AM_errno = FILE_CLOSE;
            return AME_EOF;
        }
        AM_errno = FILE_TYPE_MISMATCH;
        return AME_EOF;
    }

    memcpy(&attrType1,block+CHARACTERISTIC_SIZE,sizeof(char));
    memcpy(&attrLength1,block+CHARACTERISTIC_SIZE+sizeof(char),sizeof(int));
    memcpy(&attrType2,block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char),sizeof(char));
    memcpy(&attrLength2,block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char)*2,sizeof(int));
    memcpy(&data_MAX,block+CHARACTERISTIC_SIZE+sizeof(int)*2+sizeof(char)*2,sizeof(int));
    memcpy(&index_MAX,block+CHARACTERISTIC_SIZE+sizeof(int)*3+sizeof(char)*2,sizeof(int));

    if((pos = insert_open_file(fileManager, MAXOPENFILES, fileDesc,
                               fileName, attrType1, attrLength1, attrType2,
                               attrLength2, data_MAX, index_MAX)) == -1)
    {
        if (BF_CloseFile(fileDesc) < 0)
        {
            AM_errno = FILE_CLOSE;
            return AME_EOF;
        }
        AM_errno = FILE_MANAGER_INSERT;
        return AME_EOF;
    }

    printf("%s File Opened!\n", fileName);
    free(file_check);
    return pos;
}

int AM_CloseIndex (int fileDesc)
{
    /***************************************************
    an o global pinakas anazhthsewn exei anoixtes
    ***************************************************/
    if(OpenScansForThisFiledesc(ArrayOfScans,fileManager[fileDesc]->fileDesc,MAXSCANS) == 0)
    {
        /***************************************************
        anazhthseis se ayto to filedesc

        kratame to teleytaio sfalma
        gyrna ton kwdiko lathous
        ***************************************************/

        AM_errno=OPEN_SCANS;
        return AME_EOF;
    }
    else
    {
        if(BF_CloseFile(fileManager[fileDesc]->fileDesc) < 0)
        {
            AM_errno = FILE_CLOSE;
            return AME_EOF;
        }
    }
    printf("File Closed!\n");
    return AME_OK;
}

int AM_InsertEntry(int fileDesc, void *value1, void *value2)
{
    int     n, attrLength1, attrLength2, data_MAX, index_MAX,
            it, wpos = 0, match_flag = 0, icmp_val,fd, 
                pleaf, j, pleaf_new, Oldnext, Pnext, new_block = -1,
    
                                                     index_records, finished, root_Num, pindex_new, i, root_block;
    float fcmp_val;
    void * K;
    short block_id, bitDesc;
    char attrType1, attrType2, *scmp_val;
    void *draftBlock, *firstBlock, *temp, *new_data_block, *root;

    /***************************************************
    dhmiourgia stoivas
    ***************************************************/
    Stack * S = malloc(sizeof(Stack));
    S->top = -1;


    /***************************************************
    Diavasma 1ou block
    ***************************************************/

    /***************************************************
    Anaktisi stixion tou arxeiou
    ***************************************************/

    fd=fileManager[fileDesc]->fileDesc;
    attrType1 = fileManager[fileDesc]->attrType1;
    attrLength1 = fileManager[fileDesc]->attrLength1;
    attrType2 = fileManager[fileDesc]->attrType2;
    attrLength2 = fileManager[fileDesc]->attrLength2;
    data_MAX = fileManager[fileDesc]->data_MAX;
    index_MAX = fileManager[fileDesc]->index_MAX;
    index_MAX = 2;


    if (BF_ReadBlock(fd, 0, &firstBlock) < 0)
    {
        AM_errno = RD_BLOCK;
        return AME_EOF;
    }


    /***************************************************
    prepei na vroume se poio block einai h riza
    ***************************************************/
    memcpy(&root_block,firstBlock+CHARACTERISTIC_SIZE+sizeof(int)*4+sizeof(char)*2,sizeof(int));

    /***********************************************************************************************************/
    if(root_block == -2)
    {
        n = BF_GetBlockCounter(fd) - 1;
    }
    else
    {
        if(root_block != -1)
        {
            /***************************************************
            diavazoume to block ths rizas
            ***************************************************/
            if (BF_ReadBlock(fd, root_block, &root) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }

            memcpy(&bitDesc,root,sizeof(short));
            push(S,root_block);

            n=Scan_for_found_value_with_stack(S,fd,attrType1,attrLength1,value1,bitDesc,root);
            if(n == -1)
            	return -1;

        }
        /***************************************************
        den exoume kanei akoma kamia eisagwgh
        ***************************************************/
        else
        {
            /***************************************************
            stoixeia pou tha valoume sto data block
            anagnoristiko data block
            ***************************************************/
            short block_id = 1;
            /***************************************************
            den exei akoma epomeno
            ***************************************************/
            int Pnext = -1;
            /***************************************************
            den exoume akoma eggrafes
            ***************************************************/
            int pleaf_new = 0;

            root_block = -2;

            memcpy(firstBlock+CHARACTERISTIC_SIZE+sizeof(int)*4+sizeof(char)*2,&root_block,sizeof(int));

            if (BF_AllocateBlock(fd)<0)
            {
                AM_errno = AL_BLOCK;
                return AME_EOF;
            }

            if (BF_ReadBlock(fd, BF_GetBlockCounter(fd) - 1, &new_data_block) < 0)
            {
                AM_errno = RD_BLOCK;
                return AME_EOF;
            }

            memcpy(new_data_block,&block_id,sizeof(short));
            memcpy(new_data_block+sizeof(short),&Pnext,sizeof(int));
            memcpy(new_data_block+sizeof(short)+sizeof(int),&pleaf_new,sizeof(int));

            if (BF_WriteBlock(fd,BF_GetBlockCounter(fd) - 1) < 0)
            {
                AM_errno = WR_BLOCK;
                return AME_EOF;
            }
            n = BF_GetBlockCounter(fd) - 1;
        }
    }

    /***************************************************
    Diavasma toy n block
    ***************************************************/
    if (BF_ReadBlock(fd, n, &draftBlock) < 0)
    {
        AM_errno = RD_BLOCK;
        return AME_EOF;
    }
    //data_MAX =2;
    //index_MAX = 2;
    memcpy(&pleaf,draftBlock+sizeof(short)+sizeof(int),sizeof(int));
    if(pleaf != data_MAX)
    {
        if(pleaf != 0)//an o komvos fullo-data den einai adeios  ... valto sortarismeno
        {
            /***************************************************
            SEARCH ACCORDING TO TYPE
            ***************************************************/
            if(attrType1 == INTEGER)
            {
                for(it = 0; it < pleaf; it++)
                {
                    wpos = it;
                    memcpy(&icmp_val,
                           draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                           attrLength1);
                    if( *(int*)value1 <= icmp_val)
                    {
                        match_flag = 1;
                        break;
                    }
                }
            }
            else if(attrType1 == FLOAT)
            {
                for(it = 0; it < pleaf; it++)
                {
                    wpos = it;
                    memcpy(&fcmp_val,
                           draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                           attrLength1);
                    if( *(float*)value1 <= fcmp_val)
                    {
                        match_flag = 1;
                        break;
                    }
                }
            }
            else
            {
                scmp_val = malloc(sizeof(char)*attrLength1);
                for(it = 0; it < pleaf; it++)
                {
                    wpos = it;
                    memcpy(scmp_val,
                           draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                           attrLength1);
                    if(strncmp((char*)value1, scmp_val, attrLength1) <= 0)
                    {
                        match_flag = 1;
                        break;
                    }

                }
                free(scmp_val);
            }
            if(match_flag == 1)
            {
                for(it = pleaf-1; it >= wpos; it--)
                {
                    memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*(it+1),
                           draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                           attrLength1+attrLength2);
                }
            }
            else
            {
                wpos++;
            }
            
	        memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*wpos,value1,attrLength1);
	        memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*wpos+attrLength1,value2,attrLength2);
			

            match_flag = 0;
        }
        else
        {
            memcpy(draftBlock+sizeof(short)+sizeof(int)*2,value1,attrLength1);
	        memcpy(draftBlock+sizeof(short)+sizeof(int)*2+attrLength1,value2,attrLength2);
		}		

        /***************************************************
        GRAFOUME TON NEO ARITHMO EGGRAFON
        ***************************************************/
        pleaf++;
        memcpy(draftBlock+sizeof(short)+sizeof(int),&pleaf,sizeof(int));

        if (BF_WriteBlock(fd,n) < 0)
        {
            AM_errno = WR_BLOCK;
            return AME_EOF;
        }
    }
    else
    {
    	//printf("zeygari : %s    %d\n\n",(char*)value1,*(int*)value2 );
		//getchar();
        temp = draftBlock;
        /***************************************************
        SEARCH ACCORDING TO TYPE
        ***************************************************/
        if(attrType1 == INTEGER)
        {
            for(it = 0; it < pleaf; it++)
            {
                wpos = it;
                memcpy(&icmp_val,
                       temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                       attrLength1);
                if( *(int*)value1 <= icmp_val)
                {
                    match_flag = 1;
                    break;
                }
            }
        }
        else if(attrType1 == FLOAT)
        {
            for(it = 0; it < pleaf; it++)
            {
                wpos = it;
                memcpy(&fcmp_val,
                       temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                       attrLength1);
                if( *(float*)value1 <= fcmp_val)
                {
                    match_flag = 1;
                    break;
                }
            }
        }
        else
        {
            scmp_val = malloc(sizeof(char)*attrLength1);
            for(it = 0; it < pleaf; it++)
            {
                wpos = it;
                memcpy(scmp_val,
                       temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                       attrLength1);
                if(strncmp((char*)value1, scmp_val, attrLength1) <= 0)
                {
                    match_flag = 1;
                    break;
                }

            }
            free(scmp_val);
        }
        if(match_flag == 1)
        {
            for(it = pleaf-1; it >= wpos; it--)
            {
                memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*(it+1),
                       temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*it,
                       attrLength1+attrLength2);
            }
        }
        else
        {
            wpos++;
        }
        memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*wpos,value1,attrLength1);
        memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*wpos+attrLength1,value2,attrLength2);

        match_flag = 0;

        //printf("1o stadio OK:\n copy n sto temp\ninsert ths kataxwrhshs(K,Pr) sth swsth thesh sto temp\n\n");
      //  getchar();
        /***************************************************
        ALLOCATE NEW BLOCK
        ***************************************************/
        if (BF_AllocateBlock(fd)<0)
        {
            AM_errno = AL_BLOCK;
            return AME_EOF;
        }
        new_block = BF_GetBlockCounter(fd) - 1;
        if (BF_ReadBlock(fd, new_block, &new_data_block) < 0)
        {
            AM_errno = RD_BLOCK;
            return AME_EOF;
        }
        /***************************************************
        Plirofories gia to new Data Block
        ***************************************************/
        block_id = 1;
        /***************************************************
        The next from N. We write new block as his next
        ***************************************************/
        memcpy(&Oldnext,draftBlock+sizeof(short), sizeof(int));
        memcpy(draftBlock+sizeof(short),&new_block,sizeof(int));
        /***************************************************
        New block's next becomes draftblock's next
        ***************************************************/
        Pnext = Oldnext;
        j = (pleaf+1)/2;
        j += (pleaf+1)%2;
        pleaf_new = (pleaf +1)- j;

        /***************************************************
        Allagi tou arithmou ton eggrafon ston N
        ***************************************************/
        memcpy(draftBlock+sizeof(short)+sizeof(int), &j, sizeof(int));

        /***************************************************
        Prosthiki pliroforion sto new block
        ***************************************************/
        memcpy(new_data_block, &block_id, sizeof(short));
        memcpy(new_data_block+sizeof(short), &Pnext, sizeof(int));
        memcpy(new_data_block+sizeof(short)+sizeof(int), &pleaf_new, sizeof(int));

        //printf("2o stadio OK:\n new <-- enas neos komvos-fullo gia to dentro\nj<--(pleaf+1)/2\n\n");
       // getchar();

        /***************************************************
        Eisagogi pleaf_new eggrafon
        ***************************************************/
        memcpy(new_data_block+sizeof(short)+sizeof(int)*2,
               temp+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*j,
               (attrLength1+attrLength2)*pleaf_new);

        //printf("3o stadio OK:\n new<-- oi upoloipes kataxwrhseis tou temp\n\n");
       // getchar();
        if(attrType1 == STRING)
        {
        	K=(char*)malloc(sizeof(char)*attrLength1);
        	memcpy(K,
            	   draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*(j-1),
               	   attrLength1);
        }
        else
        {
         	memcpy(&K,
            	   draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+attrLength2)*(j-1),
               	   attrLength1);       	
        }
    

        //printf("4o stadio OK:\n K<--Kj\n\n");
        /***************************************************
        Eggrafi tou new Block
        ***************************************************/
        if (BF_WriteBlock(fd,new_block) < 0)
        {
            AM_errno = WR_BLOCK;
            return AME_EOF;
        }

        /***************************************************
        Eggrafi tou paliou block
        ***************************************************/
        if (BF_WriteBlock(fd,n) < 0)
        {
            AM_errno = WR_BLOCK;
            return AME_EOF;
        }
        finished = 0;
        do
        {
            /***************************************************
            if S einai empty
            ***************************************************/
            if(S->top == -1)
            {
                /***************************************************
                then nea riza
                ***************************************************/
                if (BF_AllocateBlock(fd)<0)
                {
                    AM_errno = AL_BLOCK;
                    return AME_EOF;
                }
                root_Num = BF_GetBlockCounter(fd) - 1;
                
                /***************************************************
                Eggrafi root block sto block 0
                ***************************************************/
                memcpy(firstBlock+CHARACTERISTIC_SIZE+sizeof(int)*4+sizeof(char)*2,&root_Num,sizeof(int));

                if (BF_ReadBlock(fd, root_Num, &root) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return AME_EOF;
                }

                /***************************************************
                Eggrafi stixion gia to index block
                ***************************************************/
                block_id = 0;
                pindex_new = 1;
                memcpy(root, &block_id, sizeof(short));
                memcpy(root+sizeof(short), &pindex_new, sizeof(int));

                //printf("5o stadio OK:\n enas neos eswterikos komvos gia to dentro\n\n");
                //getchar();
                /***************************************************
                Eggrafi index record
                ***************************************************/
                memcpy(root+sizeof(short)+sizeof(int), &n, sizeof(int));
                if(attrType1 == STRING)
                	memcpy(root+sizeof(short)+sizeof(int)*2, K, attrLength1);
                else
                	memcpy(root+sizeof(short)+sizeof(int)*2, &K, attrLength1);
                memcpy(root+sizeof(short)+sizeof(int)*2+attrLength1, &new_block, sizeof(int));
                
                /***************************************************
                Eggrafi root block
                ***************************************************/
                if (BF_WriteBlock(fd,root_Num) < 0)
                {
                    AM_errno = WR_BLOCK;
                    return AME_EOF;
                }

                finished = 1;
                //printf("6o stadio OK:\nroot<--<n,K,new>\n\n");
                //getchar();
            }
            else
            {

                n = pop(S);
     
                //printf("7o stadio OK:\nn<--pop stoiva S\n\n");
                //getchar();
                if (BF_ReadBlock(fd, n, &draftBlock) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return AME_EOF;
                }
                memcpy(&pleaf,draftBlock+sizeof(short),sizeof(int));

                if( pleaf != index_MAX)
                {
                    if(pleaf != 0)
                    {
                        /***************************************************
                        SEARCH ACCORDING TO TYPE
                        ***************************************************/
                        if(attrType1 == INTEGER)
                        {
                            for(it = 0; it < pleaf; it++)
                            {
                                wpos = it;
                                memcpy(&icmp_val,
                                       /*bit*/      /*pleaf*/   /*pointer*/
                                       /*change*/             draftBlock+sizeof(short)+sizeof(int)+sizeof(int)+(attrLength1+sizeof(int))*it,
                                       attrLength1);
                                if( K <= icmp_val)
                                {
                                    match_flag = 1;
                                    break;
                                }
                            }
                        }
                        else if(attrType1 == FLOAT)
                        {
                            for(it = 0; it < pleaf; it++)
                            {
                                wpos = it;
                                memcpy(&fcmp_val,
                                       /*bit*/      /*pleaf*/   /*pointer*/
                                       /*change*/                 draftBlock+sizeof(short)+sizeof(int)+sizeof(int)+(attrLength1+sizeof(int))*it,
                                       attrLength1);
                                if( *(float*)value1 <= fcmp_val)
                                {
                                    match_flag = 1;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            scmp_val = malloc(sizeof(char)*attrLength1);
                            for(it = 0; it < pleaf; it++)
                            {
                                wpos = it;
                                memcpy(scmp_val,
                                       /*bit*/      /*pleaf*/   /*pointer*/
                                       /*change*/            draftBlock+sizeof(short)+sizeof(int)+sizeof(int)+(attrLength1+sizeof(int))*it,
                                       attrLength1);
                                if(strncmp((char*)value1, scmp_val, attrLength1) <= 0)
                                {
                                    match_flag = 1;
                                    break;
                                }

                            }
                            free(scmp_val);
                        }
                        if(match_flag == 1)
                        {
                            for(it = pleaf-1; it >= wpos; it--)
                            {
                                /*change*/          memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*(it+1),
                                                           /*change*/                draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*it,
                                                           attrLength1+attrLength2);
                            }
                        }
                        else
                        {
                            wpos++;
                        }
                        if(attrType1 == STRING)
						{
                        	memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos,K,attrLength1);
							
						}
                        else
                        	memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos,&K,attrLength1);

                        memcpy(draftBlock+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos+attrLength1,&new_block,sizeof(int));

                        match_flag = 0;
                    }
                    /* den mporei na    else
                       isxuei se aythn	{
                    	thn periptwsh		memcpy(draftBlock+sizeof(short)+sizeof(int)*2,value1,attrLength1);
                    (adeio index)			memcpy(draftBlock+sizeof(short)+sizeof(int)*2+attrLength1,value2,attrLength2);
                    			        }
                    */			        /***************************************************
GRAFOUME TON NEO ARITHMO EGGRAFON
***************************************************/
                    pleaf++;
                    memcpy(draftBlock+sizeof(short),&pleaf,sizeof(int));

                    if (BF_WriteBlock(fd,n) < 0)
                    {
                        AM_errno = WR_BLOCK;
                        return AME_EOF;
                    }
                    finished = 1;
                    //printf("8o stadio OK\nIf eswterikos komvos den einai gematos insert (K,new) sto index n\n\n");
                    //getchar();
                }
                else
                {

                    temp = draftBlock;

                    /************************
                    SEARCH ACCORDING TO TYPE
                    *************************/
                    if(attrType1 == INTEGER)
                    {
                        for(it = 0; it < pleaf; it++)
                        {
                            wpos = it;
                            memcpy(&icmp_val,
                                   temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*it,
                                   attrLength1);
                            if( *(int*)value1 <= icmp_val)
                            {
                                match_flag = 1;
                                break;
                            }
                        }
                    }
                    else if(attrType1 == FLOAT)
                    {
                        for(it = 0; it < pleaf; it++)
                        {
                            wpos = it;
                            memcpy(&fcmp_val,
                                   temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*it,
                                   attrLength1);
                            if( *(float*)value1 <= fcmp_val)
                            {
                                match_flag = 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        scmp_val = malloc(sizeof(char)*attrLength1);
                        for(it = 0; it < pleaf; it++)
                        {
                            wpos = it;
                            memcpy(scmp_val,
                                   temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*it,
                                   attrLength1);
                            if(strncmp((char*)value1, scmp_val, attrLength1) <= 0)
                            {
                                match_flag = 1;
                                break;
                            }

                        }
                        free(scmp_val);
                    }
                    if(match_flag == 1)
                    {
                        for(it = pleaf-1; it >= wpos; it--)
                        {
                            memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*(it+1),
                                   temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*it,
                                   attrLength1+sizeof(int));
                        }
                    }
                    else
                    {
                        wpos++;
                    }
                    if(attrType1 != STRING)
                    	memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos,&K,attrLength1);
                    else
                    	memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos,K,attrLength1);

                    memcpy(temp+sizeof(short)+sizeof(int)*2+(attrLength1+sizeof(int))*wpos+attrLength1,&new_block,sizeof(int));

                    match_flag = 0;

                    /********************
                    ALLOCATE NEW BLOCK
                    *********************/
                    if (BF_AllocateBlock(fd)<0)
                    {
                        AM_errno = AL_BLOCK;
                        return AME_EOF;
                    }
                    new_block = BF_GetBlockCounter(fd) - 1;
                    if (BF_ReadBlock(fd, new_block, &new_data_block) < 0)
                    {
                        AM_errno = RD_BLOCK;
                        return AME_EOF;
                    }

                    /**********************************
                    Plirofories gia to new Index Block
                    ***********************************/
                    block_id = 0;
                    j = (pleaf+2)/2; // oxi pleaf alla p(pou shmainei deiktes)				/******CHANGE********/
                    int pleaf_n = j-1;															/******CHANGE********/
                    pleaf_new = (pleaf + 1) - j;

                    /****************************************
                    REMEMBER TO CHANGE INDEX MAX
                    Allagi tou arithmou ton eggrafon ston N
                    ****************************************/
                    memcpy(draftBlock+sizeof(short), &pleaf_n, sizeof(int));					/******CHANGE*******/

                    /*****************************************
                    Prosthiki pliroforion sto new index block
                    ******************************************/
                    memcpy(new_data_block,&block_id,sizeof(short));
                    memcpy(new_data_block+sizeof(short),&pleaf_new,sizeof(int));

                    /***************************
                    Eisagogi pleaf_new eggrafon  ????????????
                    ****************************/
                    memcpy(new_data_block+sizeof(short)+sizeof(int),
        	               temp+sizeof(short)+sizeof(int)+(sizeof(int)+attrLength1)*j,				/******CHANGE*******/
                           (sizeof(int)+attrLength1)*pleaf_new+sizeof(int));

                    /*********
                    K <- Kj
                    **********/
                    if(attrType1 == STRING)
                    	memcpy(K,
                           temp+sizeof(short)+sizeof(int)+(sizeof(int)+attrLength1)*(j-1)+sizeof(int),  /******CHANGE*******/
                           attrLength1);
                    else
                    	memcpy(&K,
                           temp+sizeof(short)+sizeof(int)+(sizeof(int)+attrLength1)*(j-1)+sizeof(int),	/******CHANGE*******/
                           attrLength1);
                    /***************************
                    Eisagogi pleaf_new eggrafon
                    ****************************/
                    if (BF_WriteBlock(fd,new_block) < 0)
                    {
                        AM_errno = WR_BLOCK;
                        return AME_EOF;
                    }

                    /***************************
                    Eggrafi tou paliou block
                    ****************************/
                    if (BF_WriteBlock(fd,n) < 0)
                    {
                        AM_errno = WR_BLOCK;
                        return AME_EOF;
                    }



                    //printf(" new block %d\n",new_block);
                    //getchar();
                }
            }
        }
        while(finished == 0);
    }

    if (BF_WriteBlock(fd,0) < 0)
    {
        AM_errno = WR_BLOCK;
        return AME_EOF;
    }

    /***************************
    diagrafh stoivas
    ****************************/
    if(S!=NULL)
        free(S);
    return 0;
}

int AM_OpenIndexScan(int fileDesc,int op,void *value)
{
    char type_key,type_record;
    int pos,size_key,size_record;
    void * block;

    if((op < 1) || (op > 6))
    {
        /***************************
        error in operaror type
        ****************************/
        AM_errno=WRONG_OPERATOR;
        return AME_EOF;
    }

    /***************************
    diavazoume to block 0 (arxiko)
    ****************************/
    if (BF_ReadBlock(fileManager[fileDesc]->fileDesc, 0, &block) < 0)
    {

        AM_errno = RD_BLOCK;
        return AME_EOF;
    }
    /***************************
    ALLAGI
    ****************************/
    memcpy(&type_key,block+CHARACTERISTIC_SIZE,sizeof(char));   //diavazoume ton typo tou kleidiou
    memcpy(&size_key,block+CHARACTERISTIC_SIZE+sizeof(char),sizeof(int));//diavazoume to megethos tou kleidiou
    memcpy(&type_record,block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char),sizeof(char));//diavazoume ton typo tou record
    memcpy(&size_record,block+CHARACTERISTIC_SIZE+sizeof(int)+sizeof(char)*2,sizeof(int));//size of record

    /********************************************
    an o global pinakas anazhthsewn einai gematos
    *********************************************/
    if(ArrayOfScansIsFull(ArrayOfScans,MAXSCANS) == 0)
    {
        /***************************
        kratame to teleytaio sfalma
        gyrna ton kwdiko lathous
        ****************************/
        AM_errno=FULL_SCAN_ARRAY;
        return AME_EOF;

    }
    else
    {
        /***************************
        eisagoume kainourgia sarwsh
        ****************************/
        pos=InsertScan(ArrayOfScans,MAXSCANS,fileManager[fileDesc]->fileDesc,op,value,type_key,size_key,type_record,size_record);

        if(pos == -1)
        {

            AM_errno = INSERT_SCAN;

            return AME_EOF;
        }
    }

    return pos;
}

void *AM_FindNextEntry(int scanDesc)
{
    /***************************
    dhlwseis metavlhtwn
    ****************************/
    int root_block,i,errno,nextBlock,pointer,flag=-1,temp;
    void *block,*temp_adress,*key;
    void * v;
    /***************************
    0 or 1  index or data
    ****************************/
    short bitDesc;
    int Num_of_Index_data,Num_of_Record_data;

    /********************************************
    diavazoume to 1o block gia na vroume thn riza
    *********************************************/
    if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, 0, &block) < 0)
    {
        AM_errno = RD_BLOCK;
        return NULL;
    }
    /******************************************
    prepei na vroume se poio block einai h riza
    *******************************************/
    memcpy(&root_block,block+CHARACTERISTIC_SIZE+sizeof(int)*4+sizeof(char)*2,sizeof(int));

    /***************************
    diavazoume to block ths rizas
    ****************************/
    //printf("riza = %d\n", root_block);
    if(root_block == -1 || root_block == -2)
    {
        if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, 1, &block) < 0)
        {

            AM_errno = RD_BLOCK;
            return NULL;
        }
    }
    else
    {
        if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, root_block, &block) < 0)
        {

            AM_errno = RD_BLOCK;
            return NULL;
        }
    }
    /******************************************
    diavazw to bit anagnwrishs (komvos h fyllo)
    *******************************************/
    memcpy(&bitDesc,block,sizeof(short));
    /***************************
    periptwshs anazhthshs
    ****************************/

    switch(ArrayOfScans[scanDesc]->oper)
    {
    /******************************************
    eksetazoume thn anazhthsh gia isothta
    *******************************************/
    case 1:
        if(root_block != -1 && root_block != -2)
        {
            errno=Scan_for_found_value_in_Index(scanDesc,bitDesc,block);

            if(errno<0)
            {
                AM_errno=errno;
                return NULL;
            }
            /****************************
            diavazoume to block ths rizas
            *****************************/
            
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
        }
        /***************************
        briskomaste sta dedomena
        ****************************/

        v=Scan_for_found_value_in_Data(scanDesc,block);
        if(v!=NULL)
        	return v;

        break;
    /************************************************************************************************************
    eksetazoume thn anazhthsh gia diaforo....den mporei na ginei kati kalitero apo thn grammikh anazhthsh
    *************************************************************************************************************/
    case 2:
        if(ArrayOfScans[scanDesc]->first == 0)
        {
            if(root_block != -1 && root_block != -2)
            {
                errno = Go_to_the_most_left_leafNode(ArrayOfScans[scanDesc]->fd,bitDesc,block);
                if(errno<0)
                {
                    AM_errno=errno;
                    return NULL;
                }
                /****************************
                diavazoume to block ths rizas
                *****************************/
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return NULL;
                }
                ArrayOfScans[scanDesc]->first = 1;
                ArrayOfScans[scanDesc]->tempBlock = block;
            }
            else
            {
                ArrayOfScans[scanDesc]->first = 1;
                ArrayOfScans[scanDesc]->tempBlock = block;
            }
        }
        v=get_contition_entries_for_not_equal(scanDesc,ArrayOfScans[scanDesc]->tempBlock);
        if(v!=NULL)
        	return v;

        break;
    /*********************************************************************************
    anazhthsh gia mikrotero : twra den tha eksetasoume aparaithta ola ta block
    vhma 1o
    tha vroume mexri kai pio block tha psaksoume
    **********************************************************************************/
    case 3:

        if(root_block != -1 && root_block != -2)
        {
            temp_adress=block;
            errno=Scan_for_found_value_in_Index(scanDesc,bitDesc,temp_adress);
            if(errno < 0)
            {
                AM_errno=errno;
                return NULL;
            }
            /****************************
            diavazoume to block ths rizas
            *****************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &temp_adress) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            temp = errno;
        }
        if(ArrayOfScans[scanDesc]->first == 0 && (root_block != -1 && root_block != -2))
        {
            /******************************************************
            vhma 2
            ksanadiavazoume thn riza
            diavazoume to block ths rizas
            *******************************************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, root_block, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            memcpy(&bitDesc,block,sizeof(short));
            /******************************************************
            vhma 3
            tha pame sto pio aristero komvo-fyllo
            *******************************************************/

            errno = Go_to_the_most_left_leafNode(ArrayOfScans[scanDesc]->fd,bitDesc,block);
            if(errno<0)
            {
                AM_errno=errno;
                return NULL;
            }

            /******************************************************
            diavazoume to block ton pio aristera komvo
            *******************************************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            ArrayOfScans[scanDesc]->first = 1;
            ArrayOfScans[scanDesc]->tempBlock = block;
        }
        if(ArrayOfScans[scanDesc]->first == 1)
        {
            /*********************************************************************************
            vhma 4
            tha pame na psaksoume mexri to block pou exei dieythinsi temp_adress
            **********************************************************************************/
            void * vl = NULL;
            if((vl=get_contition_entries_for_less(scanDesc,ArrayOfScans[scanDesc]->tempBlock,temp_adress)) == NULL)
            {
                ArrayOfScans[scanDesc]->first = -1;
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, temp, &ArrayOfScans[scanDesc]->tempBlock) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return NULL;
                }
            }
            else return vl;
        }
        if(ArrayOfScans[scanDesc]->first == -1)
        {
            /*********************************************************************************
            vhma 5o
            psaxnoume kai to teleytaio block pou tha isxyei h synthikh
            **********************************************************************************/
            return (void*)get_last_less_entries(scanDesc,ArrayOfScans[scanDesc]->tempBlock);
        }
        if(root_block == -1 || root_block == -2)
        {
            v=get_last_less_entries(scanDesc,block);
            if(v!=NULL)
            	return v;
        }


        break;
    /***************************
    anazhthsh gia megalitero
    ****************************/
    case 4:

        if(ArrayOfScans[scanDesc]->first == 0 && (root_block != -1 && root_block != -2))
        {
            errno=Scan_for_found_value_in_Index(scanDesc,bitDesc,block);
            if(errno < 0)
            {
                AM_errno=errno;
                break;
            }

            /****************************
            diavazoume to block ths rizas
            *****************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }

            /******************************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************************/
            memcpy(&Num_of_Record_data,block+sizeof(short)+sizeof(int),sizeof(int));
            memcpy(&nextBlock,block+sizeof(short),sizeof(int));
            if(nextBlock = -1)
                flag = 0;
            ArrayOfScans[scanDesc]->first=1;
            ArrayOfScans[scanDesc]->tempBlock=block;
        }
        if(ArrayOfScans[scanDesc]->first == 1 )
        {
            
            /******************************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************************/
            memcpy(&Num_of_Record_data,ArrayOfScans[scanDesc]->tempBlock+sizeof(short)+sizeof(int),sizeof(int));
            /******************************************************
            vriskoume ta megalitera se auto to block
            *******************************************************/
            v = get_first_greater_entries(scanDesc,ArrayOfScans[scanDesc]->tempBlock,Num_of_Record_data);
            if (v != NULL)
            {
                return v;
            }
            else
            {
                ArrayOfScans[scanDesc]->first = -1;
                ArrayOfScans[scanDesc]->DescLastEntry = 0;
                memcpy(&nextBlock,ArrayOfScans[scanDesc]->tempBlock+sizeof(short),sizeof(int));
                /***************************
                diavazoume to epomeno block
                ****************************/
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, nextBlock, &ArrayOfScans[scanDesc]->tempBlock) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return NULL;
                }
            }

        }
        if(ArrayOfScans[scanDesc]->first == -1 && flag!=0 && (root_block != -1 && root_block != -2))
        {
            /************************************************
            vriskoume ta megalitera sta epomena block
            *************************************************/
            v=get_contition_entries_for_greater(scanDesc,ArrayOfScans[scanDesc]->tempBlock);
        	if(v!=NULL)
        		return v;
        }
        if(root_block == -1 || root_block == -2)
        {getchar();
            /******************************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************************/
            memcpy(&Num_of_Record_data,block+sizeof(short)+sizeof(int),sizeof(int));
            /******************************************************
            vriskoume ta megalitera se auto to block
            *******************************************************/
            v = get_first_greater_entries(scanDesc,block,Num_of_Record_data);
            if (v != NULL)
            {
                return v;
            }

        }

        break;
    case 5:
        /************************************************
        anazhthsh gia mikrotero h' iso
        vhma 1o
        tha vroume mexri kai pio block tha psaksoume

        *************************************************/
        if(root_block != -1 && root_block != -2)
        {
            temp_adress=block;
            errno=Scan_for_found_value_in_Index(scanDesc,bitDesc,temp_adress);
            if(errno < 0)
            {
                AM_errno=errno;
                return NULL;
            }

            /****************************
            diavazoume to block ths rizas
            *****************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &temp_adress) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            temp = errno;
        }
        if(ArrayOfScans[scanDesc]->first == 0 && (root_block != -1 && root_block != -2))
        {
            /***************************
            vhma 2
            ksanadiavazoume thn riza
            diavazoume to block ths rizas
            ****************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, root_block, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            memcpy(&bitDesc,block,sizeof(short));

            /*******************************************************************
            vhma 3
            tha pame sto pio aristero komvo-fyllo
            ********************************************************************/
            errno = Go_to_the_most_left_leafNode(ArrayOfScans[scanDesc]->fd,bitDesc,block);
            if(errno<0)
            {
                AM_errno=errno;
                return NULL;
            }

            /*******************************************************************
            diavazoume to block ton pio aristera komvo
            ********************************************************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            ArrayOfScans[scanDesc]->first = 1;
            ArrayOfScans[scanDesc]->tempBlock = block;
        }
        if(ArrayOfScans[scanDesc]->first == 1)
        {
            /*******************************************************************
            vhma 4
            tha pame na psaksoume mexri to block pou exei dieythinsi temp_adress
            ********************************************************************/
            void * vl;
            if((vl=get_contition_entries_for_less(scanDesc,ArrayOfScans[scanDesc]->tempBlock,temp_adress)) == NULL)
            {
                ArrayOfScans[scanDesc]->first = -1;
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, temp, &ArrayOfScans[scanDesc]->tempBlock) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return NULL;
                }
            }
            else return vl;
        }
        if(ArrayOfScans[scanDesc]->first == -1)
        {
            /*******************************************************************
            vhma 5o
            psaxnoume kai to teleytaio block pou tha isxyei h synthikh
            ********************************************************************/
            v=get_last_less_or_equal_entries(scanDesc,ArrayOfScans[scanDesc]->tempBlock);
            if(v!=NULL)
            	return v;
        }
        if(root_block == -1 || root_block == -2)
        {
           v=get_last_less_or_equal_entries(scanDesc,block);
           if(v!=NULL)
           {
           	return v;
           }
       }

       break;
    case 6:
        /******************************
        anazhthsh gia megalitero h' iso
        *******************************/
        if(ArrayOfScans[scanDesc]->first == 0 && root_block!=-1 )
        {
            errno=Scan_for_found_value_in_Index(scanDesc,bitDesc,block);
            if(errno < 0)
            {
                AM_errno=errno;
                break;
            }

            /****************************
            diavazoume to block ths rizas
            *****************************/
            if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, errno, &block) < 0)
            {
                AM_errno = RD_BLOCK;
                return NULL;
            }
            /******************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************/
            memcpy(&Num_of_Record_data,block+sizeof(short)+sizeof(int),sizeof(int));
            memcpy(&nextBlock,block+sizeof(short),sizeof(int));
            if(nextBlock = -1)
                flag = 0;
            ArrayOfScans[scanDesc]->first=1;
            ArrayOfScans[scanDesc]->tempBlock=block;
        }
        if(ArrayOfScans[scanDesc]->first == 1 )
        {
            void * vl;
            /******************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************/
            memcpy(&Num_of_Record_data,ArrayOfScans[scanDesc]->tempBlock+sizeof(short)+sizeof(int),sizeof(int));
            /***************************************
            vriskoume ta megalitera se auto to block
            ****************************************/
            vl = get_first_greater_or_equal_entries(scanDesc,ArrayOfScans[scanDesc]->tempBlock,Num_of_Record_data);
            if (vl != NULL)
            {
                return vl;
            }
            else
            {
                ArrayOfScans[scanDesc]->first = -1;
                ArrayOfScans[scanDesc]->DescLastEntry = 0;
                memcpy(&nextBlock,ArrayOfScans[scanDesc]->tempBlock+sizeof(short),sizeof(int));
                /***************************
                diavazoume to epomeno block
                ****************************/
                if (BF_ReadBlock(ArrayOfScans[scanDesc]->fd, nextBlock, &ArrayOfScans[scanDesc]->tempBlock) < 0)
                {
                    AM_errno = RD_BLOCK;
                    return NULL;
                }
            }

        }
        if(ArrayOfScans[scanDesc]->first == -1 && flag!=0)
        {
            /****************************************
            vriskoume ta megalitera sta epomena block
            *****************************************/
            return (void*)get_contition_entries_for_greater(scanDesc,ArrayOfScans[scanDesc]->tempBlock);
        }
        if(root_block == -1 || root_block == -2)
        {
            
            /******************************************
            diavazoume posa zeygaria exei mesa to block
            *******************************************/
            memcpy(&Num_of_Record_data,ArrayOfScans[scanDesc]->tempBlock+sizeof(short)+sizeof(int),sizeof(int));
            /***************************************
            vriskoume ta megalitera se auto to block
            ****************************************/
            v = get_first_greater_or_equal_entries(scanDesc,ArrayOfScans[scanDesc]->tempBlock,Num_of_Record_data);
            if (v != NULL)
            {
                return v;
            }
        }

        break;
    }

    AM_errno = AME_EOF;
    return NULL;
}

int AM_CloseIndexScan(int scanDesc)
{
    /*******************************************
    an o global pinakas anazhthsewn einai adeios
    ********************************************/
    if(ArrayOfScansIsEmpty(ArrayOfScans,MAXSCANS) == 0)
    {

        /***************************
        kratame to teleytaio sfalma
        gyrna ton kwdiko lathous
        ****************************/
        AM_errno=EMPTY_SCAN_ARRAY;

        return EMPTY_SCAN_ARRAY;
    }
    else
    {

        /**********************************************
        svhnoume thn kataxorhsh apo ton pinaka sarwsewn
        ***********************************************/
        if(Delete_Scan(ArrayOfScans,scanDesc) == -1)
        {
            AM_errno = DELETE_SCAN;
            return AME_EOF;
        }
    }
    return AME_OK;
}

void AM_PrintError(char *errString)
{
    /***************************
    Ektiponei to proto epipedo error
    ****************************/
    printf("\n------------------\n");
    BF_PrintError(errString);
    /***************************
    Ektiponei to stoxeumeno error
    ****************************/
    switch(AM_errno)
    {
    case FILE_EXISTS :
        printf("::File Already Exists!" );
        break;
    case FILE_CREATION :
        printf("::Error Creating File!" );
        break;
    case FILE_OPEN :
        printf("::Error Opening File!" );
        break;
    case AL_BLOCK :
        printf("::Error Allocating Block!" );
        break;
    case RD_BLOCK :
        printf("::Error Reading Block!" );
        break;
    case WR_BLOCK :
        printf("::Error Writing Block back!" );
        break;
    case FILE_CLOSE :
        printf("::Error Closing File!" );
        break;
    case LEN_ATTR_1 :
        printf("::Error in length of attribute 1!" );
        break;
    case TYPE_ATTR_1 :
        printf("::Error in type of attribute 1!" );
        break;
    case LEN_ATTR_2 :
        printf("::Error in length of attribute 2!" );
        break;
    case TYPE_ATTR_2 :
        printf("::Error in type of attribute 2!" );
        break;
    case FILE_DESTROY :
        printf("::Error Destroying File!" );
        break;
    case FILE_TYPE_MISMATCH :
        printf("::File is not a B+ file!" );
        break;
    case MAXOPENFILES_ERROR:
        printf("::The array of open files is full. Cannot open more files!" );
        break;
    case FILE_MANAGER_INSERT:
        printf("::Error inserting in file managing array!" );
        break;
    default :
        printf("\n" );
    }

    printf("\n------------------\n");
}
