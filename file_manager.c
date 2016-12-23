#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_manager.h"

File_info **fileManager;

int insert_open_file(File_info **file_manager_array, int maxopenfiles,
                     int fileDesc, char *fileName, char attrType1,
                     int attrLength1, char attrType2, int attrLength2,
                     int data_MAX, int index_MAX)
{
    int i;
    for(i = 0; i < maxopenfiles; i++)
    {
        if(file_manager_array[i] == NULL)
        {
            file_manager_array[i] = (File_info*)malloc(sizeof(File_info));
            file_manager_array[i]->fileDesc = fileDesc;
            file_manager_array[i]->fileName = (char*)malloc(strlen(fileName)+1);
            file_manager_array[i]->attrType1 = attrType1;
            file_manager_array[i]->attrType2 = attrType2;
            file_manager_array[i]->attrLength1 = attrLength1;
            file_manager_array[i]->attrLength2 = attrLength2;
            file_manager_array[i]->data_MAX = data_MAX;
            file_manager_array[i]->index_MAX = index_MAX;
            strncpy(file_manager_array[i]->fileName, fileName, strlen(fileName)+1);
            return i;
        }
    }
    return -1;
}

int delete_open_fileDesc(File_info **file_manager_array, int maxopenfiles, int fileDesc)
{
    int i;
    for(i = 0; i < maxopenfiles; i++)
    {
        if(file_manager_array[i] != NULL)
        {
            if( file_manager_array[i]->fileDesc == fileDesc )
            {
                free(file_manager_array[i]->fileName);
                free(file_manager_array[i]);
                file_manager_array[i] = NULL;
                return 0;
            }
        }
    }
    return -1;
}

int delete_open_fileName(File_info **file_manager_array, int maxopenfiles, char *fileName)
{
    int i, fileDesc;
    for(i = 0; i < maxopenfiles; i++)
    {
        if(file_manager_array[i] != NULL)
        {
            if( strcmp(file_manager_array[i]->fileName, fileName) == 0 )
            {
                fileDesc = file_manager_array[i]->fileDesc;
                free(file_manager_array[i]->fileName);
                free(file_manager_array[i]);
                file_manager_array[i] = NULL;
                return fileDesc;
            }
        }
    }
    return -1;
}

int count_open_files(File_info **file_manager_array, int maxopenfiles)
{
    int i, count = 0;
    for(i = 0; i < maxopenfiles; i++)
    {
        if(file_manager_array[i] != NULL)
        {
            count++;
        }
    }
    return count;
}

int free_array(File_info **file_manager_array, int maxopenfiles)
{
    int i;
    for(i = 0; i < maxopenfiles; i++)
    {
        if(file_manager_array[i] != NULL)
        {
            free(file_manager_array[i]->fileName);
            free(file_manager_array[i]);
            file_manager_array[i] = NULL;
        }
    }
    free(file_manager_array);
}
