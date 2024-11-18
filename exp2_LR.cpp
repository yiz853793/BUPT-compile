#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define MAX_LENGTH 1024

#define ERR 30

const char sentence[] = {'\0', 'E', 'E', 'E', 'T', 'T', 'T', 'F', 'F', };
int SenLen[] = {1, 3, 3, 1, 3, 3, 1, 3, 1};
char buffer[MAX_LENGTH];
int StackLength = 0, BufferPos = 0;
int stack[MAX_LENGTH];

void push(int c){
    stack[StackLength] = c;
    StackLength++;
}

void pop(){
    StackLength--;
}

int StackHead(){
    return stack[StackLength-1];
}
int input(char* buffer){
    fgets(buffer, MAX_LENGTH * sizeof(char), stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    int siz = strlen(buffer);
    buffer[siz++] = '$';
    buffer[siz] = '\0';
    return siz;
}

int StackInit(){
    push(0);
}
int Action(int pos, char Ch){
    switch (pos)
    {
    case 0:{
        switch (Ch)
        {
        case 'n':
            return 1;
        case '(':
            return 2;
        case 'E':
            return 3;
        case 'T':
            return 4;
        case 'F':
            return 5;        
        default:
            return ERR;
        }
    }
    case 1:{
        switch (Ch)
        {
        case '$': case '+': case '-': case '*': case '/':
            return -8;
        default:
            return ERR;
        }
    }
    case 2:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'E' :
            return 8;
        case 'T' :
            return 9;
        case 'F':
            return 10;
        default:
            return ERR;
        }
    }
    case 3:{
        switch (Ch)
        {
        case '+':
            return 11;
        case '-':
            return 12;
        case '$':
            return 0;
        default:
            return ERR;
        }
    }
    case 4:{
        switch (Ch)
        {
        case '*':
            return 13;
        case '/':
            return 14;
        case '$': case '+': case '-':
            return -3;
        default:
            return ERR;
        }
    }
    case 5:{
        switch (Ch)
        {
        case '$': case '+': case '-': case '*': case '/':
            return -6;
        default:
            return ERR;
        }
    }
    case 6:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'E':
            return 15;
        case 'T':
            return 9;
        case 'F':
            return 10;
        default:
            return ERR;
        }
    }
    case 7:{
        switch (Ch)
        {
        case ')': case '+': case '-': case '*': case '/':
            return -8;
        default:
            return ERR;
        }
    }
    case 8:{
        switch (Ch)
        {
        case '+':
            return 16;
        case '-':
            return 17;
        case ')':
            return 18;
        default:
            return ERR;
        }
    }
    case 9:{
        switch (Ch)
        {
        case '*':
            return 19;
        case '/':
            return 20;
        case ')': case '+': case '-':
            return -3;
        default:
            return ERR;
        }
    }
    case 10:{
        switch (Ch)
        {
        case ')': case '+': case '-': case '*': case '/':
            return -6;
        default:
            return ERR;
        }
    }
    case 11:{
        switch (Ch)
        {
        case '(':
            return 2;
        case 'n':
            return 1;
        case 'T':
            return 21;
        case 'F':
            return 5;
        default:
            return ERR;
        }
    }
    case 12:{
        switch (Ch)
        {
        case '(':
            return 2;
        case 'n':
            return 1;
        case 'T':
            return 22;
        case 'F':
            return 5;
        default:
            return ERR;
        }
    }
    case 13:{
        switch (Ch)
        {
        case '(':
            return 2;
        case 'n':
            return 1;
        case 'F':
            return 28;
        default:
            return ERR;
        }
    }
    case 14:{
        switch (Ch)
        {
        case '(':
            return 2;
        case 'n':
            return 1;
        case 'F':
            return 29;
        default:
            return ERR;
        }
    }
    case 15:{
        switch (Ch)
        {
        case ')':
            return 23;
        case '+':
            return 16;
        case '-':
            return 17;
        default:
            return ERR;
        }
    }
    case 16:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'F':
            return 10;
        case 'T':
            return 24;
        default:
            return ERR;
        }
    }
    case 17:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'F':
            return 10;
        case 'T':
            return 25;
        default:
            return ERR;
        }
    }
    case 18:{
        switch (Ch)
        {
        case '$': case '+': case '-': case '*': case '/':
            return -7;
        default:
            return ERR;
        }
    }
    case 19:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'F':
            return 26;
        default:
            return ERR;
        }
    }
    case 20:{
        switch (Ch)
        {
        case '(':
            return 6;
        case 'n':
            return 7;
        case 'F':
            return 27;
        default:
            return ERR;
        }
    }
    case 21:{
        switch (Ch)
        {
        case '*':
            return 13;
        case '/':
            return 14;
        case '$': case '+': case '-':
            return -1;
        default:
            return ERR;
        }
    }
    case 22:{
        switch (Ch)
        {
        case '*':
            return 13;
        case '/':
            return 14;
        case '$' : case '+': case '-':
            return -2;
        default:
            return ERR;
        }
    }
    case 23:{
        switch (Ch)
        {
        case ')': case '+': case '-': case '*': case '/':
            return -7;
        default:
            return ERR;
        }
    }
    case 24:{
        switch (Ch)
        {
        case '*':
            return 19;
        case 'n':
            return 20;
        case ')': case '+': case '-':
            return -1;
        default:
            return ERR;
        }
    }
    case 25:{
        switch (Ch)
        {
        case '*':
            return 19;
        case 'n':
            return 20;
        case ')': case '+': case '-':
            return -2;
        default:
            return ERR;
        }
    }
    case 26:{
        switch (Ch)
        {
        case ')': case '+': case '-': case '*': case '/':
            return -4;
        default:
            return ERR;
        }
    }
    case 27:{
        switch (Ch)
        {
        case ')': case '+': case '-': case '*': case '/':
            return -5;
        default:
            return ERR;
        }
    }
    case 28:{
        switch (Ch)
        {
        case '$': case '+': case '-': case '*': case '/':
            return -4;
        
        default:
            return ERR;
        }
    }
    case 29:{
        switch (Ch)
        {
        case '$': case '+': case '-': case '*': case '/':
            return -5;
        
        default:
            return ERR;
        }
    }
    default:
        return ERR;
    }
}

int main(){
    int siz = input(buffer);

    bool isRunning = true;
    int curPos = 0;
    while(curPos < siz && isRunning){
        // printf("%d %c ", StackHead(), buffer[curPos]);
        int nextAction = Action(StackHead(), buffer[curPos]);
        
        if(nextAction == 0){
            puts("accept");
            curPos++;
        }else if(nextAction == ERR){
            puts("error");
            isRunning = false;
        }else if(nextAction > 0){
            puts("shift");
            push(nextAction);
            curPos++;
        }else if(nextAction < 0){
            int SenIdx = -nextAction;
            printf("%d\n", SenIdx);
            for(int i = 0; i < SenLen[SenIdx]; i++)
                pop();
            nextAction = Action(StackHead(), sentence[SenIdx]);
            // printf("%d\n", nextAction);
            push(nextAction);
        }
    }
    return 0;
}