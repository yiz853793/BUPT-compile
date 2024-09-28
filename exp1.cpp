#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX             1024
#define TYPE            8

//  word type
#define KEYWORD         0
#define IDENTIFIER      1
#define OPERATOR        2
#define DELIMITER       3
#define CHARCON         4
#define STRING          5
#define NUMBER          6
#define ERROR           7

//  charactor type
#define WLETTER         0
#define WNUMBER         1
#define WSCOLON         2
#define WDCOLON         3
#define WOTHER          4

long long sum[TYPE], line = 1;

// int condition = CONREAD;
const char* keyword[] = {"char","double","enum","float","int","long",
    "short","signed","struct","union","unsigned","void","for","do",
    "while","break","continue","if","else","goto","switch","case",
    "default","return","auto","extern","register","static","const",
    "sizeof","typedef","volatile"};

char myget(FILE* file){
    char c = fgetc(file);
    if(c == '\n'){
        line++;
    }
    return c;
}
int curMax = 16, curLen = 0;
char* word = (char*)malloc((curMax + 1) * sizeof(char));

int WordType(char ch){
    if((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || ch == '_'){
        return WLETTER;
    }

    if(ch >= '0' && ch <= '9'){
        return WNUMBER;
    }
    if(ch == '\''){
        return WSCOLON;
    }
    if(ch == '\"'){
        return WDCOLON;
    }
    return WOTHER;    
}

void push(char c){
    if(curLen >= curMax){
        curMax <<= 1;
        word = (char*)realloc(word, (curMax + 1) * sizeof(char));
    }

    if(c != '\n'){
        word[curLen++] = c;
        word[curLen] = '\0';
    }
}

void clear(){
    curLen = 0;
    word[curLen] = '\0';
}

char getWord(char c, FILE* file);
char getNum(char c, FILE* file, int pos);
char getString(char c, FILE* file);
char getChar(char, FILE* file);
char getOther(char c, FILE* file);


int main(int argc, char* argv[])
{

    const char* file_name = argv[1]; //该字符串变量为识别的C语言程序文件名

    /*程序每次运行只需要考虑识别一个C语言程序;
      需要使用读取文件的操作,可以在自己的本地环境里创建样例文件进行测试；
      在自己的IDE里测试时可以将file_name赋其他的值，
      但注意，最终测评时由平台提供的main参数来传入文件名*/
    // printf("%s\n",file_name);
    
    FILE* file = fopen(file_name,"r"); 
	if (file == NULL) {
		return -1;
	}

    char ch = myget(file);
    
    while(ch != EOF){
        if(ch == ' ' || ch == '\t' || ch == '\n'){
            ch = myget(file);
        }else{
            switch (WordType(ch)){
                case WLETTER:{
                    ch = getWord(ch, file);
                    break;
                }
                case WNUMBER:{
                    if(ch == '0'){
                        ch = getNum(ch, file, 5);  // ZERO
                    }else{
                        ch = getNum(ch, file, 1);  //  DECIMAL
                    }
                    
                    break;
                }
                case WSCOLON:{
                    ch = getChar(ch, file);
                    break;
                }
                case WDCOLON:{
                    ch = getString(ch, file);
                    break;
                }
                default:{
                    ch = getOther(ch, file);
                }
            }
        }
    }
    fclose(file);

    printf("%d\n", line);
    for(int i = 0; i < sizeof(sum) / sizeof(sum[0]) - 1; i++){
        printf("%d", sum[i]);
        if(i == NUMBER){
            putchar('\n');
        }else{
            putchar(' ');
        }
    }
    printf("%d\n", sum[ERROR]);

	return 0;
}


char getWord(char c, FILE* file){
    int curLine = line;
    if(c == 'u'){
        push(c);
        c = myget(file);
        if(c == '\''){
            return getChar(c, file);
        }else if(c == '"'){
            return getString(c, file);
        }else if(c == '8'){
            push(c);
            c = myget(file);
            if(c == '"'){
                return getString(c, file);
            }
        }
    }else if(c == 'U' || c == 'L'){
        push(c);
        c = myget(file);
        if(c == '\''){
            return getChar(c, file);
        }else if(c == '"'){
            return getString(c, file);
        }
    }

    while (WordType(c) == WLETTER || WordType(c) == WNUMBER){
        push(c);
        c = myget(file);
    }

    int isKey = false;
    for(int i = 0; (i < sizeof(keyword) / sizeof(keyword[0])) && !isKey; i++){
        isKey = strcmp(word, keyword[i]) == 0;
    }

    if(isKey){
        printf("%d <KEYWORD,%s>\n", curLine, word);
        sum[KEYWORD]++;
    }else{
        printf("%d <IDENTIFIER,%s>\n", curLine, word);
        sum[IDENTIFIER]++;
    }
    clear();
    return c;
}

#define DECIMAL     1
#define DOT         2
#define SCI         3
#define SCIN        4
#define ZERO        5
#define OCTAL       6
#define HEX         7
#define OWNU        8
#define UL          9
#define ULL         10
#define OWNL        11
#define LL          12
#define NUMERROR    13
#define NUMOUT      14
#define SCICHARA    15
#define OCERROR     16
#define LU          17
#define TERROR      18
#define FSUFFIX     19

char getNum(char c, FILE* file, int pos){
    // double a = 0xa.9;
    int curLine = line;
    
    while(pos != TERROR && pos != NUMOUT){
        push(c);
        c = myget(file);
        switch (pos){
            case DECIMAL:{
                if(c <= '9' && c >= '0'){
                    pos = DECIMAL;
                }else if(c == '.'){
                    pos = DOT;
                }else if(c == 'e' || c == 'E'){
                    pos = SCI;
                }else if(c == 'U' || c == 'u'){
                    pos = OWNU;
                }else if(c == 'l' || c == 'L'){
                    pos = OWNL;
                }else if(c == 'f' || c == 'F'){
                    pos = FSUFFIX;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                
                break;
            }
            case DOT:{
                if(c <= '9' && c >= '0'){
                    pos = DOT;
                 }else if(c == 'e' || c == 'E'){
                    pos = SCI;
                 }else if(c == 'f' || c == 'F' || c == 'l' || c == 'L'){
                    pos = FSUFFIX;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                 }else{
                    pos = NUMOUT;
                 }
                break;
            }
            case SCI:{
                if(c == '+' || c == '-'){
                    pos = SCICHARA;
                 }else if(c >= '0' && c <= '9'){
                    pos = SCIN;
                 }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                 }else{
                    pos = TERROR;
                }
                break;
            }
            case SCICHARA:{
                if(c >= '0' && c <= '9'){
                    pos = SCIN;
                 }else{
                    pos = NUMERROR;
                }
                break;
            }
            case SCIN:{
                if(c >= '0' && c <= '9'){
                    pos = SCIN;
                 }else if(c == 'f' || c == 'F' || c == 'l' || c == 'L'){
                    pos = FSUFFIX;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                 }else{
                    pos = NUMOUT;
                }
                break;
            }
            case ZERO:{
                if(c == 'x' || c == 'X'){
                    pos = HEX;
                 }else if(c == '.'){
                    pos = DOT;
                 }else if(c == 'e' || c == 'E'){
                    pos = SCI;
                 }else if(c >= '0' && c <= '7'){
                    pos = OCTAL;
                }else if(c == '8' || c == '9'){
                    pos = OCERROR;
                }else if(c == 'U' || c == 'u'){
                    pos = OWNU;
                }else if(c == 'l' || c == 'L'){
                    pos = OWNL;
                }else if(c == 'f' || c == 'F'){
                    pos = FSUFFIX;
                }else  if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case OCTAL:{
                if(c <= '7' && c >= '0'){
                    pos = OCTAL;
                }else if(c == '8' || c == '9'){
                    pos = OCERROR;
                }else if(c == '.'){
                    pos = DOT;
                }else if(c == 'e' || c == 'E'){
                    pos = SCI;
                }else if(c == 'U' || c == 'u'){
                    pos = OWNU;
                }else if(c == 'l' || c == 'L'){
                    pos = OWNL;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                
                break;
            }

            case OCERROR:{
                if(c <= '9' && c >= '0'){
                    pos = OCERROR;
                }else if(c == '.'){
                    pos == DOT;
                }else if(c == 'e' || c == 'E'){
                    pos = SCI;
                }else{
                    pos = NUMERROR;
                }
                break;
            }

            case HEX:{
                if((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') ){
                    pos = HEX;
                }else if(c == 'U' || c == 'u'){
                    pos = OWNU;
                }else if(c == 'l' || c == 'L'){
                    pos = OWNL;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }

            case OWNU:{
                if(c == 'l' || c == 'L'){
                    pos = UL;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case UL:{
                if(c == 'l' || c == 'L'){
                    pos = ULL;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case ULL:{
                if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }

            case OWNL:{
                if(c == 'l' || c == 'L'){
                    pos = LL;
                }else if(c == 'u' || c == 'U'){
                    pos = LU;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT; 
                }
                break;
            }
            case LU:{
                if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case LL:{
                if(c == 'u' || c == 'U'){
                    pos = ULL;
                }else if(WordType(c) == WLETTER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case FSUFFIX:{
                if(WordType(c) == WLETTER || WordType(c) == WNUMBER){
                    pos = NUMERROR;
                }else{
                    pos = NUMOUT;
                }
                break;
            }
            case NUMERROR:{
                if(WordType(c) == WLETTER || WordType(c) == WNUMBER){
                    pos = NUMERROR;
                }else{
                    pos = TERROR;
                }
            }
            default:
                break;
        }
    }
    
    if(pos == TERROR){
        printf("%d <ERROR,%s>\n", curLine, word);
        sum[ERROR]++;
    }else{
        printf("%d <NUMBER,%s>\n", curLine, word);
        sum[NUMBER]++;
    }

    clear();
    return c;
}

#undef DECIMAL
#undef DOT         
#undef SCI         
#undef SCIN        
#undef ZERO        
#undef Octal       
#undef HEX         
#undef OWNU        
#undef UL          
#undef ULL         
#undef OWNL        
#undef LL          
#undef NUMERROR    
#undef NUMOUT      
#undef SCICHARA    
#undef OCERROR   
#undef LU  
#undef TERROR
#undef FSUFFIX

char getString(char c, FILE* file){
    push(c);
    int isBackSlash = false, curLine = line;
    c = myget(file);

    // 输入不是EOF并且未出现单独的双引号
    while(c != EOF && c != '\n' && (isBackSlash || c != '\"')){
        if(c == '\\'){
            isBackSlash = !isBackSlash;
        }else{
            isBackSlash = false;
        }
        push(c);
        c = myget(file);
    }

    if(c != EOF && c != '\n'){
        push(c);
        printf("%d <STRING,%s>\n", curLine, word);
        sum[STRING]++;
        c = myget(file);
    }else{
        printf("%d <ERROR,%s>\n", curLine, word);
        sum[ERROR]++;
    }

    clear();
    return c;
}
char getChar(char c, FILE* file){
    push(c);
    int isBackSlash = false, curLine = line;
    c = myget(file);

    if(c != EOF && c != '\n' && c != '\''){
        if(c == '\\'){
            isBackSlash = !isBackSlash;
        }else{
            isBackSlash = false;
        }
        push(c);
        c = myget(file);
    }else{
        if(c == '\''){
            push(c);
            c = myget(file);
        }
        printf("%d <ERROR,%s>\n", curLine, word);
        clear();
        return c;
    }
    // 输入不是EOF并且未出现单独的单引号
    while(c != EOF && c != '\n' && (isBackSlash || c != '\'')){
        if(c == '\\'){
            isBackSlash = !isBackSlash;
        }else{
            isBackSlash = false;
        }
        push(c);
        c = myget(file);
    }

    if(c != EOF && c != '\n'){
        push(c);
        printf("%d <CHARCON,%s>\n", curLine, word);
        sum[CHARCON]++;
        c = myget(file);
    }else{
        printf("%d <ERROR,%s>\n", curLine, word);
        sum[ERROR]++;
    }

    clear();
    return c;
}
char getOther(char c, FILE* file){
    if( c == ';'|| c == ','|| c == ':'|| c == '?'|| c == '['|| c == ']'|| c == '('|| c == ')'|| c == '{'|| c == '}'){
        printf("%d <DELIMITER,%c>\n", line, c);
        sum[DELIMITER]++;
        return myget(file);
    }

    int curLine = line;
    switch (c)
    {
    case '=':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,==>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else{
            printf("%d <OPERATOR,=>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '+':{
        c = myget(file);
        if(c == '+'){
            printf("%d <OPERATOR,++>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else if(c == '='){
            printf("%d <OPERATOR,+=>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else{
            printf("%d <OPERATOR,+>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '-':{
        c = myget(file);
        if(c == '-'){
            printf("%d <OPERATOR,-->\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '='){
            printf("%d <OPERATOR,-=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '>'){
            printf("%d <OPERATOR,->>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else{
            printf("%d <OPERATOR,->\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '*':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,*=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else{
            printf("%d <OPERATOR,*>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '/':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,/=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '/'){
            while(c != EOF && c != '\n'){
                c = myget(file);
            }
        }else if(c == '*'){
            c = myget(file);
            #define NON     0
            #define STAR    1
            #define QUIT    2
            int pos = NON;
            while(c != EOF && pos != QUIT){
                switch (pos)
                {
                case NON:
                    if(c == '*'){
                        pos = STAR;
                    }
                    break;
                case STAR:
                    if(c == '/'){
                        pos = QUIT;
                    }else if(c == '*'){
                        pos = STAR;
                    }else{
                        pos = NON;
                    }
                    break;
                default:
                    break;
                }
                c = myget(file);
            }
        }else{
            printf("%d <OPERATOR,/>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '%':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,%=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else{
            printf("%d <OPERATOR,%>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '<':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,<=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '<'){
            c = myget(file);
            if(c == '='){
                c = myget(file);
                printf("%d <OPERATOR,<<=>\n", curLine);
                sum[OPERATOR]++;
            }else{
                printf("%d <OPERATOR,<<>\n", curLine);
                sum[OPERATOR]++;
            }
        }else{
            printf("%d <OPERATOR,<>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '>':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,>=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '>'){
            c = myget(file);
            if(c == '='){
                c = myget(file);
                printf("%d <OPERATOR,>>=>\n", curLine);
                sum[OPERATOR]++;
            }else{
                printf("%d <OPERATOR,>>>\n", curLine);
                sum[OPERATOR]++;
            }
        }else{
            printf("%d <OPERATOR,>>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '!':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,!=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else{
            printf("%d <OPERATOR,!>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '&':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,&=>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else if(c == '&'){
            printf("%d <OPERATOR,&&>\n", curLine);
            c = myget(file);
            sum[OPERATOR]++;
        }else{
            printf("%d <OPERATOR,&>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '|':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,|=>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else if(c == '|'){
            printf("%d <OPERATOR,||>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else{
            printf("%d <OPERATOR,|>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '^':{
        c = myget(file);
        if(c == '='){
            printf("%d <OPERATOR,^=>\n", curLine);
            sum[OPERATOR]++;
            c = myget(file);
        }else{
            printf("%d <OPERATOR,^>\n", curLine);
            sum[OPERATOR]++;
        }
        break;
    }
    case '.':{
        c = myget(file);
        if(WordType(c) == WNUMBER){
            push('.');
            c = getNum(c, file, 2); // DOT
        }else{
            printf("%d <OPERATOR,.>\n", curLine);
            sum[OPERATOR]++;  
        }
        break;
    }
    case '~':{
        c = myget(file);
        printf("%d <OPERATOR,~>\n", curLine);
        sum[OPERATOR]++;
        break;
    }
    default:{
        printf("%d <ERROR,%c>\n", curLine, c);
        sum[ERROR]++;
        c = myget(file);
        break;
    }
    }
    return c;
}
