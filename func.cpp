//func.cpp
#include "head.h"

int nodeNum = 0;

/*������ת��ΪDFA����Ҫ����*/

// ���� NFA ״̬�Ħűհ�
DFAState eClosure(const set<string>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<string> stateStack;

	// ��ʼ��ջ������ʼ״̬����ջ���ʼnfaState��ֻ��NFA_Elem.startName
	for (const string& nfaState_name : nfaStates) {
		stateStack.push(nfaState_name);
	}

	while (!stateStack.empty()) {
		string currentState = stateStack.top();
		stateStack.pop();

		// ���� NFA �ı�
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬�����ұߵ�ת��������#����ô��Ŀ��״̬����űհ�
			if (currentEdge.startName.nodeName == currentState && currentEdge.tranSymbol == '#') {
				// ���Ŀ��״̬�Ƿ��Ѿ��ڦűհ��У������ظ����
				if (eClosureState.nfaStates.find(currentEdge.endName.nodeName) == eClosureState.nfaStates.end()) {
					eClosureState.nfaStates.insert(currentEdge.endName.nodeName);
					// ��Ŀ��״̬����ջ�Ա��һ������
					stateStack.push(currentEdge.endName.nodeName);
				}
			}
		}
	}

	// Ϊ�űհ�����һ��Ψһ������
	for (const string& nfaState_name : eClosureState.nfaStates) {
		eClosureState.stateName += nfaState_name;
	}

	return eClosureState;
}

//move����
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// ���� DFAState �е�ÿ�� NFA ״̬
	for (const string& nfaState_name : dfaState.nfaStates) {
		// ������������� NFA ״̬�ı�
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// ����ߵ���ʼ״̬�ǵ�ǰ״̬���ұߵ�ת�����ŵ���������ţ���Ŀ��״̬���� nextState
			if (currentEdge.startName.nodeName == nfaState_name && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.endName.nodeName);
			}
		}
	}

	// Ϊ nextState ����һ��Ψһ������
	for (const string& nfaState_name : nextState.nfaStates) {
		nextState.stateName += nfaState_name;
	}

	return nextState;
}

// ��� DFA ״̬�Ƿ���״̬������,��dfaStates����û���ҵ�targetState
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState) {
	for (const DFAState& state : dfaStates) {
		if (state.stateName == targetState.stateName) {
			return true; // �ҵ�ƥ���״̬
		}
	}
	return false; // û���ҵ�ƥ���״̬
}

//���ת�����Ƿ��ڱ߼����У�����a->b�Ƿ��Ѿ��ڼ�����
bool isTransitionInVector(DFAState dfaState, DFAState dfaNextState, char symbol, vector<DFATransition> dfaTransitions)
{
	for (const DFATransition& transition : dfaTransitions) {
		if (transition.fromState.stateName == dfaState.stateName && dfaNextState.stateName == dfaNextState.stateName && symbol == transition.transitionSymbol) {
			return true;	//�ҵ�ƥ���״̬
		}
	}
	return false;
}

void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	// ��ʼ�� DFA ״̬���Ϻ�ת����ϵ
	set<string> nfaInitialStateSet;
	nfaInitialStateSet.insert(NFA_Elem.startName.nodeName);
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // ���� NFA ��ʼ״̬�� �űհ�
	dfaStates.push_back(dfaInitialState);

	// ��ʼ���� DFA
	for (int i = 0; i < dfaStates.size(); i++) {
		DFAState dfaState = dfaStates[i];
		for (int j = 0; j < NFA_Elem.edgeCount; j++) {
			char symbol = NFA_Elem.edgeSet[j].tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// �����һ��״̬��Ϊ�գ����� DFA ״̬�����л�δ��ӣ������ DFA ״̬����
				if (!isDFAStateInVector(dfaStates, dfaNextState)) {
					dfaStates.push_back(dfaNextState);
				}
				// ���ڱ�ҲҪȥ�أ���Ϊ����a�ı߿��ܻ����������
				// �����ǰ���� DFA ת����ϵ�л�δ��ӣ������ DFA ת����ϵ
				if (!isTransitionInVector(dfaState, dfaNextState, symbol, dfaTransitions)) {
					dfaTransitions.push_back({ dfaState, dfaNextState, symbol });
				}
			}
		}
	}
}

