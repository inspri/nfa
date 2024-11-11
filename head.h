//#pragma once
//head.h
#ifndef HEAD_H
#define HEAD_H

#include<bits/stdc++.h>
#include<unordered_map>

using namespace std;

/*构造NFA和DFA所需要的结构体*/
struct node
{
	string nodeName;
	//每个结点都有对应的一个状态
	//即是否可接受
	//可接受状态即终态，这里定义为1
	//不可接受状态就不是终态，这里定义为0
	//这里在定义这个状态的时候默认为0，即非终态
	bool isAccept=0;
};
//NFA的边
struct edge
{
	node nfaFrom;	//起始点
	node nfaTo;	//目标点
	char tranSymbol;	//转换符号
};


//NFA的组成单元，一个大的NFA单元可以是由很多小单元通过规则拼接起来
struct elem
{
	node nfaStartState;//nfa的开始状态
	set<node> nfaNodeSet;//状态集合
	set<edge> nfaEdgeSet;	//nfa的边的集合
};

// 定义 DFA 的状态
struct DFAState {
	set<node> nfaStates;	//一个包含NFA状态的集合,在利用子集法将nfa转化为dfa时出现的nfa状态的集合
	node dfaState;
};


// 定义 DFA 的转换关系
struct DFATransition {
	DFAState dfaFrom;//DFA中的状态1
	DFAState dfaTo;//DFA中的状态2
	char transitionSymbol;//两个状态之间的转换
};

struct DFA {
	DFAState dfaStartState;//dfa的开始状态
	set<DFAState> dfaNodeSet;//状态集合
	set<DFATransition> dfaEdgeSet;//状态边的集合
};

/*下面是转换为DFA的主要函数*/

// 计算 NFA 状态的ε闭包
DFAState eClosure(const set<string>& nfaStates, elem nfa);

// 计算 DFA 的状态转移
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa);

// 检查 DFA 状态是否在状态集合中
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState);

//检查转换边是否在边集合中，比如a->b是否已经在集合中
bool isTransitionInVector(DFAState, DFAState, char, vector<DFATransition>);

//NFA转换为DFA
void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

// 显示 DFA 状态和转移关系
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

//生成dot文件
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions,const string filename);


/*下面是构造NFA的主要函数*/
//创建新节点
string new_node();

//处理 a
elem act_Elem(char);

//处理a|b
elem act_Unit(elem, elem);

//组成单元拷贝函数
void elem_copy(elem&, elem);

//处理ab
elem act_join(elem, elem);

//处理 a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//两个单元拼接在一起相当于中间有一个+，如ab相当于a+b

//接收到的正则表达式从中缀表达式转化为后缀表达式
class infixToPostfix {
public:
	infixToPostfix(const string& infix_expression);

	int is_letter(char check);
	int ispFunc(char c);
	int icpFunc(char c);
	void infToPost();
	string getResult();

private:
	string infix;
	string postfix;
	map<char, int> isp;
	map<char, int> icp;
};

elem express_to_NFA(string);

void Display(elem);

int is_letter(char check);

void generateDotFile_NFA(const elem& nfa,string filename);

void readNFA(const string& filename, elem& nfa);//从nfa描述文件中读取nfa

void renameDFAStates(std::vector<DFAState>& dfaStates, std::vector<DFATransition>& transitions);//重命名nfa的状态结点以及对应的状态转移关系

void minimizeDFA(const vector<DFAState>& dfaStates,
	const vector<DFATransition>& transitions,
	vector<DFAState>& newdfaStates,
	vector<DFATransition>& newtransitions,
	string& endname);
#endif
