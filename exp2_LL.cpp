#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_LENGTH 1024
#define ACC 11
#define MATCH 12
#define ERR 13

const char* sentence[] = {"","TA", "+TA", "-TA", "", "FB", "*FB", "/FB", "", "(E)", "n"};
char stack[MAX_LENGTH];
int StackLength = 0;
void push(char c){
    stack[StackLength] = c;
    StackLength++;
    stack[StackLength] = '\0';
}

char pop(){
    StackLength--;
    char ans = stack[StackLength];
    stack[StackLength] = '\0';
    return ans;
}

void StackInit(){
    push('$');
    push('E');
}

int input(char* buffer){
    fgets(buffer, MAX_LENGTH * sizeof(char), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    int siz = strlen(buffer);
    buffer[siz++] = '$';
    buffer[siz] = '\0';
    return siz;
}
void StrPush(const char* str){
    int siz = strlen(str);
    for(int i = siz - 1; i >= 0; i--){
        push(str[i]);
    }
}
int Action(char StackCh, char BufferCh){
    if(StackCh == '$' && BufferCh == '$'){
        return ACC;
    }
    if(StackCh == '$'){
        return ERR;
    }
    switch (StackCh)
    {
    case 'A':{
        switch (BufferCh)
        {
        case '+':
            return 2;
        case '-':
            return 3;
        case '$': case ')':
            return 4;
        default:
            return ERR;
        }
    }
    case 'B': {
        switch (BufferCh)
        {
        case '*':
            return 6;
        case '/':
            return 7;
        case '+': case '-': case ')': case '$':
            return 8;
        default:
            return ERR;
        }
    }
    case 'F':{
        switch (BufferCh)
        {
        case '(':
            return 9;
        case 'n':
            return 10;
        
        default:
            return ERR;
        }
    }
    case 'T' :{
        switch (BufferCh)
        {
        case '(': case 'n':
            return 5;
        
        default:
            return ERR;
        }
    }
    case 'E':{
        switch (BufferCh)
        {
        case '(': case 'n':
            return 1;
        
        default:
            return ERR;
        }
    }
    default:
        if(StackCh == BufferCh) return MATCH;
        else    return ERR;
    }
}

int main(){
    char *buffer = (char*) malloc (MAX_LENGTH * sizeof(char));
    char* tmp = buffer;
    int InputSiz = input(buffer);
    StackInit();
    char BufferHead = *buffer;
    int isRunning = true;
    while(BufferHead && isRunning){
        printf("%s\t%s\t", stack, buffer);
        int nextAction = Action(pop(), BufferHead);
        if(nextAction == ACC){
            puts("accept");
            buffer++;
        }
        else if(nextAction == MATCH){
            puts("match");
            buffer++;
        }else if(nextAction == ERR){
            puts("error");
            isRunning = false;
        }else{
            printf("%d\n", nextAction);
            StrPush(sentence[nextAction]);
        }
        BufferHead = *buffer;
    }
    free(tmp);
    return 0;
}


// $E	(n+n)n$	1
// $AT	(n+n)n$	5
// $ABF	(n+n)n$	9
// $AB)E(	(n+n)n$	match
// $AB)E	n+n)n$	1
// $AB)AT	n+n)n$	5
// $AB)ABF	n+n)n$	10
// $AB)ABn	n+n)n$	match
// $AB)AB	+n)n$	8
// $AB)A	+n)n$	2
// $AB)AT+	+n)n$	match
// $AB)AT	n)n$	5
// $AB)ABF	n)n$	10
// $AB)ABn	n)n$	match
// $AB)AB	)n$	8
// $AB)A	)n$	4
// $AB)	)n$	match
// $AB	n$	error