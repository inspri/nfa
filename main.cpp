//main.cpp
#include "head.h" // �����ṩ��ͷ�ļ�

//RE��NFA�ٵ�DFA
int main() {
    string Regular_Expression;
    elem NFA_Elem;
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)    Regular_Expression = add_join_symbol(Regular_Expression);
    infixToPostfix Solution(Regular_Expression);
    //��׺ת��׺
    cout << "��׺���ʽΪ��";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;
    //���ʽתNFA
    NFA_Elem = express_to_NFA(Regular_Expression);
    //��ʾ
    Display(NFA_Elem);
    //����NFAdot�ļ�
    generateDotFile_NFA(NFA_Elem,"./nfa_graph_re.dot");

    // ��ʼ�� DFA ״̬���Ϻ�ת����ϵ
    vector<DFAState> dfaStates; //���ڴ洢���е�DFA״̬
    vector<DFATransition> dfaTransitions; //���ڴ洢DFA״̬֮���ת��
    set<string> nfaInitialStateSet;   //�洢NFA�ĳ�ʼ״̬
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);//��NFA����DFA
    // ��ʾ DFA
    displayDFA(dfaStates, dfaTransitions);
    //����DFAdot�ļ�
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
//    //��ʾ
//    Display(NFA_Elem);
//    elem NFA_Elem2;
//    readNFA("./nfa1.txt", NFA_Elem2);
//    Display(NFA_Elem2);
//    elem newNFA = mergeNFA(NFA_Elem, NFA_Elem2);
//    Display(newNFA);
//
//    //����NFAdot�ļ�
//    generateDotFile_NFA(newNFA);
//
//    // ��ʼ�� DFA ״̬���Ϻ�ת����ϵ
//    vector<DFAState> dfaStates; //���ڴ洢���е�DFA״̬
//    vector<DFATransition> dfaTransitions; //���ڴ洢DFA״̬֮���ת��
//    set<string> nfaInitialStateSet;   //�洢NFA�ĳ�ʼ״̬
//    buildDFAFromNFA(newNFA, dfaStates, dfaTransitions);//��NFA����DFA
//    // ��ʾ DFA
//    displayDFA(dfaStates, dfaTransitions);
//    vector<DFAState> mindfaStates; 
//    vector<DFATransition> mindfaTransitions;
//    minimizeDFA(dfaStates, dfaTransitions, mindfaStates, mindfaTransitions, newNFA.endName.nodeName);
//    displayDFA(mindfaStates, mindfaTransitions);
//    //��С��
//    //����DFAdot�ļ�
//    generateDotFile_DFA(dfaStates, dfaTransitions,"./dfa_graph.dot");
//    generateDotFile_DFA(mindfaStates, mindfaTransitions,"./mindfa.dot");
//    return 0;
//}