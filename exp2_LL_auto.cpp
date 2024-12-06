#include <bits/stdc++.h>
#define MAX_LENGTH 1024
#define ACC 11
#define MATCH 12
#define ERR 13

typedef std::map<char, std::vector< std::pair< std::string, int > > > ProductionRule;
// ProductionRule['a'][i]->second 为序号
typedef std::map<char, std::map<char, int> > LLtable;

typedef std::map<char, std::set<char> > FirstFollowTable;

std::set<char> NonTerminate, Terminate;
std::map<char, bool> isBegin;

const char* sentence[] = {"","TA", "+TA", "-TA", "", "FB", "*FB", "/FB", "", "(E)", "n"};

ProductionRule initGramma() {
    ProductionRule gramma;

    NonTerminate.insert('E');
    NonTerminate.insert('A');
    NonTerminate.insert('T');
    NonTerminate.insert('B');
    NonTerminate.insert('F');
    isBegin.insert(std::make_pair('E', true));
    isBegin.insert(std::make_pair('A', false));
    isBegin.insert(std::make_pair('T', false));
    isBegin.insert(std::make_pair('F', false));
    isBegin.insert(std::make_pair('B', false));
    Terminate.insert('+');
    Terminate.insert('-');
    Terminate.insert('*');
    Terminate.insert('/');
    Terminate.insert('(');
    Terminate.insert(')');
    Terminate.insert('n');

    /*
    ->  1 E -> TA
        2 A -> +TA
        3 A -> -TA
        4 A -> ε
        5 T -> FB
        6 B -> *FB
        7 B -> /FB
        8 B -> ε
        9 F -> (E)
        10 F -> num
    */

    gramma['E'] = {
        {"TA", 1}  // E -> TA
    };
    gramma['A'] = {
        {"+TA", 2},  // A -> +TA
        {"-TA", 3},  // A -> -TA
        {"", 4}      // A -> ε
    };
    gramma['T'] = {
        {"FB", 5}  // T -> FB
    };
    gramma['B'] = {
        {"*FB", 6},  // B -> *FB
        {"/FB", 7},  // B -> /FB
        {"", 8}      // B -> ε
    };
    gramma['F'] = {
        {"(E)", 9},  // F -> (E)
        {"n", 10}    // F -> num
    };

    return gramma;
}

// 获取一个字符串的First集
// 和getChFirst一起可能有死循环，条件是有左递归
std::set<char> getStrFirst(ProductionRule gramma, FirstFollowTable& table, std::string str);

// 获取一个非终结符的First集
std::set<char> getChFirst(ProductionRule gramma, FirstFollowTable& table, char NonT){
    // 如果已经获取这个非终结符的First集，直接返回
    if(table.find(NonT) != table.end()){
        return table[NonT];
    }

    std::set<char> ans;

    // 遍历每一个产生式的First集合
    for(auto& it : gramma[NonT]){
        std::set<char> fir = getStrFirst(gramma, table, it.first);
        ans.insert(fir.begin(), fir.end());    
    }

    table[NonT] = ans;
    return ans;
}

std::set<char> getStrFirst(ProductionRule gramma, FirstFollowTable& table, std::string str){
    if(str.empty()){
        return {' '}; // 空串返回epsilon
    }

    std::set<char> ans;
    // 当前First集合是否有epsilon
    bool firstEpsilon = true;
    // 遍历字符串中每一个字符
    // 如果是终结符,ans没有空串,ans中加入该字符,退出循环
    // 如果是非终结符,获取该字符的First集
    // 如果First集没有空串,ans没有空串,ans中加入该First集,退出循环
    // 否则ans中加入该First集-epsilon,继续循环
    for(auto& ch : str){
        if(Terminate.find(ch) != Terminate.end()){
            ans.insert(ch);
            firstEpsilon = false;
            break;
        }else{
            std::set<char> fir = getChFirst(gramma, table, ch);
            if(fir.find(' ') != fir.end()){
                fir.erase(' ');
                ans.insert(fir.begin(), fir.end());
            }else{
                firstEpsilon = false;
                ans.insert(fir.begin(), fir.end());
                break;
            }
        }
    }
    // 循环结束后,当前First集合还有epsilon,First集加入epsilon
    if(firstEpsilon){
        ans.insert(' ');
    }
    return ans;
}

// 构造First表
FirstFollowTable createFirst(ProductionRule gramma){
    FirstFollowTable First;

    for(auto& it : gramma){

        getChFirst(gramma, First, it.first);

    }

    return First;
}

// 判断两个集合是否相同
bool setEqual(std::set<char> a, std::set<char> b){
    if(a.size() != b.size()){
        return false;
    }
    for(auto& it : a){
        if(b.find(it) == b.end()){
            return false;
        }
    }
    return true;
}

// 判断两个映射是否相同
bool mapEqual(FirstFollowTable table1, FirstFollowTable table2){
    if(table1.size() != table2.size()){
        return false;
    }
    for(auto& it : table1){
        if(table2.find(it.first) == table2.end()){
            return false;
        }
        if(!setEqual(it.second, table2[it.first])){
            return false;
        }
    }
    return true;
}

