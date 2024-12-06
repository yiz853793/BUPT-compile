#include <bits/stdc++.h>

typedef std::map<char, std::vector< std::pair< std::string, int > > > ProductionRule;

typedef std::map<int, std::map<char, int> > LRtable;

std::map<char, bool> isBegin;
std::set<char> NonTerminate, Terminate;

char sentenceLeft[] = {'G', 'E', 'E', 'E', 'T', 'T', 'T', 'F', 'F'};
std::string sentenceRight[] = {"E", "E+T", "E-T", "T", "T*F", "T/F", "F", "(E)", "n"};

ProductionRule initGramma() {
    ProductionRule gramma;

    NonTerminate.insert('G');
    NonTerminate.insert('E');
    NonTerminate.insert('T');
    NonTerminate.insert('F');
    isBegin.insert(std::make_pair('G', true));
    isBegin.insert(std::make_pair('E', false));
    isBegin.insert(std::make_pair('T', false));
    isBegin.insert(std::make_pair('F', false));
    Terminate.insert('+');
    Terminate.insert('-');
    Terminate.insert('*');
    Terminate.insert('/');
    Terminate.insert('(');
    Terminate.insert(')');
    Terminate.insert('n');

    /*
    ->  0	E'-> E
        1	E -> E+T
        2	E -> E-T
        3	E -> T
        4	T -> T*F
        5	T -> T/F
        6	T -> F
        7	F -> (E)
        8	F -> num    
    */

    gramma['G'] = {
        {"E", 0}  // E' -> E
    };
    gramma['E'] = {
        {"E+T", 1},  // E -> E+T
        {"E-T", 2},  // E -> E-T
        {"T", 3}      // E -> T
    };
    gramma['T'] = {
        {"T*F", 4},  // T -> T*F
        {"T/F", 5},  // T -> T/F
        {"F", 6}     // T -> F
    };
    gramma['F'] = {
        {"(E)", 7},  // F -> (E)
        {"n", 8}    // F -> num
    };

    return gramma;
}

// 项目类
class item{
public:
    // 项目左侧
    char left;
    // 项目终结符
    char end;
    // 生成式编号
    int idx;
    // 项目中点的位置
    int dotPos;
    // 项目右侧
    std::string sentence;

    item(char left, char end, int idx, std::string sen): left(left), end(end), idx(idx), sentence(sen){
        this->dotPos = 0;
    }
    // 获取到点后的字符后构造新的项目
    item next() const ;
    // 是否是归约项目
    bool isReduction() const ;
    // 获取期望接收的字符
    char expect() const ;
    // 获取生成式编号
    int itemIdx() const;
    // 获取项目终结符
    char itemEnd() const;
    // 获取新项目的新终结符
    char newEnd() const ;
    bool operator<(const item& other) const;
};

item item::next() const {
    item ans(this->left, this->end, this->idx, this->sentence);
    ans.dotPos = this->dotPos + 1;
    return ans;
}

bool item::isReduction() const {
    return this->dotPos == this->sentence.size();
}

char item::expect() const {
    if(this->dotPos < this->sentence.size()){
        return this->sentence[this->dotPos];
    }
    return '\0';
}

int item::itemIdx() const {
    return this->idx;
}

char item::itemEnd() const {
    return this->end;
}

char item::newEnd() const {
    if(this->dotPos < this->sentence.size() - 1){
        return this->sentence[this->dotPos + 1];
    }
    return this->end;
}

bool item::operator<(const item& other) const {
    if (this->left != other.left) return this->left < other.left;
    if (this->end != other.end) return this->end < other.end;
    if (this->dotPos != other.dotPos) return this->dotPos < other.dotPos;
    if (this->sentence != other.sentence) return this->sentence < other.sentence;
    return this->idx < other.idx;
}

// 求状态闭包
std::set<item> closure(ProductionRule gramma, std::set<item> J){
    std::set<item> ans = J;
    std::vector<item> ansSt;
    // 将当前项目推入队列
    for(auto& it : J){
        ansSt.push_back(it);
    }
    // 遍历队列中的项目，如果接收非终结符，计算新的项目
    // 如果前面不存在该项目，推入队列，加入闭包
    for(int i = 0; i < ansSt.size(); i++){
        auto it = ansSt[i];
        char dotCh = it.expect();
        if(NonTerminate.find(dotCh) != NonTerminate.end()){
            char newEnd = it.newEnd();
            for(auto& ii : gramma[dotCh]){
                item tmp(dotCh, newEnd, ii.second, ii.first);
                if(ans.find(tmp) == ans.end()){
                    ans.insert(tmp);
                    ansSt.push_back(tmp);
                }
            }
        }
    }

    return ans;
}

