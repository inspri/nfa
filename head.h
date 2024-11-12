//head.h
#ifndef HEAD_H
#define HEAD_H

#include<bits/stdc++.h>
#include<unordered_map>

using namespace std;

/*构造NFA和DFA所需要的结构体*/
//NFA的节点
struct node
{
	string nodeName;
};

//NFA的边
struct edge
{
	node startName;	//起始点
	node endName;	//目标点
	char tranSymbol;	//转换符号
};


//NFA的组成单元，一个大的NFA单元可以是由很多小单元通过规则拼接起来
struct elem
{
	int edgeCount;	//边数
	edge edgeSet[100];	//该NFA拥有的边
	node startName;	//开始状态
	node endName; //结束状态
};

// 定义 DFA 的状态
struct DFAState {
	set<string> nfaStates;	//一个包含NFA状态的集合
	string stateName;
};

// 定义 DFA 的转换关系
struct DFATransition {
	DFAState fromState;
	DFAState toState;
	char transitionSymbol;
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
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);


/*下面是构造NFA的主要函数*/

//显示nfa到标准输出设备
void Display(elem);

//生成nfa的dot文件
void generateDotFile_NFA(const elem& nfa);

//读取nfa文件
void readNFA(const string& filename, elem& nfa);

//重新命名dfa状态
void renameDFAStates(vector<DFAState>, vector<DFATransition>);

#endif