// 构造FolloW表
FirstFollowTable createFollow(ProductionRule gramma, FirstFollowTable firstTable) {
    FirstFollowTable Follow, tmpFollow;
    // 起始符的Follow集加入'$'
    for(auto& it: isBegin){
        if(it.second){
            Follow[it.first].insert('$');
        }
    }

    // 循环至Follow集合不再增长
    while(!mapEqual(Follow, tmpFollow))
    {
        tmpFollow = Follow;
        // A->aBc, Follow(B)中加入First(c)除去epsilon
        // A->aBc, First(c)中有epsilon, Follow(B)中加入Follow(A)
        for(auto& oneSentence : gramma){
            char left = oneSentence.first;
            for(auto& ri : oneSentence.second){
                std::string right = ri.first;
                // left -> right
                for(int i = 0; i < right.size(); i++){
                    if(NonTerminate.find(right[i]) != NonTerminate.end()){
                        std::set<char> fir = getStrFirst(gramma, firstTable, right.substr(i + 1));
                        if(fir.find(' ') != fir.end()){
                            std::set<char> tmp = Follow[left];
                            Follow[right[i]].insert(tmp.begin(), tmp.end());
                            fir.erase(' ');
                        }
                        Follow[right[i]].insert(fir.begin(), fir.end());
                    }
                }
            }
        }
    }
    return Follow;
}

LLtable createLLtable(ProductionRule gramma, FirstFollowTable firstTable, FirstFollowTable followTable){
    LLtable llT;
    // A->a 编号为idx, 非空串b属于First(a)，LL[A][b] = idx
    // 如果epsilon属于First(a)，b属于Follow(A)，LL[A][b] = idx
    for(auto& oneSentance : gramma){
        char Left = oneSentance.first;
        for(auto& ri : oneSentance.second){
            std::set<char> firstSen = getStrFirst(gramma, firstTable, ri.first);
            if(firstSen.find(' ') != firstSen.end()){
                for(auto& Ch : followTable[Left]){
                    llT[Left][Ch] = ri.second;
                }
                firstSen.erase(' ');
            }
            for(auto& Ch : firstSen){
                llT[Left][Ch] = ri.second;
            }
        }
    }
    return llT;
}

class Stack {
private:
    std::vector<char> stack;
public:
    Stack();
    void push(char c);
    void push(std::string);
    char pop();
    std::string print() const;
};

void Stack::push(char c) {
    this->stack.push_back(c);
}

void Stack::push(std::string c){
    for(auto i = c.rbegin(); i != c.rend(); i++){
        push(*i);
    }
}
char Stack::pop() {
    if (!stack.empty()) {
        char ans = stack.back();
        stack.pop_back();
        return ans;
    }
    return '\0'; // Return null character if stack is empty
}

Stack::Stack() {
    this->push('$');
    this->push('E');
}

std::string Stack::print() const {
    return std::string(stack.begin(), stack.end());
}

std::string input() {
    std::string buffer;
    std::getline(std::cin, buffer);
    buffer += '$'; // Append the end symbol
    return buffer;
}

int Action(char stCh, char buffSt, LLtable LL){
    if(stCh == '$' && buffSt == '$'){
        return ACC;
    }
    if(stCh == '$' || Terminate.find(stCh) != Terminate.end()){
        if(stCh == buffSt){
            return MATCH;
        }else{
            return ERR;
        }
    }

    auto i = LL[stCh];
    if(i.find(buffSt) == i.end()){
        return ERR;
    }

    return i[buffSt];
}

int main() {
    ProductionRule grammar = initGramma();
    Stack st;
    FirstFollowTable fir = createFirst(grammar);
    // for(auto& it: fir){
    //     std::cout << it.first << " : ";
    //     for(auto& ii : it.second){
    //         if(ii == ' '){
    //             std::cout << "epsilon ";
    //         }else{
    //             std::cout << ii << " ";
    //         }
    //     }
    //     std::cout << std::endl;
    // }
    FirstFollowTable fol = createFollow(grammar, fir);
    // for(auto& it : fol){
    //     std::cout << it.first << " : ";
    //     for(auto& ii : it.second){
    //         std::cout << ii << " ";
    //     }
    //     std::cout << std::endl;
    // }
    LLtable LL = createLLtable(grammar, fir, fol);
    // for(auto& it : LL){
    //     for(auto& ii : it.second){
    //         std::cout << "LL[" << it.first << "][" << ii.first << "] = " << ii.second << " ";
    //     }
    //     std::cout << std::endl;
    // }
    std::string buffer = input();

    int siz = buffer.size();
    bool isRunning = true;
    int buffPos = 0;
    while(buffPos < siz && isRunning){
        std::cout << st.print() << "\t" << buffer.substr(buffPos) << "\t";

        char stHead = st.pop();
        int nextAction = Action(stHead, buffer[buffPos], LL);
        switch (nextAction)
        {
        case ACC:{
            std::cout << "accept";
            buffPos++;
            break;
        }
        case ERR:{
            std::cout << "error";
            isRunning = false;
            break;
        }
        case MATCH:{
            std::cout << "match" << std::endl;
            buffPos++;
            break;
        }
        default:{
            std::cout << nextAction << std::endl;
            st.push(sentence[nextAction]);
            // buffPos++;
            break;
        }
        }
    }
    return 0;
}
