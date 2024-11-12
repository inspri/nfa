//main.cpp
#include "head.h" // 包含提供的头文件

int main() {
	elem NFA_Elem;
	readNFA("./nfa.txt", NFA_Elem);

    Display(NFA_Elem);

    //生成NFAdot文件
    generateDotFile_NFA(NFA_Elem);

    // 初始化 DFA 状态集合和转换关系
    vector<DFAState> dfaStates; //用于存储所有的DFA状态
    vector<DFATransition> dfaTransitions; //用于存储DFA状态之间的转移
    set<string> nfaInitialStateSet;   //存储NFA的初始状态
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);//从NFA构造DFA
    // 显示 DFA
    displayDFA(dfaStates, dfaTransitions);
    //生成DFAdot文件
    generateDotFile_DFA(dfaStates, dfaTransitions);
    return 0;
}