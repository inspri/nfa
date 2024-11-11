//func.cpp
#include "head.h"

int nodeNum = 0;

/*������ת��ΪDFA����Ҫ����*/

// ���� NFA ״̬�Ħűհ�
DFAState eClosure(const set<node>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<node> stateStack;

	// ��ʼ��ջ������ʼ״̬����ջ���ʼnfaState��ֻ��NFA_Elem.startName
	for (const auto& nfaState : nfaStates) {
		stateStack.push(nfaState);
	}

	while (!stateStack.empty()) {
		node currentState = stateStack.top();
		stateStack.pop();

		// ���� NFA �ı�
		for (const auto& currentEdge : nfa.nfaEdgeSet ) {

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬�����ұߵ�ת��������#����ô��Ŀ��״̬����űհ�
			if (currentEdge.nfaFrom.nodeName == currentState.nodeName && currentEdge.tranSymbol == '#') {
				auto result=eClosureState.nfaStates.insert(currentEdge.nfaTo);
				if (result.second) {
					// ��Ŀ��״̬����ջ�Ա��һ������
					stateStack.push(currentEdge.nfaTo);
				}
			}
		}
	}

	// Ϊ�űհ�����һ��Ψһ������,�����ж��µõ������dfa״̬�Ƿ�Ϊ��̬
	for (const auto& nfaState : eClosureState.nfaStates) {
		eClosureState.dfaState.nodeName += nfaState.nodeName;
		//nfa״̬�����Ƿ������̬
		if (nfaState.isAccept) {
			//������µ�״̬Ҳ����Ϊ��̬
			eClosureState.dfaState.isAccept = 1;
		}
	}

	return eClosureState;
}

//move����
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// ���� DFAState �е�ÿ�� NFA ״̬
	for (const auto& nfaState : dfaState.nfaStates) {
		// ������������� NFA ״̬�ı�
		for (const auto& currentEdge : nfa.nfaEdgeSet) {

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬���ұߵ�ת�����ŵ���������ţ���Ŀ��״̬���� nextState
			if (currentEdge.nfaFrom.nodeName == nfaState.nodeName && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.nfaTo);
			}
		}
	}

	// Ϊ nextState ����һ��Ψһ������
	for (const auto& nfaState : nextState.nfaStates) {
		nextState.dfaState.nodeName += nfaState.nodeName;
		if (nfaState.isAccept) {
			nextState.dfaState.isAccept = 1;
		}
	}

	return nextState;
}

DFA buildDFAFromNFA(const elem& NFA_Elem) {
	DFA newDFA;
	set<node> nfaInitialStateSet;
	nfaInitialStateSet.insert(NFA_Elem.nfaStartState);
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // ���� NFA ��ʼ״̬�� �űհ�
	//�õ��ľ���DFA�Ŀ�ʼ״̬
	newDFA.dfaStartState = dfaInitialState;
	set<DFAState> dfaStates;
	dfaStates.insert(dfaInitialState);
	set<DFATransition> dfaTransitions;
	// ��ʼ���� DFA
	for (const auto& dfaState : dfaStates) {
		//����nfa�е����бߣ���Ϊ����Ľṹ����û��ͳ�Ƴ����е�ת���ַ�
		for (const auto& currentNfaEdge : NFA_Elem.nfaEdgeSet) {
			char symbol = currentNfaEdge.tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// �����һ��״̬��Ϊ�գ����� DFA ״̬�����л�δ��ӣ������ DFA ״̬����
				//ʹ��set���Ͽ����Զ��������Ԫ���Ƿ��ظ�
				//�������set�е�Ԫ����һ�����ӽṹ�壬��ô����Ҫ���¶���set�еıȽ������
				dfaStates.insert(dfaNextState);
				dfaTransitions.insert({ dfaState, dfaNextState, symbol });
			}
		}
	}

	//��ϳ�һ��������DFA
	newDFA.dfaEdgeSet = dfaTransitions;
	newDFA.dfaNodeSet = dfaStates;
	return newDFA;
}

