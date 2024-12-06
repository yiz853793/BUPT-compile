#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include <string>
#include <sstream>
#include <cctype>
#include <utility>

using namespace std;

// 定义终结符和非终结符
const set<string> TERMINALS = {"+", "-", "*", "/", "(", ")", "n", "$"};
const set<string> NON_TERMINALS = {"E", "T", "F"};

// 定义产生式
struct Production {
    int number;         // 产生式编号
    string lhs;         // 左部
    vector<string> rhs; // 右部
};

// 产生式列表，编号从0到8
vector<Production> productions = {
    {0, "E'", {"E"}},
    {1, "E", { "E", "+", "T" }},
    {2, "E", { "E", "-", "T" }},
    {3, "E", { "T" }},
    {4, "T", { "T", "*", "F" }},
    {5, "T", { "T", "/", "F" }},
    {6, "T", { "F" }},
    {7, "F", { "(", "E", ")" }},
    {8, "F", { "n" }}
};

// FIRST 集合
map<string, set<string>> FIRST;

// 计算FIRST集合
void compute_FIRST() {
    // 初始化FIRST集
    for(auto terminal : TERMINALS){
        FIRST[terminal].insert(terminal);
    }
    for(auto non_terminal : NON_TERMINALS){
        FIRST[non_terminal] = set<string>();
    }

    bool changed = true;
    while(changed){
        changed = false;
        for(auto &prod : productions){
            string A = prod.lhs;
            vector<string> alpha = prod.rhs;
            // 遍历产生式 A -> alpha
            if(alpha.empty()){
                // 如果产生式右部为空，添加ε (这里假设没有ε产生式)
                // FIRST[A].insert("ε");
            }
            else{
                // 对于每个符号
                bool all_nullable = true;
                for(auto symbol : alpha){
                    // 将FIRST(symbol) - {ε} 加入FIRST(A)
                    size_t before = FIRST[A].size();
                    for(auto s : FIRST[symbol]){
                        if(s != "ε"){
                            FIRST[A].insert(s);
                        }
                    }
                    size_t after = FIRST[A].size();
                    if(after > before){
                        changed = true;
                    }
                    // 如果symbol的FIRST不包含ε，停止
                    if(FIRST[symbol].find("ε") == FIRST[symbol].end()){
                        all_nullable = false;
                        break;
                    }
                }
                if(all_nullable){
                    // 如果所有symbol的FIRST都包含ε，添加ε到FIRST(A)
                    if(FIRST[A].find("ε") == FIRST[A].end()){
                        FIRST[A].insert("ε");
                        changed = true;
                    }
                }
            }
        }
    }
}

// 计算字符串的FIRST集合
set<string> first_of_string(const vector<string>& symbols){
    set<string> result;
    if(symbols.empty()){
        result.insert("ε");
        return result;
    }
    bool all_nullable = true;
    for(auto symbol : symbols){
        for(auto s : FIRST[symbol]){
            if(s != "ε"){
                result.insert(s);
            }
        }
        if(FIRST[symbol].find("ε") == FIRST[symbol].end()){
            all_nullable = false;
            break;
        }
    }
    if(all_nullable){
        result.insert("ε");
    }
    return result;
}

// 定义LR(1)项
struct Item {
    int prod_num;       // 产生式编号
    int dot_pos;        // 点的位置
    string lookahead;   // 向前看符号

    bool operator<(const Item& other) const{
        if(prod_num != other.prod_num)
            return prod_num < other.prod_num;
        if(dot_pos != other.dot_pos)
            return dot_pos < other.dot_pos;
        return lookahead < other.lookahead;
    }

    bool operator==(const Item& other) const{
        return prod_num == other.prod_num && dot_pos == other.dot_pos && lookahead == other.lookahead;
    }
};

