SOURCE = main1.c AM.c file_manager.c scan_manager.c B+tree.c Stack.c
OBJS = main1.o AM.o file_manager.o scan_manager.o B+tree.o Stack.o
HEADER = AM.h defn.h file_manager.h BF.h scan_manager.h B+tree.h Stack.h
OUT = project2
CC = gcc
FLAGS = -c -w -g3

all: $(OBJS) 
	$(CC) -g $(OBJS) -L . BF_32.a -o $(OUT)

main1.o : main1.c
	$(CC) $(FLAGS) main1.c

AM.o: AM.c
	$(CC) $(FLAGS) AM.c
	
file_manager.o: file_manager.c
	$(CC) $(FLAGS) file_manager.c

scan_manager.o: scan_manager.c
	$(CC) $(FLAGS) scan_manager.c

B+tree.o: B+tree.c
	$(CC) $(FLAGS) B+tree.c

Stack.o: Stack.c
	$(CC) $(FLAGS) Stack.c

build : $(SOURCE)
	$(CC) -o $(OUT) $(SOURCE)

clean :
	rm -f $(OBJS) $(OUT) B+ DNAME-AGE EMP-AGE EMP-NAME EMP-SAL