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

class item{
public:
    char left, end;
    int idx, dotPos;
    std::string sentence;

    item(char left, char end, int idx, std::string sen): left(left), end(end), idx(idx), sentence(sen){
        this->dotPos = 0;
    }
    item next() const ;
    bool isReduction() const ;
    char expect() const ;
    int itemIdx() const;
    char itemEnd() const;
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

std::set<item> closure(ProductionRule gramma, std::set<item> J){
    std::set<item> ans = J;
    std::vector<item> ansSt;
    for(auto& it : J){
        ansSt.push_back(it);
    }
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
//     ans.push_back('|');
//     for(int i = this->dotPos; i < this->sentence.size(); i++){
//         ans.push_back(sentence[i]);
//     }
//     return ans;
// }

int findItemSet(std::vector<std::set<item>> condition, std::set<item> J){
    int siz = condition.size();
    for(int i = 0; i < siz; i++){
        if(setEqual(condition[i], J)){
            return i;
        }
    }
    return -1;
}

int ERR;
LRtable createLRtable(ProductionRule gramma){
    LRtable ans;
    std::vector<std::set<item>> condition;
    for(auto& it : isBegin){
        if(it.second){
            std::set<item> fir;
            for(auto& ii : gramma[it.first]){
                fir.insert(item(it.first, '$', ii.second, ii.first));
            }
            fir = closure(gramma, fir);
            condition.push_back(fir);
        }
    }

    // for(int i = 0; i < condition.size(); i++){
    //     std::set<item> oneCond = condition[i];
    // }
    for(int i = 0; i < condition.size(); i++){
        auto& it = condition[i];
        std::map<char, std::set<item>> k;
        for(auto& ii : it){
            if(ii.isReduction()){
                ans[i][ii.itemEnd()] = -ii.itemIdx();
            }else{
                k[ii.expect()].insert(ii.next());
            }
        }
        for(auto& ii : k){

            // std::cout << "( " << i << ", " << ii.first << ") = closure( ";
            // std::map<smallItem, std::set<char>> t;
            // for(auto& i : ii.second){
            //     t[smallItem(i.left, i.dotPos, i.sentence)].insert(i.end);
            // }
            // std::cout << " {" ;
            // for(auto& tt : t){
            //     smallItem tmp = tt.first;
            //     std::cout << "[ " << tmp.print() << ", ";
            //     for(auto& i : tt.second){
            //         std::cout << i;
            //     }
            //     std::cout << " ], ";
            // }
            // std::cout << " } )" << std::endl;


            auto tmp = closure(gramma, ii.second);
            int nextCon = findItemSet(condition, tmp);
            if(nextCon == -1){
                nextCon = condition.size();
                condition.push_back(tmp);
            }
            ans[i][ii.first] = nextCon;
        }
    }

    ERR = condition.size();
    // for(int j = 0; j < condition.size(); j++){
    //     auto it = condition[j];
    //     std::map<smallItem, std::set<char>> t;

    //     for(auto& i : it){
    //         t[smallItem(i.left, i.dotPos, i.sentence)].insert(i.end);
    //     }

    //     std::cout << j;

    //     std::cout << "\t :{";
    //     for(auto& tt : t){
    //         smallItem tmp = tt.first;
    //         std::cout << "[ " << tmp.print() << ", ";
    //         for(auto& i : tt.second){
    //             std::cout << i;
    //         }
    //         std::cout << " ], ";
    //     }
    //     std::cout << "}" << std::endl;
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
    LRtable LR = createLRtable(gramma);
    // std::cout << LR.size() << std::endl;

    // for(auto& it : LR){
    //     for(auto& ii : it.second){
    //         std::cout << "go(" << it.first << ", " << ii.first << ") = " << ii.second << "  ";
    //     }
    //     std::cout << std::endl;
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