// ��ʾ DFA ״̬��ת�ƹ�ϵ��������ʼ�ͽ���״̬
void displayDFA(const vector<DFAState>& dfaStates, const vector<DFATransition>& dfaTransitions) {
	cout << "DFA States:" << endl;

	for (const DFAState& state : dfaStates) {
		cout << "State " << state.stateName << " (NFA States: ";
		for (const string& nfaState_name : state.nfaStates) {
			cout << nfaState_name << " ";
		}
		cout << ")";
		if (state.stateName == dfaStates.front().stateName) {
			cout << " (Initial State)";
		}
		if (state.stateName == dfaStates.back().stateName) {
			cout << " (Final State)";
		}
		cout << endl;
	}

	cout << "DFA Transitions:" << endl;
	for (const DFATransition& transition : dfaTransitions) {
		cout << "State " << transition.fromState.stateName << " --(" << transition.transitionSymbol << ")--> State " << transition.toState.stateName << endl;
	}
}

//����DFA��dot�ļ�
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("dfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // ���򲼾�\n\n";
		dotFile << " node [shape = circle];   // ��ʼ״̬\n\n";
		dotFile << dfaStates.back().stateName << "[shape = doublecircle];\n";
		// ���DFA״̬
		for (const auto& state : dfaStates) {
			dotFile << "  " << state.stateName;
			dotFile << " [label=\"State " << state.stateName;
			if (state.stateName == dfaStates.front().stateName) dotFile << "\\n(startState)";
			if (state.stateName == dfaStates.back().stateName) {
				dotFile << "\\n(endState)";
			}
			dotFile << "\"];\n";
		}
		dotFile << "\n";

		// ���DFAת��
		for (const auto& transition : dfaTransitions) {
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

//��ɵ�Ԫ��������
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount + i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
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
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

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

//// ��С�� DFA
//void minimizeDFA(const vector<DFAState>& dfaStates,
//	const vector<DFATransition>& transitions,
//	vector<DFAState>& newdfaStates,
//	vector<DFATransition>& newtransitions,
//	string& endname) {
//
//	// ����DFA״̬��ת��ӳ��
//	unordered_map<DFAState, unordered_map<char, DFAState>> transitionMap;
//	for (const auto& transition : transitions) {
//		//״̬1����ת������״̬2
//		transitionMap[transition.fromState][transition.transitionSymbol] = transition.toState;
//	}
//
//	// 1. ��ʼ�����ܺͷǽ���״̬����
//	set<DFAState> acceptStates, nonAcceptStates;
//	//���ﶨ���״̬�����д洢��Ҳ�Ƕ����dfa״̬�����ݽṹ����
//	//�����ǵ�����һ��string���͵����洢dfa״̬������
//	//����Ҫ�洢���״̬��Ӧnfa�е���Щ״̬��ɵ�һ������
//	
//	//����dfa��״̬����ͬʱ��dfa��״̬���Ϸֳ���������
//	//����״̬�ͷǽ���״̬�ֱ��ʾ�ľ���dfa״̬ͼ�е���̬�ͷ���̬
//	for (const auto& state : dfaStates) {
//		if (state.nfaStates.find(endname) != state.nfaStates.end()) {
//			//���dfa��һ��״̬�У�ͨ���Ӽ�����nfaת�������ģ�
//			//����nfa����̬����ô���״̬��nfa��Ҳ�ǽ���״̬������̬��
//			acceptStates.insert(state);
//		}
//		else {
//			nonAcceptStates.insert(state);
//		}
//	}
//
//	// ��ʼ����Ϊ����״̬���Ϻͷǽ���״̬����
//	//�������ݽṹvector��ÿ��vector�ж���һ�����ϣ���С����ʱ����Ҫ��dfa�Ĳ�ͬ״̬���ֳ�����
//	vector<set<DFAState>> partitions = { acceptStates, nonAcceptStates };
//	//���ö�������״̬���Ͻ��и�ϸ�Ļ���
//	//���Ľ���У�����һ�������е�״̬�ǿ��Ժϲ���
//	//����м������Ͼʹ������յ�dfa��С��֮���״̬�м���
//	queue<pair<set<DFAState>, char>> workQueue;
//	//��״̬1��ת������Ϊһ�Դ����һ��������
//	//�������ܹ��ܺõ�֪���ܹ��Ƴ��Ľ��״̬���ϣ�ͨ�������transitionMAP��
//
//	// ��ʼ���������У�ֻ�������е�ת������
//	set<char> symbols;
//	//��dfa�е�ת�����Ŵ洢��һ��������
//	for (const auto& transition : transitions) {
//		//transitions�ǻ�û����С����dfa������ת����ϵ
//		//����vector�е�ÿ��Ԫ���о����������״̬��״̬֮���ת����
//		//�����е�ת�����洢����
//		symbols.insert(transition.transitionSymbol);
//	}
//
//	for (const auto& symbol : symbols) {
//		//������ֿ�������״̬������״̬�ͷǽ���״̬�����Ϸֱ��Ӧ�����е�ת��������ŵ�����������
//		if (!acceptStates.empty()) workQueue.push(make_pair(acceptStates, symbol));
//		if (!nonAcceptStates.empty()) workQueue.push(make_pair(nonAcceptStates, symbol));
//
//	}
//
//	// 2. ���Ϸָ����ֱ���ȶ�
//	while (!workQueue.empty()) {
//		//���ö����Ƚ��ȳ��Ĺ��ܽ����ȴ洢��״̬���ϴӶ����л�ȡ����
//		pair<set<DFAState>, char> currentPair = workQueue.front();
//		//pop��������ͷ�еļ���
//		workQueue.pop();
//		//��ȡ�������д洢��ÿ����Ԫ�еĵ�һ���ּ�״̬����
//		set<DFAState> currentSet = currentPair.first;
//		//��õڶ����ּ����״̬���϶�Ӧ��ת����
//		char symbol = currentPair.second;
//
//		// �ҳ����Ե���currentSet��״̬��״̬����
//		unordered_map<DFAState, set<DFAState>> affectedStates;
//		for (const auto& transition : transitions) {
//			//��һ��״̬ͨ��һ��ת�����ҵ���״̬���ڵ�ǰ״̬������
//			//��ô���״̬��Ӧ�ú͵�ǰ״̬���з���
//			if (transition.transitionSymbol == symbol && (!currentSet.count(transition.toState))) {
//				affectedStates[transition.fromState].insert(transition.toState);
//			}
//		}
//
//		// ���·���������Ӱ���״̬��һ��ϸ��
//		vector<set<DFAState>> newPartitions;
//		set<DFAState> intersect, difference;
//		for (const auto& state : currentSet) {
//			//state��Ϊ���Ƿ������affectedStates��
//			if (affectedStates.count(state)) intersect.insert(state);
//			else difference.insert(state);
//		}
//
//		// ��ϸ�ֽ�������·���
//		if (!intersect.empty()) newPartitions.push_back(intersect);
//		if (!difference.empty()) newPartitions.push_back(difference);
//
//		// ����µ�ϸ�ֲ��ֵ���������
//		if (!intersect.empty() && !difference.empty()) {
//			for (const auto& sym : symbols) {
//				workQueue.push(make_pair(intersect, sym));
//			}
//		}
//
//		partitions.erase(remove(partitions.begin(),partitions.end(),currentSet),partitions.end());
//		for (const auto& newPartition : newPartitions) {
//			partitions.push_back(newPartition);
//		}
//	}
//
//	// 3. ������С�����DFA״̬
//	unordered_map<DFAState, DFAState> stateMap;
//	for (const auto& partition : partitions) {
//		DFAState newState;
//		newState.stateName = "q" + to_string(newdfaStates.size());
//		for (const auto& state : partition) {
//			newState.nfaStates.insert(state.nfaStates.begin(),state.nfaStates.end());
//		}
//		stateMap[(*partition.begin())] = newState;
//		newdfaStates.push_back(newState);
//	}
//
//	// 4. �����µ�ת����ϵ
//	for (const auto& transition : transitions) {
//		auto from = stateMap[transition.fromState];
//		auto to = stateMap[transition.toState];
//		newtransitions.push_back({ from, to, transition.transitionSymbol });
//	}
//}

