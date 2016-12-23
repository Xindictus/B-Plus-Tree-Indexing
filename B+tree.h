#ifndef B_PLUS_TREE
#define B_PLUS_TREE

#include "Stack.h"
#define  END 2

int compare_two_keys(char type_of_key, void * mykey, void * IndexKey , int op);
/*
   sygkrinei 2 kleidia an isxyei h synthikh me telesth op epistrefei 1 alliws 0
*/

void * found_value(int scanDesc,void * block,int i);
void * get_contition_entries_for_less(int scanDesc,void * block,void * temp_adress);
void * get_last_less_entries(int scanDesc,void * block);
void * get_last_less_or_equal_entries(int scanDesc,void * block);
void * get_first_greater_entries(int scanDesc,void * block,int Num_of_Record_data);
void * get_first_greater_or_equal_entries(int scanDesc,void * block,int Num_of_Record_data);
void * get_contition_entries_for_greater(int scanDesc,void * block);
void * get_contition_entries_for_not_equal(int scanDesc,void * block);
int Scan_for_found_value_in_Index(int scanDesc,short bitDesc,void * block);
void * Scan_for_found_value_in_Data(int scanDesc,void * block);
int Scan_for_found_value_with_stack(Stack * stackptr,int fd,char type,int size,void * value,short bitDesc,void * block);
int Go_to_the_most_left_leafNode(int fd,short bitDesc,void * block);
#endif
