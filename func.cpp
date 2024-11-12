//func.cpp
#include "head.h"

int nodeNum = 0;

/*下面是转换为DFA的主要函数*/

// 计算 NFA 状态的ε闭包
DFAState eClosure(const set<string>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<string> stateStack;

	// 初始化栈，将初始状态加入栈，最开始nfaState里只有NFA_Elem.startName
	for (const string& nfaState_name : nfaStates) {
		stateStack.push(nfaState_name);
	}

	while (!stateStack.empty()) {
		string currentState = stateStack.top();
		stateStack.pop();

		// 遍历 NFA 的边
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// 如果边的起始状态是当前状态，并且边的转换符号是#，那么将目标状态加入ε闭包
			if (currentEdge.startName.nodeName == currentState && currentEdge.tranSymbol == '#') {
				// 检查目标状态是否已经在ε闭包中，避免重复添加
				if (eClosureState.nfaStates.find(currentEdge.endName.nodeName) == eClosureState.nfaStates.end()) {
					eClosureState.nfaStates.insert(currentEdge.endName.nodeName);
					// 将目标状态加入栈以便进一步处理
					stateStack.push(currentEdge.endName.nodeName);
				}
			}
		}
	}

	// 为ε闭包分配一个唯一的名称
	for (const string& nfaState_name : eClosureState.nfaStates) {
		eClosureState.stateName += nfaState_name;
	}

	return eClosureState;
}

//move函数
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// 遍历 DFAState 中的每个 NFA 状态
	for (const string& nfaState_name : dfaState.nfaStates) {
		// 在这里遍历所有 NFA 状态的边
		for (int i = 0; i < nfa.edgeCount; i++) {
			edge currentEdge = nfa.edgeSet[i];

			// 如果边的起始状态是当前状态，且边的转换符号等于输入符号，将目标状态加入 nextState
			if (currentEdge.startName.nodeName == nfaState_name && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.endName.nodeName);
			}
		}
	}

	// 为 nextState 分配一个唯一的名称
	for (const string& nfaState_name : nextState.nfaStates) {
		nextState.stateName += nfaState_name;
	}

	return nextState;
}

// 检查 DFA 状态是否在状态集合中,即dfaStates里有没有找到targetState
bool isDFAStateInVector(const vector<DFAState>& dfaStates, const DFAState& targetState) {
	for (const DFAState& state : dfaStates) {
		if (state.stateName == targetState.stateName) {
			return true; // 找到匹配的状态
		}
	}
	return false; // 没有找到匹配的状态
}

//检查转换边是否在边集合中，比如a->b是否已经在集合中
bool isTransitionInVector(DFAState dfaState, DFAState dfaNextState, char symbol, vector<DFATransition> dfaTransitions)
{
	for (const DFATransition& transition : dfaTransitions) {
		if (transition.fromState.stateName == dfaState.stateName && dfaNextState.stateName == dfaNextState.stateName && symbol == transition.transitionSymbol) {
			return true;	//找到匹配的状态
		}
	}
	return false;
}

void buildDFAFromNFA(const elem& NFA_Elem, vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	// 初始化 DFA 状态集合和转换关系
	set<string> nfaInitialStateSet;
	nfaInitialStateSet.insert(NFA_Elem.startName.nodeName);
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // 计算 NFA 初始状态的 ε闭包
	dfaStates.push_back(dfaInitialState);

	// 开始构建 DFA
	for (int i = 0; i < dfaStates.size(); i++) {
		DFAState dfaState = dfaStates[i];
		for (int j = 0; j < NFA_Elem.edgeCount; j++) {
			char symbol = NFA_Elem.edgeSet[j].tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// 如果下一个状态不为空，且在 DFA 状态集合中还未添加，则加入 DFA 状态集合
				if (!isDFAStateInVector(dfaStates, dfaNextState)) {
					dfaStates.push_back(dfaNextState);
				}
				// 对于边也要去重，因为等于a的边可能会遍历到两次
				// 如果当前边在 DFA 转换关系中还未添加，则加入 DFA 转换关系
				if (!isTransitionInVector(dfaState, dfaNextState, symbol, dfaTransitions)) {
					dfaTransitions.push_back({ dfaState, dfaNextState, symbol });
				}
			}
		}
	}
}