bool setEqual(std::set<item> ItemA, std::set<item> ItemB){
    if(ItemA.size() != ItemB.size()){
        return false;
    }

    for(auto& it : ItemA){
        if(ItemB.find(it) == ItemB.end()){
            return false;
        }
    }

    return true;
}

// class smallItem{
// public:
//     char left;
//     int dotPos;
//     std::string sentence;

//     smallItem(char left, int dotPos, std::string sen): left(left), dotPos(dotPos), sentence(sen) {};
//     bool operator<(const smallItem& other) const;
//     std::string print();
// };

// bool smallItem::operator<(const smallItem& other) const {
//     if(this->left != other.left) return this->left < other.left;
//     if(this->sentence != other.sentence) return this->sentence < other.sentence;
//     return this->dotPos < other.dotPos;
// }

// std::string smallItem::print(){
//     std::string ans;
//     ans.push_back(this->left);
//     ans.append(" -> ");
//     for(int i = 0; i < this->dotPos; i++){
//         ans.push_back(sentence[i]);
//     }
//     ans.append(" \\textbullet ");
//     for(int i = this->dotPos; i < this->sentence.size(); i++){
//         ans.push_back(sentence[i]);
//     }
//     return ans;
// }

int findConditionSet(std::vector<std::set<item>> condition, std::set<item> J){
    int siz = condition.size();
    for(int i = 0; i < siz; i++){
        if(setEqual(condition[i], J)){
            return i;
        }
    }
    return -1;
}

// 错误编号，在所有状态编号之后
int ERR; 
// 创建LR分析表，LR[state][Ch] < 0时，该值表示通过生成式-LR[state][Ch]归约
// LR[state][Ch] = 0，表示识别成功
// LR[state][Ch] > 0，值为下一个状态
LRtable createLRTable(ProductionRule grammar){
    LRtable ans;
    // 状态队列
    std::vector<std::set<item>> conditionSets;
    // 起始符的所有生成式构建项目并求闭包，为初始状态，状态编号0
    for(auto& it : isBegin){
        if(it.second){
            std::set<item> firstSet;
            for(auto& rule : grammar[it.first]){
                firstSet.insert(item(it.first, '$', rule.second, rule.first));
            }
            firstSet = closure(grammar, firstSet);
            conditionSets.push_back(firstSet);
        }
    }

    // 遍历每一个状态
    // 如果一个项目是归约项目，那么LR[state][end] = -idx
    // 其余按照期待接收的字符分类，并计算移进后的新项目
    // 分类后求闭包，成下一个状态
    // 如果该状态之前存在，找到它的状态号，LR[state][end] = stateIdx
    // 否则状态推入状态队列中，LR[state][end] = newStateIdx
    for(int i = 0; i < conditionSets.size(); i++){
        auto& stateItems = conditionSets[i];
        std::map<char, std::set<item>> transitionMap;
        for(auto& stateItem : stateItems){
            if(stateItem.isReduction()){
                ans[i][stateItem.itemEnd()] = -stateItem.itemIdx();
            }else{
                transitionMap[stateItem.expect()].insert(stateItem.next());
            }
        }
        for(auto& transition : transitionMap){
            // debug
            // std::cout << "( " << i << ", " << transition.first << ") = closure( ";
            // std::map<smallItem, std::set<char>> tempMap;
            // for(auto& i : transition.second){
            //     tempMap[smallItem(i.left, i.dotPos, i.sentence)].insert(i.end);
            // }
            // std::cout << " {" ;
            // for(auto& tt : tempMap){
            //     smallItem temp = tt.first;
            //     std::cout << "[ " << temp.print() << ", ";
            //     for(auto& i : tt.second){
            //         std::cout << i;
            //     }
            //     std::cout << " ], ";
            // }
            // std::cout << " } )" << std::endl;

            auto temp = closure(grammar, transition.second);
            int nextCondition = findConditionSet(conditionSets, temp);
            if(nextCondition == -1){
                nextCondition = conditionSets.size();
                conditionSets.push_back(temp);
            }
            ans[i][transition.first] = nextCondition;
        }
    }

    ERR = conditionSets.size();
    // debug
    // for(int j = 0; j < conditionSets.size(); j++){
    //     auto it = conditionSets[j];
    //     std::map<smallItem, std::set<char>> tempMap;

    //     for(auto& item : it){
    //         tempMap[smallItem(item.left, item.dotPos, item.sentence)].insert(item.end);
    //     }

    //     std::cout << "\\texttt{$I_{" << j << "}$:} \\newline" << std::endl;;

    //     std::cout << "\\texttt{\\{} \\newline" << std::endl;
    //     for(auto& tt : tempMap){
    //         smallItem temp = tt.first;
    //         std::cout << "\\texttt{\t[ " << temp.print() << ",\t";
    //         for(auto& i : tt.second){
    //             if(i == '$'){
    //                 std::cout << "\\";
    //             }
    //             std::cout << i;
    //         }
    //         std::cout << " ]} \\newline" << std::endl;
    //     }
    //     std::cout << "\\texttt{\\}} \\newline" << std::endl;
    // }
    return ans;
}


