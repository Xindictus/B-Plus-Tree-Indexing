#ifndef STACK
#define STACK

#define MAX 1000

typedef struct Stack
{
	int top;
	int Array[MAX];

}Stack;

int push(Stack * S,int NumOfBlock);
int pop(Stack * S);

#endif