// ��ʾ DFA ״̬��ת�ƹ�ϵ��������ʼ�ͽ���״̬
void displayDFA(const DFA dfa) {
	cout << "DFA States:" << endl;
	for (const DFAState& state : dfa.dfaNodeSet) {
		cout << "State " << state.dfaState.nodeName << " (NFA States: ";
		for (const node& dfaState : state.nfaStates) {
			cout << dfaState.nodeName << " ";
		}
		cout << ")";
		if (state.dfaState.nodeName == dfa.dfaStartState.dfaState.nodeName) {
			cout << " (Initial State)";
		}
		if (state.dfaState.isAccept) {
			cout << " (Final State)";
		}
		cout << endl;
	}

	cout << "DFA Transitions:" << endl;
	for (const DFATransition& transition : dfa.dfaEdgeSet) {
		cout << "State " << transition.dfaFrom.dfaState.nodeName << " --(" << transition.transitionSymbol << ")--> State " << transition.dfaTo.dfaState.nodeName << endl;
	}
}

//����DFA��dot�ļ�
void generateDotFile_DFA(const DFA dfa,const string filename) {
	std::ofstream dotFile(filename);

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ��ʼ״̬\n\n";
		for (const auto& endstate : dfa.dfaNodeSet) {
			if (endstate.dfaState.isAccept) {
				dotFile << endstate.dfaState.nodeName << "[shape = doublecircle];\n";
			}
		}
		// ���DFA״̬
		for (const auto& state : dfa.stateSet) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.stateName == dfa.dfaStart.stateName) dotFile << "\\n(startState)";
			if (dfa.dfaEnd.find(state) != dfa.dfaEnd.end()) {
				dotFile << "\\n(endState)";
			}
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// ���DFAת��
		for (const auto& transition : dfa.edgeSet) {
			dotFile << "  " << transition.fromState.stateName << " -> " << transition.toState.stateName << " [label=\"" << transition.transitionSymbol << "\"];\n";
		}

		dotFile << "}\n";

		dotFile.close();
		std::cout << "DFA DOT file generated successfully.\n";
	}
	else {
		std::cerr << "Unable to open DOT file.\n";
	}
}

/*�����ǹ���NFA����Ҫ����*/

//�����½ڵ�
string new_node()
{
	string newNode;
	newNode = nodeNum + 65;//�������ô�д��ĸ��ʾ
	nodeNum++;
	return newNode;
}

//��������������ʽ
void input(string& RE)
{
	cout << "������������ʽ��  ����������() * |;�ַ�����a~z A~Z��" << endl;
	cin >> RE;
}


//��ɵ�Ԫ��������
//��NFA�ĺϲ�
elem elem_merge(elem& dest, const elem& source)
{
	// �ϲ�״̬���ϣ������ظ�״̬��
	dest.nodeSet.insert(source.nodeSet.begin(), source.nodeSet.end());

	// �ϲ��߼��ϣ������ظ��ߣ�
	dest.edgeSet.insert(source.edgeSet.begin(), source.edgeSet.end());

	// �ϲ�����״̬����
	dest.endName.insert(source.endName.begin(), source.endName.end());

	// ���Ŀ��Ԫ�صĿ�ʼ״̬Ϊ�գ�ʹ��ԴԪ�صĿ�ʼ״̬
	if (dest.startName.empty()||dest.startName==source.startName) {
		dest.startName = source.startName;
	}
	else {
		//�������nfa���п�ʼԪ�أ�����������ʼԪ�ز���ͬ
		string newStartName = new_node();
		dest.nodeSet.insert(dest.startName);
		dest.edgeSet.insert({ newStartName,dest.startName,'#' });
		dest.edgeSet.insert({ newStartName,source.startName,'#' });
		dest.startName = newStartName;
	}

	// ����״̬����
	dest.stateCount = dest.nodeSet.size();

	// ���±�����
	dest.edgeCount = dest.edgeSet.size();
	//�ϲ����NFAʱ
	//ͨ���ᴴ��һ���µ���ʼ״̬������ÿ��NFA����ʼ״̬ͨ����-ת�����ӵ��������ʼ״̬��
	//�ϲ������ֹ״̬�����Խ�������ֹ״̬�ϲ�Ϊһ���µ���ֹ״̬��
	//��������ͨ��ͨ�����һ���µ���ֹ״̬������ԭ����ֹ״̬����״̬֮����Ӧ�-ת����ʵ�֡�
	elem newNFA;
	// �ж���ʼ״̬�Ƿ�һ��
	node newStart;
	newNFA.edgeCount = 0;
	if (nfa1.startName.nodeName != nfa2.startName.nodeName) {
		newStart.nodeName = "newStart"; // ���Ը�����Ҫ����Ψһ������
		// ��Ӧ�-ת����nfa1��nfa2����ʼ״̬
		newNFA.edgeSet[newNFA.edgeCount++] = { newStart, nfa1.startName, '#' };
		newNFA.edgeSet[newNFA.edgeCount++] = { newStart, nfa2.startName, '#' };
	}
	else {
		newStart = nfa1.startName; // ʹ�ù�ͬ����ʼ״̬
	}

	// �ж���ֹ״̬�Ƿ�һ��
	node newEnd;
	if (nfa1.endName.nodeName != nfa2.endName.nodeName) {
		newEnd.nodeName = "newEnd"; // ���Ը�����Ҫ����Ψһ������
	}
	else {
		newEnd = nfa1.endName; // ʹ�ù�ͬ����ֹ״̬
	}

	// ��nfa1�ı߸��Ƶ���NFA��
	for (int i = 0; i < nfa1.edgeCount; ++i) {
		newNFA.edgeSet[newNFA.edgeCount++] = nfa1.edgeSet[i];
	}

	// ��nfa2�ı߸��Ƶ���NFA��
	for (int i = 0; i < nfa2.edgeCount; ++i) {
		newNFA.edgeSet[newNFA.edgeCount++] = nfa2.edgeSet[i];
	}

	// ��nfa1��nfa2����ֹ״̬ͨ����-ת�����ӵ��µ���ֹ״̬
	if (nfa1.endName.nodeName != newEnd.nodeName) {
		newNFA.edgeSet[newNFA.edgeCount++] = { nfa1.endName, newEnd, '#' };
	}
	if (nfa2.endName.nodeName != newEnd.nodeName) {
		newNFA.edgeSet[newNFA.edgeCount++] = { nfa2.endName, newEnd, '#' };
	}

	// �����µ���ʼ����ֹ״̬
	newNFA.startName = newStart;
	newNFA.endName = newEnd;

	return newNFA;
}

