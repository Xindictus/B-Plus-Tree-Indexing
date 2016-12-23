#ifndef FILE_MANAGER_
#define FILE_MANAGER_

#define MAXOPENFILES 20
typedef struct File_info{
	int fileDesc;
	char *fileName;
	char attrType1;
	int attrLength1;
	char attrType2;
	int attrLength2;
	int data_MAX;
	int index_MAX;
}File_info;

extern File_info **fileManager;

int insert_open_file(File_info **, int, int, char *, char, int, char, int, int, int);
int delete_open_fileName(File_info **, int, char *);
int delete_open_fileDesc(File_info **, int, int);
int count_open_files(File_info **, int);
int free_array(File_info **, int);

#endif