// 显示 DFA 状态和转移关系，包括起始和结束状态
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

//生成DFA的dot文件
void generateDotFile_DFA(vector<DFAState>& dfaStates, vector<DFATransition>& dfaTransitions) {
	std::ofstream dotFile("dfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 初始状态\n\n";
		dotFile << dfaStates.back().stateName << "[shape = doublecircle];\n";
		// 添加DFA状态
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

		// 添加DFA转移
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

/*下面是构造NFA的主要函数*/

//组成单元拷贝函数
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount + i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
}

//打印NFA
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

//生成NFAdot文件
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 状态节点\n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// 添加 NFA 状态
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// 添加 NFA 转移
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

	// 跳过前四行
	for (int i = 0; i < 4; ++i) {
		getline(file, line);
	}

	// 读取边的定义
	while (getline(file, line) && !line.empty()) {
		edge e;

		std::istringstream iss(line);
		int nodeIndex;
		iss >> nodeIndex;
		e.startName.nodeName = to_string(nodeIndex);
		// 处理边的定义
		int dest;
		std::vector<int> destinations; // 存储目的地
		while (iss >> dest) {
			destinations.push_back(dest);
		}

		// 确保第一条边的 tranSymbol 从 destinations 获取
		if (!destinations.empty()) {
			// 取出 tranSymbol
			e.tranSymbol = (destinations[0] == -1) ? '#' : to_string(destinations[0])[0];

			// 遍历后续的目的地
			for (size_t i = 1; i < destinations.size(); ++i) {
				e.endName.nodeName = to_string(destinations[i]);
				nfa.edgeSet[index++] = e; // 保存边
			}
		}
	}
	nfa.edgeCount = index;


	// 读取起始状态和终止状态
	if (getline(file, line) && !line.empty()) {
		nfa.startName.nodeName = line;
	}

	if (getline(file, line) && !line.empty()) {
		nfa.endName.nodeName = line;
	}

	file.close();
}

// 重新命名 DFA 状态的函数
void renameDFAStates(std::vector<DFAState>& dfaStates, std::vector<DFATransition>& transitions) {
	std::unordered_map<std::string, std::string> nameMap; // 旧名称到新名称的映射
	int counter = 0; // 用于生成新名称的计数器

	// 第一步：重命名状态
	for (auto& state : dfaStates) {
		std::ostringstream newName;
		newName << "q" << counter++; // 生成新名称，如 q0, q1, q2...
		nameMap[state.stateName] = newName.str(); // 映射旧名称到新名称
		state.stateName = newName.str(); // 更新状态名称
	}

	// 第二步：更新转换关系中的状态名称
	for (auto& transition : transitions) {
		// 更新 fromState 和 toState 的名称
		transition.fromState.stateName = nameMap[transition.fromState.stateName];
		transition.toState.stateName = nameMap[transition.toState.stateName];
	}
}

// 判断两个状态是否等价
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

