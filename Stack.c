#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Stack.h"

int push(Stack * S,int NumOfBlock)
{
    S->top++;
//    printf("stoiva = %d\n", S->top);
    S->Array[S->top]=NumOfBlock;
    return 0;
}


int pop(Stack * S)
{
    int p=S->Array[S->top];
    if(S->top>-1)
        S->top--;
    else return -1;

    return p;
}