//���� a
elem act_Elem(char c)
{
	//�½ڵ�
	string startNode = new_node();
	string endNode = new_node();

	//�±�
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//��NFA���Ԫ�أ�С��NFAԪ��/��Ԫ)
	elem newElem;
	newElem.edgeCount = 0;	//��ʼ״̬
	newElem.edgeSet.insert(newEdge);
	newElem.edgeCount++;
	newElem.startName = newEdge.startName;
	newElem.endName.insert(newEdge.endName);

	return newElem;
}

//����a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//����µ�״̬�ڵ�
	string startNode = new_node();
	string endNode = new_node();

	//����e1����������AB����ʼ��A��
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//����e2����������CD����ʼ��C��
	edge2.startName = startNode;
	edge2.endName = sec.startName;
	edge2.tranSymbol = '#';

	//����e3������AB���յ���յ㣩
	edge3.startName = *fir.endName.begin();
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//����e4������CD���յ���յ㣩
	edge4.startName = *sec.endName.begin();
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//��fir��sec�ϲ�
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//�¹�����4����
	newElem.edgeSet.insert(edge1);
	newElem.edgeSet.insert(edge2);
	newElem.edgeSet.insert(edge3);
	newElem.edgeSet.insert(edge4);
	newElem.edgeCount += 4;

	newElem.startName = startNode;
	newElem.endName.insert(endNode);

	return newElem;
}

//���� N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//��fir�Ľ���״̬��sec�Ŀ�ʼ״̬�ϲ�����sec�ı߸��Ƹ�fir����fir����
	//��sec��������StartState��ͷ�ı�ȫ���޸�
	for (auto tempEdge : sec.edgeSet) {
		if (tempEdge.startName.compare(sec.startName) == 0)
		{
			tempEdge.startName = *fir.endName.begin(); //�ñ�e1�Ŀ�ʼ״̬����N(t)����ʼ״̬
		}
		else if (*tempEdge.endName.begin().compare(sec.startName) == 0) {
			tempEdge.endName  fir.endName; //�ñ�e2�Ľ���״̬����N(t)����ʼ״̬
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//��fir�Ľ���״̬����Ϊsec�Ľ���״̬
	fir.endName = sec.endName;
	return fir;
}

//����a*
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//�����״̬�ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//�հ�ȡ�մ�

	//e2
	edge2.startName = Elem.endName;
	edge2.endName = Elem.startName;
	edge2.tranSymbol = '#';

	//e3
	edge3.startName = startNode;
	edge3.endName = Elem.startName;
	edge3.tranSymbol = '#';

	//e4
	edge4.startName = Elem.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//������Ԫ
	elem_copy(newElem, Elem);

	//���¹����������߼���EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//����NewElem����ʾ״̬�ͽ���״̬
	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}