// 闭包操作
set<Item> closure(const set<Item>& items){
    set<Item> closure_set = items;
    bool changed = true;
    while(changed){
        changed = false;
        set<Item> new_items;
        for(auto &item : closure_set){
            if(item.dot_pos < productions[item.prod_num].rhs.size()){
                string B = productions[item.prod_num].rhs[item.dot_pos];
                if(NON_TERMINALS.find(B) != NON_TERMINALS.end()){
                    // βa
                    vector<string> beta_a;
                    for(int i = item.dot_pos +1; i < productions[item.prod_num].rhs.size(); ++i){
                        beta_a.push_back(productions[item.prod_num].rhs[i]);
                    }
                    beta_a.push_back(item.lookahead);
                    // 计算FIRST(beta a)
                    set<string> first_beta_a = first_of_string(beta_a);
                    // 对每个产生式 B -> γ
                    for(auto &prod : productions){
                        if(prod.lhs == B){
                            for(auto la : first_beta_a){
                                if(la != "ε"){
                                    Item new_item = {prod.number, 0, la};
                                    if(closure_set.find(new_item) == closure_set.end()){
                                        new_items.insert(new_item);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if(!new_items.empty()){
            closure_set.insert(new_items.begin(), new_items.end());
            changed = true;
        }
    }
    return closure_set;
}

// GOTO操作
set<Item> GOTO_func(const set<Item>& items, const string& X){
    set<Item> goto_set;
    for(auto &item : items){
        if(item.dot_pos < productions[item.prod_num].rhs.size() && productions[item.prod_num].rhs[item.dot_pos] == X){
            Item moved_item = item;
            moved_item.dot_pos += 1;
            goto_set.insert(moved_item);
        }
    }
    return closure(goto_set);
}

// Canonical Collection of LR(1) items
vector<set<Item>> C; // 状态集
map< set<Item>, int, less<set<Item>> > state_map;
map<pair<int, string>, int> transitions;

// 构造LR(1)项集规范族
void construct_Canonical_Collection(){
    // 初始项集
    Item start_item = {0, 0, "$"};
    set<Item> start_set = {start_item};
    set<Item> closure_start = closure(start_set);
    C.push_back(closure_start);
    state_map[closure_start] = 0;

    // BFS
    queue<int> q;
    q.push(0);

    while(!q.empty()){
        int state_id = q.front();
        q.pop();
        set<Item> current = C[state_id];
        // 遍历所有可能的符号
        set<string> symbols;
        for(auto &item : current){
            if(item.dot_pos < productions[item.prod_num].rhs.size()){
                symbols.insert(productions[item.prod_num].rhs[item.dot_pos]);
            }
        }
        for(auto &X : symbols){
            set<Item> goto_X = GOTO_func(current, X);
            if(goto_X.empty()) continue;
            // 检查是否已经存在
            if(state_map.find(goto_X) == state_map.end()){
                C.push_back(goto_X);
                int new_state = C.size() -1;
                state_map[goto_X] = new_state;
                q.push(new_state);
            }
            // 添加转移
            transitions[{state_id, X}] = state_map[goto_X];
        }
    }
}

// ACTION 和 GOTO 表
map<int, map<string, pair<string, int>>> ACTION_TABLE; // ACTION[s][a] = (action, number)
map<int, map<string, int>> GOTO_TABLE;

// 构建ACTION和GOTO表
bool construct_tables(){
    bool conflict = false;
    for(int i = 0; i < C.size(); ++i){
        for(auto &item : C[i]){
            string A = productions[item.prod_num].lhs;
            vector<string> alpha = productions[item.prod_num].rhs;
            if(item.dot_pos < alpha.size()){
                string a = alpha[item.dot_pos];
                if(TERMINALS.find(a) != TERMINALS.end()){
                    // ACTION[i][a] = shift s
                    if(transitions.find({i, a}) != transitions.end()){
                        int s = transitions[{i, a}];
                        // Check for existing action
                        if(ACTION_TABLE[i].find(a) != ACTION_TABLE[i].end()){
                            // 冲突
                            if(!(ACTION_TABLE[i][a].first == "shift" && ACTION_TABLE[i][a].second == s)){
                                cout << "冲突在状态 " << i << ", 符号 " << a << endl;
                                conflict = true;
                            }
                        }
                        else{
                            ACTION_TABLE[i][a] = {"shift", s};
                        }
                    }
                }
            }
            else{
                if(A != "E'"){
                    // ACTION[i][a] = reduce A -> alpha
                    // a 是lookahead
                    string a = item.lookahead;
                    if(ACTION_TABLE[i].find(a) != ACTION_TABLE[i].end()){
                        // 冲突
                        pair<string, int> existing = ACTION_TABLE[i][a];
                        if(!(existing.first == "reduce" && existing.second == item.prod_num)){
                            cout << "冲突在状态 " << i << ", 符号 " << a << endl;
                            conflict = true;
                        }
                    }
                    else{
                        ACTION_TABLE[i][a] = {"reduce", item.prod_num};
                    }
                }
                else{
                    // ACTION[i][$] = accept
                    if(item.lookahead == "$"){
                        if(ACTION_TABLE[i].find("$") != ACTION_TABLE[i].end()){
                            if(ACTION_TABLE[i]["$"].first != "accept"){
                                cout << "冲突在状态 " << i << ", 符号 $" << endl;
                                conflict = true;
                            }
                        }
                        else{
                            ACTION_TABLE[i]["$"] = {"accept", -1};
                        }
                    }
                }
            }
        }
        // GOTO 表
        for(auto &nt : NON_TERMINALS){
            if(transitions.find({i, nt}) != transitions.end()){
                GOTO_TABLE[i][nt] = transitions[{i, nt}];
            }
        }
    }
    return conflict;
}

// 词法分析器：将输入字符串分割为符号
vector<string> lexer(const string& input){
    vector<string> tokens;
    int i = 0;
    while(i < input.size()){
        char c = input[i];
        if(isspace(c)){
            i++;
            continue;
        }
        if(c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')'){
            string s(1, c);
            tokens.push_back(s);
            i++;
        }
        else if(c == 'n'){
            tokens.push_back("n");
            i++;
        }
        else{
            // 处理未知符号
            cout << "未知的符号：" << c << endl;
            // 为了简单，直接跳过
            i++;
        }
    }
    tokens.push_back("$"); // 添加结束符
    return tokens;
}

// 解析函数
bool parse(const vector<string>& tokens, vector<string>& action_sequence){
    vector<int> stack;
    stack.push_back(0);
    int index = 0;
    vector<string> input = tokens;
    while(true){
        int state = stack.back();
        string a = input[index];
        // 获取ACTION[state][a]
        if(ACTION_TABLE.find(state) == ACTION_TABLE.end() || ACTION_TABLE[state].find(a) == ACTION_TABLE[state].end()){
            action_sequence.push_back("error");
            return false;
        }
        pair<string, int> action = ACTION_TABLE[state][a];
        if(action.first == "shift"){
            action_sequence.push_back("shift");
            stack.push_back(action.second);
            index++;
        }
        else if(action.first == "reduce"){
            // 记录归约的产生式编号
            action_sequence.push_back(to_string(action.second));
            Production prod = productions[action.second];
            // 弹出 |rhs| 个状态
            if(!(prod.rhs.size() ==1 && prod.rhs[0] == "ε")){
                for(int i = 0; i < prod.rhs.size(); ++i){
                    if(stack.empty()){
                        action_sequence.push_back("error");
                        return false;
                    }
                    stack.pop_back();
                }
            }
            // 获取新的状态
            if(stack.empty()){
                action_sequence.push_back("error");
                return false;
            }
            int current_state = stack.back();
            if(GOTO_TABLE.find(current_state) == GOTO_TABLE.end() || GOTO_TABLE[current_state].find(prod.lhs) == GOTO_TABLE[current_state].end()){
                action_sequence.push_back("error");
                return false;
            }
            int new_state = GOTO_TABLE[current_state][prod.lhs];
            stack.push_back(new_state);
        }
        else if(action.first == "accept"){
            action_sequence.push_back("accept");
            return true;
        }
    }
}

// 主函数
int main(){
    // 计算FIRST集合
    compute_FIRST();

    // 构造项集规范族
    construct_Canonical_Collection();

    // 构造ACTION和GOTO表
    bool conflict = construct_tables();
    if(conflict){
        // 存在冲突，文法不是LR(1)
        return 1;
    }

    // 解析输入
    string line;
    // 提示用户输入
    // cout << "\n请输入算术表达式（例如：n+n）：" << endl;
    getline(cin, line);
    // 词法分析
    vector<string> tokens = lexer(line);
    // 解析并记录操作序列
    vector<string> action_sequence;
    bool result = parse(tokens, action_sequence);
    // 输出操作序列
    for(auto &action : action_sequence){
        cout << action << endl;
    }
    // 根据结果输出
    if(result){
        // 已经在action_sequence中添加了"accept"
        // 不需要额外输出
    }
    else{
        // 已经在action_sequence中添加了"error"
        // 不需要额外输出
    }
    return 0;
}