//// 最小化 DFA
//void minimizeDFA(const vector<DFAState>& dfaStates,
//	const vector<DFATransition>& transitions,
//	vector<DFAState>& newdfaStates,
//	vector<DFATransition>& newtransitions,
//	string& endname) {
//
//	// 构建DFA状态和转换映射
//	unordered_map<DFAState, unordered_map<char, DFAState>> transitionMap;
//	for (const auto& transition : transitions) {
//		//状态1经过转换符到状态2
//		transitionMap[transition.fromState][transition.transitionSymbol] = transition.toState;
//	}
//
//	// 1. 初始化接受和非接受状态分区
//	set<DFAState> acceptStates, nonAcceptStates;
//	//这里定义的状态集合中存储的也是定义的dfa状态的数据结构类型
//	//而不是单纯的一个string类型的来存储dfa状态的名称
//	//还需要存储这个状态对应nfa中的哪些状态组成的一个集合
//	
//	//遍历dfa的状态集，同时将dfa的状态集合分成两个集合
//	//接受状态和非接受状态分别表示的就是dfa状态图中的终态和非终态
//	for (const auto& state : dfaStates) {
//		if (state.nfaStates.find(endname) != state.nfaStates.end()) {
//			//如果dfa的一个状态中（通过子集法从nfa转化过来的）
//			//包含nfa的终态，那么这个状态在nfa中也是接受状态（即终态）
//			acceptStates.insert(state);
//		}
//		else {
//			nonAcceptStates.insert(state);
//		}
//	}
//
//	// 初始分区为接受状态集合和非接受状态集合
//	//利用数据结构vector，每个vector中都是一个集合（最小化的时候需要将dfa的不同状态区分出来）
//	vector<set<DFAState>> partitions = { acceptStates, nonAcceptStates };
//	//利用队列来将状态集合进行更细的划分
//	//最后的结果中，放在一个集合中的状态是可以合并的
//	//最后有几个集合就代表最终的dfa最小化之后的状态有几个
//	queue<pair<set<DFAState>, char>> workQueue;
//	//将状态1和转换符作为一对存放在一个队列中
//	//这两个能够很好的知道能够推出的结果状态集合（通过上面的transitionMAP）
//
//	// 初始化工作队列，只遍历现有的转换符号
//	set<char> symbols;
//	//将dfa中的转换符号存储到一个集合中
//	for (const auto& transition : transitions) {
//		//transitions是还没有最小化的dfa的所有转换关系
//		//里面vector中的每个元组中均存放了两个状态和状态之间的转换符
//		//将所有的转换符存储起来
//		symbols.insert(transition.transitionSymbol);
//	}
//
//	for (const auto& symbol : symbols) {
//		//把最初分开的两种状态（接受状态和非接受状态）集合分别对应到所有的转换符并存放到工作队列中
//		if (!acceptStates.empty()) workQueue.push(make_pair(acceptStates, symbol));
//		if (!nonAcceptStates.empty()) workQueue.push(make_pair(nonAcceptStates, symbol));
//
//	}
//
//	// 2. 不断分割分区直至稳定
//	while (!workQueue.empty()) {
//		//利用队列先进先出的功能将最先存储的状态集合从队列中获取出来
//		pair<set<DFAState>, char> currentPair = workQueue.front();
//		//pop出来队列头中的集合
//		workQueue.pop();
//		//获取到队列中存储的每个单元中的第一部分即状态集合
//		set<DFAState> currentSet = currentPair.first;
//		//获得第二部分即这个状态集合对应的转换符
//		char symbol = currentPair.second;
//
//		// 找出可以到达currentSet中状态的状态集合
//		unordered_map<DFAState, set<DFAState>> affectedStates;
//		for (const auto& transition : transitions) {
//			//将一个状态通过一个转换符找到的状态不在当前状态集合中
//			//那么这个状态就应该和当前状态进行分离
//			if (transition.transitionSymbol == symbol && (!currentSet.count(transition.toState))) {
//				affectedStates[transition.fromState].insert(transition.toState);
//			}
//		}
//
//		// 更新分区：将受影响的状态进一步细分
//		vector<set<DFAState>> newPartitions;
//		set<DFAState> intersect, difference;
//		for (const auto& state : currentSet) {
//			//state作为键是否存在于affectedStates中
//			if (affectedStates.count(state)) intersect.insert(state);
//			else difference.insert(state);
//		}
//
//		// 将细分结果加入新分区
//		if (!intersect.empty()) newPartitions.push_back(intersect);
//		if (!difference.empty()) newPartitions.push_back(difference);
//
//		// 添加新的细分部分到工作队列
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
//	// 3. 生成最小化后的DFA状态
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
//	// 4. 生成新的转换关系
//	for (const auto& transition : transitions) {
//		auto from = stateMap[transition.fromState];
//		auto to = stateMap[transition.toState];
//		newtransitions.push_back({ from, to, transition.transitionSymbol });
//	}
//}