int is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}
//
string add_join_symbol(string add_string)
{
	int length = add_string.size();
	int return_string_length = 0;
	char* return_string = new char[2 * length + 2];//���������
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//Ҫ�ӵĿ�������ab �� *b �� a( �� )b �����
		//���ڶ�������ĸ����һ������'('��'|'��Ҫ���
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//���ڶ�����'(',��һ������'|'��'(',ҲҪ��
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//�����һ���ַ�д��second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "��'+'��ı��ʽ��" << STRING << endl;
	return STRING;
}

//����ĸ���Ԫ�ض���
infixToPostfix::infixToPostfix(const string& infix_expression) : infix(infix_expression), postfix("") {
	isp = { {'+', 3}, {'|', 5}, {'*', 7},  {'(', 1}, {')', 8}, {'#', 0} };
	icp = { {'+', 2}, {'|', 4}, {'*', 6}, {'(', 8}, {')', 1}, {'#', 0} };
}

int infixToPostfix::is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}

int infixToPostfix::ispFunc(char c) {
	int priority = isp.count(c) ? isp[c] : -1;
	if (priority == -1) {
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
	}
	return priority;
}

void infixToPostfix::infToPost() {
	string infixWithHash = infix + "#";
	stack<char> stack;
	int loc = 0;
	while (!stack.empty() || loc < infixWithHash.size()) {
		if (is_letter(infixWithHash[loc])) {
			postfix += infixWithHash[loc];
			loc++;
		}
		else {
			char c1 = (stack.empty()) ? '#' : stack.top();
			char c2 = infixWithHash[loc];
			if (ispFunc(c1) < icpFunc(c2)) {
				stack.push(c2);
				loc++;
			}
			else if (ispFunc(c1) > icpFunc(c2)) {
				postfix += c1;
				stack.pop();
			}
			else {
				if (c1 == '#' && c2 == '#') {
					break;
				}
				stack.pop();
				loc++;
			}
		}
	}
}

string infixToPostfix::getResult() {
	postfix = ""; // ��ս��
	infToPost();
	return postfix;
}

/**���ʽתNFA������,�������յ�NFA����
*/
elem express_to_NFA(string expression)
{
	int length = expression.size();
	char element;
	elem Elem, fir, sec;
	stack<elem> STACK;
	for (int i = 0; i < length; i++)
	{
		element = expression.at(i);
		switch (element)
		{
		case '|':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_Unit(fir, sec);
			STACK.push(Elem);
			break;
		case '*':
			fir = STACK.top();
			STACK.pop();
			Elem = act_star(fir);
			STACK.push(Elem);
			break;
		case '+':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_join(fir, sec);
			STACK.push(Elem);
			break;
		default:
			Elem = act_Elem(element);
			STACK.push(Elem);
		}
	}
	cout << "�ѽ�������ʽת��ΪNFA!" << endl;
	Elem = STACK.top();
	STACK.pop();

	return Elem;
}

//��ӡNFA
void Display(elem Elem) {
	cout << "NFA States:" << endl;
	cout << "Start State: " << Elem.startName.nodeName << endl;
	cout << "End State: " << Elem.endName.nodeName << endl;

	cout << "NFA Transitions:" << endl;
	for (int i = 0; i < Elem.edgeCount; i++) {
		cout << "Edge " << i + 1 << ": ";
		cout << Elem.edgeSet[i].startName.nodeName << " --(" << Elem.edgeSet[i].tranSymbol << ")--> ";
		cout << Elem.edgeSet[i].endName.nodeName << endl;
	}

	cout << "End" << endl;
}

//����NFAdot�ļ�
void generateDotFile_NFA(const elem& nfa,string filename) {
	std::ofstream dotFile(filename);

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ״̬�ڵ�\n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// ��� NFA ״̬
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// ��� NFA ת��
		for (int i = 0; i < nfa.edgeCount; i++) {
			const edge& currentEdge = nfa.edgeSet[i];
			dotFile << "  " << currentEdge.startName.nodeName << " -> " << currentEdge.endName.nodeName << " [label=\"" << currentEdge.tranSymbol << "\"];\n";
		}

		dotFile << "}\n";

		dotFile.close();
		std::cout << "NFA DOT file generated successfully.\n";
	}
	else {
		std::cerr << "Unable to open NFA DOT file.\n";
	}
}

