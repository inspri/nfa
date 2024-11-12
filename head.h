//head.h
#ifndef HEAD_H
#define HEAD_H

#include<bits/stdc++.h>
#include<unordered_map>

using namespace std;

/*����NFA��DFA����Ҫ�Ľṹ��*/
//NFA�Ľڵ�
struct node
{
	string nodeName;
};

//NFA�ı�
struct edge
{
	node startName;	//��ʼ��
	node endName;	//Ŀ���
	char tranSymbol;	//ת������
};


//NFA����ɵ�Ԫ��һ�����NFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct elem
{
	int edgeCount;	//����
	edge edgeSet[100];	//��NFAӵ�еı�
	node startName;	//��ʼ״̬
	node endName; //����״̬
};

// ���� DFA ��״̬
struct DFAState {
	set<string> nfaStates;	//һ������NFA״̬�ļ���
	string stateName;
};

// ���� DFA ��ת����ϵ
struct DFATransition {
	DFAState fromState;
	DFAState toState;
	char transitionSymbol;
};

/*������ת��ΪDFA����Ҫ����*/

// ���� NFA ״̬�Ħűհ�
DFAState eClosure(const set<string>& nfaStates, elem nfa);

// ���� DFA ��״̬ת��
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa);

// ��� DFA ״̬�Ƿ���״̬������
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState);

//���ת�����Ƿ��ڱ߼����У�����a->b�Ƿ��Ѿ��ڼ�����
bool isTransitionInVector(DFAState, DFAState, char, vector<DFATransition>);

//NFAת��ΪDFA
void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);

// ��ʾ DFA ״̬��ת�ƹ�ϵ
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions);

//����dot�ļ�
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions);


/*�����ǹ���NFA����Ҫ����*/

//��ʾnfa����׼����豸
void Display(elem);

//����nfa��dot�ļ�
void generateDotFile_NFA(const elem& nfa);

//��ȡnfa�ļ�
void readNFA(const string& filename, elem& nfa);

//��������dfa״̬
void renameDFAStates(vector<DFAState>, vector<DFATransition>);

#endif
