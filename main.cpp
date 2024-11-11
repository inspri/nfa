//main.cpp
#include "head.h" // 包含提供的头文件

//RE到NFA再到DFA
int main() {
    string Regular_Expression;
    elem NFA_Elem;
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)    Regular_Expression = add_join_symbol(Regular_Expression);
    infixToPostfix Solution(Regular_Expression);
    //中缀转后缀
    cout << "后缀表达式为：";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;
    //表达式转NFA
    NFA_Elem = express_to_NFA(Regular_Expression);
    //显示
    Display(NFA_Elem);
    //生成NFAdot文件
    generateDotFile_NFA(NFA_Elem,"./nfa_graph_re.dot");

    // 初始化 DFA 状态集合和转换关系
    vector<DFAState> dfaStates; //用于存储所有的DFA状态
    vector<DFATransition> dfaTransitions; //用于存储DFA状态之间的转移
    set<string> nfaInitialStateSet;   //存储NFA的初始状态
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);//从NFA构造DFA
    // 显示 DFA
    displayDFA(dfaStates, dfaTransitions);
    //生成DFAdot文件
    generateDotFile_DFA(dfaStates, dfaTransitions, "./dfa_graph_re.dot");
    vector<DFAState>mindfaStates;
    vector<DFATransition>mindfaTransitions;
    minimizeDFA(dfaStates, dfaTransitions, mindfaStates, mindfaTransitions, NFA_Elem.endName.nodeName);
    displayDFA(mindfaStates, mindfaTransitions);
    generateDotFile_DFA(mindfaStates, mindfaTransitions, "./mindfa_re.dot");
    return 0;
}

//int main() {
//	elem NFA_Elem;
//	readNFA("./nfa.txt", NFA_Elem);
//    /*cout << "Start State: " << NFA_Elem.startName.nodeName << endl;
//    cout << "End State: " << NFA_Elem.endName.nodeName << endl;
//    cout << "Edges: " << NFA_Elem.edgeCount << endl;
//    for (const auto& edge : NFA_Elem.edgeSet) {
//        cout << "Edge from " << edge.startName.nodeName
//            << " to " << edge.endName.nodeName
//            << " with symbol '" << edge.tranSymbol << "'" << endl;
//    }*/
//    //显示
//    Display(NFA_Elem);
//    elem NFA_Elem2;
//    readNFA("./nfa1.txt", NFA_Elem2);
//    Display(NFA_Elem2);
//    elem newNFA = mergeNFA(NFA_Elem, NFA_Elem2);
//    Display(newNFA);
//
//    //生成NFAdot文件
//    generateDotFile_NFA(newNFA);
//
//    // 初始化 DFA 状态集合和转换关系
//    vector<DFAState> dfaStates; //用于存储所有的DFA状态
//    vector<DFATransition> dfaTransitions; //用于存储DFA状态之间的转移
//    set<string> nfaInitialStateSet;   //存储NFA的初始状态
//    buildDFAFromNFA(newNFA, dfaStates, dfaTransitions);//从NFA构造DFA
//    // 显示 DFA
//    displayDFA(dfaStates, dfaTransitions);
//    vector<DFAState> mindfaStates; 
//    vector<DFATransition> mindfaTransitions;
//    minimizeDFA(dfaStates, dfaTransitions, mindfaStates, mindfaTransitions, newNFA.endName.nodeName);
//    displayDFA(mindfaStates, mindfaTransitions);
//    //最小化
//    //生成DFAdot文件
//    generateDotFile_DFA(dfaStates, dfaTransitions,"./dfa_graph.dot");
//    generateDotFile_DFA(mindfaStates, mindfaTransitions,"./mindfa.dot");
//    return 0;
//}