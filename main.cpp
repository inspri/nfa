//main.cpp
#include "head.h" // �����ṩ��ͷ�ļ�

int main() {
	elem NFA_Elem;
	readNFA("./nfa.txt", NFA_Elem);

    Display(NFA_Elem);

    //����NFAdot�ļ�
    generateDotFile_NFA(NFA_Elem);

    // ��ʼ�� DFA ״̬���Ϻ�ת����ϵ
    vector<DFAState> dfaStates; //���ڴ洢���е�DFA״̬
    vector<DFATransition> dfaTransitions; //���ڴ洢DFA״̬֮���ת��
    set<string> nfaInitialStateSet;   //�洢NFA�ĳ�ʼ״̬
    buildDFAFromNFA(NFA_Elem, dfaStates, dfaTransitions);//��NFA����DFA
    // ��ʾ DFA
    displayDFA(dfaStates, dfaTransitions);
    //����DFAdot�ļ�
    generateDotFile_DFA(dfaStates, dfaTransitions);
    return 0;
}