void readNFA(const string& filename, elem& nfa) {
	ifstream file(filename);
	if (!file.is_open()) {
		cerr << "Unable to open file: " << filename << endl;
		return;
	}

	string line;
	int index = 0;

	// ����ǰ����
	for (int i = 0; i < 4; ++i) {
		getline(file, line);
	}

	// ��ȡ�ߵĶ���
	while (getline(file, line) && !line.empty()) {
		edge e;

		std::istringstream iss(line);
		int nodeIndex;
		iss >> nodeIndex;
		e.startName.nodeName = to_string(nodeIndex);
		// ����ߵĶ���
		int dest;
		std::vector<int> destinations; // �洢Ŀ�ĵ�
		while (iss >> dest) {
			destinations.push_back(dest);
		}

		// ȷ����һ���ߵ� tranSymbol �� destinations ��ȡ
		if (!destinations.empty()) {
			// ȡ�� tranSymbol
			e.tranSymbol = (destinations[0] == -1) ? '#' : to_string(destinations[0])[0];

			// ����������Ŀ�ĵ�
			for (size_t i = 1; i < destinations.size(); ++i) {
				e.endName.nodeName = to_string(destinations[i]);
				nfa.edgeSet[index++] = e; // �����
			}
		}
	}
	nfa.edgeCount = index;


	// ��ȡ��ʼ״̬����ֹ״̬
	if (getline(file, line) && !line.empty()) {
		nfa.startName.nodeName = line;
	}

	if (getline(file, line) && !line.empty()) {
		nfa.endName.nodeName = line;
	}

	file.close();
}

// �������� DFA ״̬�ĺ���
void renameDFAStates(std::vector<DFAState>& dfaStates, std::vector<DFATransition>& transitions) {
	std::unordered_map<std::string, std::string> nameMap; // �����Ƶ������Ƶ�ӳ��
	int counter = 0; // �������������Ƶļ�����

	// ��һ����������״̬
	for (auto& state : dfaStates) {
		std::ostringstream newName;
		newName << "q" << counter++; // ���������ƣ��� q0, q1, q2...
		nameMap[state.stateName] = newName.str(); // ӳ������Ƶ�������
		state.stateName = newName.str(); // ����״̬����
	}

	// �ڶ���������ת����ϵ�е�״̬����
	for (auto& transition : transitions) {
		// ���� fromState �� toState ������
		transition.fromState.stateName = nameMap[transition.fromState.stateName];
		transition.toState.stateName = nameMap[transition.toState.stateName];
	}
}

// �ж�����״̬�Ƿ�ȼ�
bool areEquivalent(const DFAState& state1, const DFAState& state2,
	const std::unordered_map<std::string, std::unordered_map<char, DFAState>>& transitions) {
	for (const auto& entry : transitions.at(state1.stateName)) {
		const auto& symbol = entry.first;
		const auto& toState1 = entry.second;
		const auto& toState2 = transitions.at(state2.stateName).at(symbol);

		if (toState1.stateName != toState2.stateName) {
			return false;
		}
	}
	return true;
}