class Stack {
private:
    std::vector<int> stack;
public:
    Stack();
    void push(int c);
    void pop();
    int stackHead();
};

void Stack::push(int c) {
    this->stack.push_back(c);
}

void Stack::pop() {
    if (!stack.empty()) {
        stack.pop_back();
    }
}

Stack::Stack() {
    this->push(0);
}

int Stack::stackHead(){
    return stack.back();
}

std::string input() {
    std::string buffer;
    std::getline(std::cin, buffer);
    buffer += '$'; // Append the end symbol
    return buffer;
}

int Action(LRtable LR, int Pos, char Ch){
    std::map<char, int> tmp = LR[Pos];
    if(tmp.find(Ch) == tmp.end()){
        return ERR;
    }
    return tmp[Ch];
}

int main(){
    ProductionRule gramma = initGramma();
    Stack st;
    LRtable LR = createLRTable(gramma);
    // std::cout << LR.size() << std::endl;

    // for(auto& it : LR){
    //     for(auto& ii : it.second){
    //         std::cout << "go(" << it.first << ", " << ii.first << ") = " << ii.second << "  ";
    //     }
    //     std::cout << std::endl;
    // }
    // std :: cout << "\\hline ";

    // for(auto& it : Terminate){
    //     std :: cout << " & " << it ;
    // }
    // std :: cout << " & $ " ;
    // for(auto & it : NonTerminate){
    //     std :: cout << " & " << it ;
    // }
    // std :: cout << " \\\\ " << std::endl;

    // for(auto& it : LR){
    //     std::cout << "\\hline " << it.first << " ";
    //     auto a = it.second;
    //     for(auto& it : Terminate){
    //         if(a.find(it) == a.end()){
    //             std :: cout << " & ";
    //         }else{
    //             int j = a[it];
    //             if(j == 0){
    //                 std::cout << " & acc";
    //             }else if(j > 0){
    //                 std::cout << " & " << j;
    //             }else{
    //                 j = -j;
    //                 std::cout << " & R" << j;
    //             }
    //         }
    //     }
    //     if(a.find('$') == a.end()){
    //         std :: cout << " & ";
    //     }else{
    //         int j = a['$'];
    //         if(j == 0){
    //             std::cout << " & acc";
    //         }else if(j > 0){
    //             std::cout << " & " << j;
    //         }else{
    //             j = -j;
    //             std::cout << " & R" << j;
    //         }
    //     }
    //     for(auto & it : NonTerminate){
    //         if(a.find(it) == a.end()){
    //             std :: cout << " & ";
    //         }else{
    //             int j = a[it];
    //             if(j == 0){
    //                 std::cout << " & acc";
    //             }else if(j > 0){
    //                 std::cout << " & " << j;
    //             }else{
    //                 j = -j;
    //                 std::cout << " & R" << j;
    //             }
    //         }
    //     }
    //     std :: cout << " \\\\ " << std::endl;
    // }

    std::string buffer = input();

    int siz = buffer.size(), i = 0;
    bool isRunning = true;
    
    while (i < siz && isRunning)
    {
        int newAction = Action(LR, st.stackHead(), buffer[i]);
        if(newAction == ERR){
            std::cout << "error";
            isRunning = false;
        }else if(newAction == 0){
            std::cout << "accept";
            i++;
        }else if(newAction > 0){
            std::cout << "shift" << std::endl;
            st.push(newAction);
            i++;
        }else{
            int senIdx = -newAction;
            std::cout << senIdx << std::endl;
            for(int j = 0; j < sentenceRight[senIdx].size(); j++){
                st.pop();
            }
            newAction = Action(LR, st.stackHead(), sentenceLeft[senIdx]);
            st.push(newAction);
        }
    }
    
    return 0;
}