#ifndef SCAN_MANAGER_
#define SCAN_MANAGER_

typedef struct Scan_info{
	int fd;
	int oper;
	char type;
	char type_of_record;
	int size_of_key;
	int size_of_record;
	void * key;
	int DescLastEntry;
	void * tempBlock;
	short first;
	short flag;
}Scan_info;

extern Scan_info ** ArrayOfScans;

int InsertScan(Scan_info ** ArrayOfScans,int maxscans, int filedesc,int op,void * value,char type_value,int size_key,char type_rec,int size_rec);

int Delete_Scan(Scan_info ** ArrayOfScans,int position);

int ArrayOfScansIsFull(Scan_info ** ArrayOfScans ,int maxscans);

int ArrayOfScansIsEmpty(Scan_info ** ArrayOfScans ,int maxscans);

int OpenScansForThisFiledesc(Scan_info ** ArrayOfScans , int fileDesc , int maxscans);

void free_scan_array(Scan_info ** ArrayOfScans,int maxscans);

#endif