// ��С�� DFA
void minimizeDFA(const vector<DFAState>& dfaStates,
	const vector<DFATransition>& transitions,
	vector<DFAState>& newdfaStates,
	vector<DFATransition>& newtransitions,
	string& endname) {

	// ����DFA״̬��ת��ӳ��
	unordered_map<DFAState, unordered_map<char, DFAState>> transitionMap;
	for (const auto& transition : transitions) {
		//״̬1����ת������״̬2
		transitionMap[transition.fromState][transition.transitionSymbol] = transition.toState;
	}

	// 1. ��ʼ�����ܺͷǽ���״̬����
	set<DFAState> acceptStates, nonAcceptStates;
	//���ﶨ���״̬�����д洢��Ҳ�Ƕ����dfa״̬�����ݽṹ����
	//�����ǵ�����һ��string���͵����洢dfa״̬������
	//����Ҫ�洢���״̬��Ӧnfa�е���Щ״̬��ɵ�һ������
	
	//����dfa��״̬����ͬʱ��dfa��״̬���Ϸֳ���������
	//����״̬�ͷǽ���״̬�ֱ��ʾ�ľ���dfa״̬ͼ�е���̬�ͷ���̬
	for (const auto& state : dfaStates) {
		if (state.nfaStates.find(endname) != state.nfaStates.end()) {
			//���dfa��һ��״̬�У�ͨ���Ӽ�����nfaת�������ģ�
			//����nfa����̬����ô���״̬��nfa��Ҳ�ǽ���״̬������̬��
			acceptStates.insert(state);
		}
		else {
			nonAcceptStates.insert(state);
		}
	}

	// ��ʼ����Ϊ����״̬���Ϻͷǽ���״̬����
	//�������ݽṹvector��ÿ��vector�ж���һ�����ϣ���С����ʱ����Ҫ��dfa�Ĳ�ͬ״̬���ֳ�����
	vector<set<DFAState>> partitions = { acceptStates, nonAcceptStates };
	//���ö�������״̬���Ͻ��и�ϸ�Ļ���
	//���Ľ���У�����һ�������е�״̬�ǿ��Ժϲ���
	//����м������Ͼʹ������յ�dfa��С��֮���״̬�м���
	queue<pair<set<DFAState>, char>> workQueue;
	//��״̬1��ת������Ϊһ�Դ����һ��������
	//�������ܹ��ܺõ�֪���ܹ��Ƴ��Ľ��״̬���ϣ�ͨ�������transitionMAP��

	// ��ʼ���������У�ֻ�������е�ת������
	set<char> symbols;
	//��dfa�е�ת�����Ŵ洢��һ��������
	for (const auto& transition : transitions) {
		//transitions�ǻ�û����С����dfa������ת����ϵ
		//����vector�е�ÿ��Ԫ���о����������״̬��״̬֮���ת����
		//�����е�ת�����洢����
		symbols.insert(transition.transitionSymbol);
	}

	for (const auto& symbol : symbols) {
		//������ֿ�������״̬������״̬�ͷǽ���״̬�����Ϸֱ��Ӧ�����е�ת��������ŵ�����������
		if (!acceptStates.empty()) workQueue.push(make_pair(acceptStates, symbol));
		if (!nonAcceptStates.empty()) workQueue.push(make_pair(nonAcceptStates, symbol));

	}

	// 2. ���Ϸָ����ֱ���ȶ�
	while (!workQueue.empty()) {
		//���ö����Ƚ��ȳ��Ĺ��ܽ����ȴ洢��״̬���ϴӶ����л�ȡ����
		pair<set<DFAState>, char> currentPair = workQueue.front();
		//pop��������ͷ�еļ���
		workQueue.pop();
		//��ȡ�������д洢��ÿ����Ԫ�еĵ�һ���ּ�״̬����
		set<DFAState> currentSet = currentPair.first;
		//��õڶ����ּ����״̬���϶�Ӧ��ת����
		char symbol = currentPair.second;

		// �ҳ����Ե���currentSet��״̬��״̬����
		unordered_map<DFAState, set<DFAState>> affectedStates;
		for (const auto& transition : transitions) {
			//��һ��״̬ͨ��һ��ת�����ҵ���״̬���ڵ�ǰ״̬������
			//��ô���״̬��Ӧ�ú͵�ǰ״̬���з���
			if (transition.transitionSymbol == symbol && (!currentSet.count(transition.toState))) {
				affectedStates[transition.fromState].insert(transition.toState);
			}
		}

		// ���·���������Ӱ���״̬��һ��ϸ��
		vector<set<DFAState>> newPartitions;
		set<DFAState> intersect, difference;
		for (const auto& state : currentSet) {
			//state��Ϊ���Ƿ������affectedStates��
			if (affectedStates.count(state)) intersect.insert(state);
			else difference.insert(state);
		}

		// ��ϸ�ֽ�������·���
		if (!intersect.empty()) newPartitions.push_back(intersect);
		if (!difference.empty()) newPartitions.push_back(difference);

		// ����µ�ϸ�ֲ��ֵ���������
		if (!intersect.empty() && !difference.empty()) {
			for (const auto& sym : symbols) {
				workQueue.push(make_pair(intersect, sym));
			}
		}

		partitions.erase(remove(partitions.begin(),partitions.end(),currentSet),partitions.end());
		for (const auto& newPartition : newPartitions) {
			partitions.push_back(newPartition);
		}
	}

	// 3. ������С�����DFA״̬
	unordered_map<DFAState, DFAState> stateMap;
	for (const auto& partition : partitions) {
		DFAState newState;
		newState.stateName = "q" + to_string(newdfaStates.size());
		for (const auto& state : partition) {
			newState.nfaStates.insert(state.nfaStates.begin(),state.nfaStates.end());
		}
		stateMap[(*partition.begin())] = newState;
		newdfaStates.push_back(newState);
	}

	// 4. �����µ�ת����ϵ
	for (const auto& transition : transitions) {
		auto from = stateMap[transition.fromState];
		auto to = stateMap[transition.toState];
		newtransitions.push_back({ from, to, transition.transitionSymbol });
	}
}

