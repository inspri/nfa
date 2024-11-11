//#pragma once
//head.h
#ifndef HEAD_H
#define HEAD_H

#include<bits/stdc++.h>
#include<unordered_map>

using namespace std;

/*����NFA��DFA����Ҫ�Ľṹ��*/
struct node
{
	string nodeName;
	//ÿ����㶼�ж�Ӧ��һ��״̬
	//���Ƿ�ɽ���
	//�ɽ���״̬����̬�����ﶨ��Ϊ1
	//���ɽ���״̬�Ͳ�����̬�����ﶨ��Ϊ0
	//�����ڶ������״̬��ʱ��Ĭ��Ϊ0��������̬
	bool isAccept=0;
};
//NFA�ı�
struct edge
{
	node nfaFrom;	//��ʼ��
	node nfaTo;	//Ŀ���
	char tranSymbol;	//ת������
};


//NFA����ɵ�Ԫ��һ�����NFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct elem
{
	node nfaStartState;//nfa�Ŀ�ʼ״̬
	set<node> nfaNodeSet;//״̬����
	set<edge> nfaEdgeSet;	//nfa�ıߵļ���
};

// ���� DFA ��״̬
struct DFAState {
	set<node> nfaStates;	//һ������NFA״̬�ļ���,�������Ӽ�����nfaת��Ϊdfaʱ���ֵ�nfa״̬�ļ���
	node dfaState;
};


// ���� DFA ��ת����ϵ
struct DFATransition {
	DFAState dfaFrom;//DFA�е�״̬1
	DFAState dfaTo;//DFA�е�״̬2
	char transitionSymbol;//����״̬֮���ת��
};

struct DFA {
	DFAState dfaStartState;//dfa�Ŀ�ʼ״̬
	set<DFAState> dfaNodeSet;//״̬����
	set<DFATransition> dfaEdgeSet;//״̬�ߵļ���
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
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions,const string filename);


/*�����ǹ���NFA����Ҫ����*/
//�����½ڵ�
string new_node();

//���� a
elem act_Elem(char);

//����a|b
elem act_Unit(elem, elem);

//��ɵ�Ԫ��������
void elem_copy(elem&, elem);

//����ab
elem act_join(elem, elem);

//���� a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//������Ԫƴ����һ���൱���м���һ��+����ab�൱��a+b

//���յ���������ʽ����׺���ʽת��Ϊ��׺���ʽ
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

void readNFA(const string& filename, elem& nfa);//��nfa�����ļ��ж�ȡnfa

void renameDFAStates(std::vector<DFAState>& dfaStates, std::vector<DFATransition>& transitions);//������nfa��״̬����Լ���Ӧ��״̬ת�ƹ�ϵ

void minimizeDFA(const vector<DFAState>& dfaStates,
	const vector<DFATransition>& transitions,
	vector<DFAState>& newdfaStates,
	vector<DFATransition>& newtransitions,
	string& endname);
#endif
