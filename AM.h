#ifndef AM_H_
#define AM_H_

/* Error codes */

extern int AM_errno;

#define AME_OK 0
#define AME_EOF -1
#define DATA_BLOCK_ID 1
//TODO: Add your error codes
#define FILE_EXISTS -200
#define FILE_CREATION -202
#define FILE_OPEN -204
#define AL_BLOCK -206
#define RD_BLOCK -208
#define WR_BLOCK -210
#define FILE_CLOSE -212
#define TYPE_ATTR_1 -102
#define LEN_ATTR_1 -100
#define TYPE_ATTR_2 -106
#define LEN_ATTR_2 -104
#define FILE_DESTROY -300
#define FILE_TYPE_MISMATCH -310
#define MAXOPENFILES_ERROR -320
#define FILE_MANAGER_INSERT -330
#define FILE_MANAGER_DELETE -340

#define FULL_SCAN_ARRAY -2
#define EMPTY_SCAN_ARRAY -3
#define NOT_EMPTY_SCAN_ARRAY -4
#define EMPTY_FILE_ARRAY -6
#define OPEN_SCANS -7
#define WRONG_OPERATOR -8
#define INSERT_SCAN -9
#define DELETE_SCAN -10
#define SCAN_FAILED -11

#define EQUAL 1
#define NOT_EQUAL 2
#define LESS_THAN 3
#define GREATER_THAN 4
#define LESS_THAN_OR_EQUAL 5
#define GREATER_THAN_OR_EQUAL 6

#define CHARACTERISTIC_SIZE 3

#define MAXSCANS 20

void AM_Init( void );

int AM_CreateIndex(
  char *fileName, /* όνομα αρχείου */
  char attrType1, /* τύπος πρώτου πεδίου: 'c' (συμβολοσειρά), 'i' (ακέραιος), 'f' (πραγματικός) */
  int attrLength1, /* μήκος πρώτου πεδίου: 4 γιά 'i' ή 'f', 1-255 γιά 'c' */
  char attrType2, /* τύπος πρώτου πεδίου: 'c' (συμβολοσειρά), 'i' (ακέραιος), 'f' (πραγματικός) */
  int attrLength2 /* μήκος δεύτερου πεδίου: 4 γιά 'i' ή 'f', 1-255 γιά 'c' */
);


int AM_DestroyIndex(
  char *fileName /* όνομα αρχείου */
);


int AM_OpenIndex (
  char *fileName /* όνομα αρχείου */
);


int AM_CloseIndex (
  int fileDesc /* αριθμός που αντιστοιχεί στο ανοιχτό αρχείο */
);


int AM_InsertEntry(
  int fileDesc, /* αριθμός που αντιστοιχεί στο ανοιχτό αρχείο */
  void *value1, /* τιμή του πεδίου-κλειδιού προς εισαγωγή */
  void *value2 /* τιμή του δεύτερου πεδίου της εγγραφής προς εισαγωγή */
);


int AM_OpenIndexScan(
  int fileDesc, /* αριθμός που αντιστοιχεί στο ανοιχτό αρχείο */
  int op, /* τελεστής σύγκρισης */
  void *value /* τιμή του πεδίου-κλειδιού προς σύγκριση */
);


void *AM_FindNextEntry(
  int scanDesc /* αριθμός που αντιστοιχεί στην ανοιχτή σάρωση */
);


int AM_CloseIndexScan(
  int scanDesc /* αριθμός που αντιστοιχεί στην ανοιχτή σάρωση */
);


void AM_PrintError(
  char *errString /* κείμενο για εκτύπωση */
);

void AM_Close( void );

#endif /* AM_H_ */
