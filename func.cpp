//func.cpp
#include "head.h"

int nodeNum = 0;

/*下面是转换为DFA的主要函数*/

// 计算 NFA 状态的ε闭包
DFAState eClosure(const set<node>& nfaStates, elem nfa) {
	DFAState eClosureState;
	eClosureState.nfaStates = nfaStates;

	stack<node> stateStack;

	// 初始化栈，将初始状态加入栈，最开始nfaState里只有NFA_Elem.startName
	for (const auto& nfaState : nfaStates) {
		stateStack.push(nfaState);
	}

	while (!stateStack.empty()) {
		node currentState = stateStack.top();
		stateStack.pop();

		// 遍历 NFA 的边
		for (const auto& currentEdge : nfa.nfaEdgeSet ) {

			// 如果边的起始状态是当前状态，并且边的转换符号是#，那么将目标状态加入ε闭包
			if (currentEdge.nfaFrom.nodeName == currentState.nodeName && currentEdge.tranSymbol == '#') {
				auto result=eClosureState.nfaStates.insert(currentEdge.nfaTo);
				if (result.second) {
					// 将目标状态加入栈以便进一步处理
					stateStack.push(currentEdge.nfaTo);
				}
			}
		}
	}

	// 为ε闭包分配一个唯一的名称,并且判断新得到的这个dfa状态是否为终态
	for (const auto& nfaState : eClosureState.nfaStates) {
		eClosureState.dfaState.nodeName += nfaState.nodeName;
		//nfa状态集中是否包含终态
		if (nfaState.isAccept) {
			//则这个新的状态也定义为终态
			eClosureState.dfaState.isAccept = 1;
		}
	}

	return eClosureState;
}

//move函数
DFAState move(const DFAState& dfaState, char transitionSymbol, elem nfa) {
	DFAState nextState;

	// 遍历 DFAState 中的每个 NFA 状态
	for (const auto& nfaState : dfaState.nfaStates) {
		// 在这里遍历所有 NFA 状态的边
		for (const auto& currentEdge : nfa.nfaEdgeSet) {

			// 如果边的起始状态是当前状态，且边的转换符号等于输入符号，将目标状态加入 nextState
			if (currentEdge.nfaFrom.nodeName == nfaState.nodeName && currentEdge.tranSymbol == transitionSymbol && currentEdge.tranSymbol != '#') {
				nextState.nfaStates.insert(currentEdge.nfaTo);
			}
		}
	}

	// 为 nextState 分配一个唯一的名称
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
	DFAState dfaInitialState = eClosure(nfaInitialStateSet, NFA_Elem); // 计算 NFA 初始状态的 ε闭包
	//得到的就是DFA的开始状态
	newDFA.dfaStartState = dfaInitialState;
	set<DFAState> dfaStates;
	dfaStates.insert(dfaInitialState);
	set<DFATransition> dfaTransitions;
	// 开始构建 DFA
	for (const auto& dfaState : dfaStates) {
		//遍历nfa中的所有边，因为定义的结构题中没有统计出所有的转换字符
		for (const auto& currentNfaEdge : NFA_Elem.nfaEdgeSet) {
			char symbol = currentNfaEdge.tranSymbol;
			DFAState nextState = move(dfaState, symbol, NFA_Elem);
			DFAState dfaNextState = eClosure(nextState.nfaStates, NFA_Elem);

			if (!nextState.nfaStates.empty()) {
				// 如果下一个状态不为空，且在 DFA 状态集合中还未添加，则加入 DFA 状态集合
				//使用set集合可以自动检查插入的元素是否重复
				//但是如果set中的元素是一个复杂结构体，那么就需要重新定义set中的比较运算符
				dfaStates.insert(dfaNextState);
				dfaTransitions.insert({ dfaState, dfaNextState, symbol });
			}
		}
	}

	//组合成一个完整的DFA
	newDFA.dfaEdgeSet = dfaTransitions;
	newDFA.dfaNodeSet = dfaStates;
	return newDFA;
}

// 显示 DFA 状态和转移关系，包括起始和结束状态
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

//生成DFA的dot文件
void generateDotFile_DFA(const DFA dfa,const string filename) {
	std::ofstream dotFile(filename);

	if (dotFile.is_open()) {
		dotFile << "digraph DFA {\n";
		dotFile << "  rankdir=LR;  // 横向布局\n\n";
		dotFile << " node [shape = circle];   // 初始状态\n\n";
		for (const auto& endstate : dfa.dfaNodeSet) {
			if (endstate.dfaState.isAccept) {
				dotFile << endstate.dfaState.nodeName << "[shape = doublecircle];\n";
			}
		}
		// 添加DFA状态
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

		// 添加DFA转移
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

/*下面是构造NFA的主要函数*/

//创建新节点
string new_node()
{
	string newNode;
	newNode = nodeNum + 65;//将名字用大写字母表示
	nodeNum++;
	return newNode;
}

//接收输入正规表达式
void input(string& RE)
{
	cout << "请输入正则表达式：  （操作符：() * |;字符集：a~z A~Z）" << endl;
	cin >> RE;
}


//组成单元拷贝函数
//即NFA的合并
elem elem_merge(elem& dest, const elem& source)
{
	// 合并状态集合（避免重复状态）
	dest.nodeSet.insert(source.nodeSet.begin(), source.nodeSet.end());

	// 合并边集合（避免重复边）
	dest.edgeSet.insert(source.edgeSet.begin(), source.edgeSet.end());

	// 合并结束状态集合
	dest.endName.insert(source.endName.begin(), source.endName.end());

	// 如果目标元素的开始状态为空，使用源元素的开始状态
	if (dest.startName.empty()||dest.startName==source.startName) {
		dest.startName = source.startName;
	}
	else {
		//如果两个nfa均有开始元素，并且两个开始元素不相同
		string newStartName = new_node();
		dest.nodeSet.insert(dest.startName);
		dest.edgeSet.insert({ newStartName,dest.startName,'#' });
		dest.edgeSet.insert({ newStartName,source.startName,'#' });
		dest.startName = newStartName;
	}

	// 更新状态数量
	dest.stateCount = dest.nodeSet.size();

	// 更新边数量
	dest.edgeCount = dest.edgeSet.size();
	//合并多个NFA时
	//通常会创建一个新的起始状态，并将每个NFA的起始状态通过ε-转换连接到这个新起始状态。
	//合并多个终止状态，可以将所有终止状态合并为一个新的终止状态。
	//这种做法通常通过添加一个新的终止状态，并在原有终止状态到新状态之间添加ε-转换来实现。
	elem newNFA;
	// 判断起始状态是否一致
	node newStart;
	newNFA.edgeCount = 0;
	if (nfa1.startName.nodeName != nfa2.startName.nodeName) {
		newStart.nodeName = "newStart"; // 可以根据需要生成唯一的名称
		// 添加ε-转换到nfa1和nfa2的起始状态
		newNFA.edgeSet[newNFA.edgeCount++] = { newStart, nfa1.startName, '#' };
		newNFA.edgeSet[newNFA.edgeCount++] = { newStart, nfa2.startName, '#' };
	}
	else {
		newStart = nfa1.startName; // 使用共同的起始状态
	}

	// 判断终止状态是否一致
	node newEnd;
	if (nfa1.endName.nodeName != nfa2.endName.nodeName) {
		newEnd.nodeName = "newEnd"; // 可以根据需要生成唯一的名称
	}
	else {
		newEnd = nfa1.endName; // 使用共同的终止状态
	}

	// 将nfa1的边复制到新NFA中
	for (int i = 0; i < nfa1.edgeCount; ++i) {
		newNFA.edgeSet[newNFA.edgeCount++] = nfa1.edgeSet[i];
	}

	// 将nfa2的边复制到新NFA中
	for (int i = 0; i < nfa2.edgeCount; ++i) {
		newNFA.edgeSet[newNFA.edgeCount++] = nfa2.edgeSet[i];
	}

	// 将nfa1和nfa2的终止状态通过ε-转换连接到新的终止状态
	if (nfa1.endName.nodeName != newEnd.nodeName) {
		newNFA.edgeSet[newNFA.edgeCount++] = { nfa1.endName, newEnd, '#' };
	}
	if (nfa2.endName.nodeName != newEnd.nodeName) {
		newNFA.edgeSet[newNFA.edgeCount++] = { nfa2.endName, newEnd, '#' };
	}

	// 设置新的起始和终止状态
	newNFA.startName = newStart;
	newNFA.endName = newEnd;

	return newNFA;
}

//处理 a
elem act_Elem(char c)
{
	//新节点
	string startNode = new_node();
	string endNode = new_node();

	//新边
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//新NFA组成元素（小的NFA元素/单元)
	elem newElem;
	newElem.edgeCount = 0;	//初始状态
	newElem.edgeSet.insert(newEdge);
	newElem.edgeCount++;
	newElem.startName = newEdge.startName;
	newElem.endName.insert(newEdge.endName);

	return newElem;
}

//处理a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//获得新的状态节点
	string startNode = new_node();
	string endNode = new_node();

	//构建e1（连接起点和AB的起始点A）
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//构建e2（连接起点和CD的起始点C）
	edge2.startName = startNode;
	edge2.endName = sec.startName;
	edge2.tranSymbol = '#';

	//构建e3（连接AB的终点和终点）
	edge3.startName = *fir.endName.begin();
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//构建e4（连接CD的终点和终点）
	edge4.startName = *sec.endName.begin();
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//将fir和sec合并
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//新构建的4条边
	newElem.edgeSet.insert(edge1);
	newElem.edgeSet.insert(edge2);
	newElem.edgeSet.insert(edge3);
	newElem.edgeSet.insert(edge4);
	newElem.edgeCount += 4;

	newElem.startName = startNode;
	newElem.endName.insert(endNode);

	return newElem;
}

//处理 N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//将fir的结束状态和sec的开始状态合并，将sec的边复制给fir，将fir返回
	//将sec中所有以StartState开头的边全部修改
	for (auto tempEdge : sec.edgeSet) {
		if (tempEdge.startName.compare(sec.startName) == 0)
		{
			tempEdge.startName = *fir.endName.begin(); //该边e1的开始状态就是N(t)的起始状态
		}
		else if (*tempEdge.endName.begin().compare(sec.startName) == 0) {
			tempEdge.endName  fir.endName; //该边e2的结束状态就是N(t)的起始状态
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//将fir的结束状态更新为sec的结束状态
	fir.endName = sec.endName;
	return fir;
}

//处理a*
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//获得新状态节点
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//闭包取空串

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

	//构建单元
	elem_copy(newElem, Elem);

	//将新构建的四条边加入EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//构建NewElem的启示状态和结束状态
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
	char* return_string = new char[2 * length + 2];//最多是两倍
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//要加的可能性如ab 、 *b 、 a( 、 )b 等情况
		//若第二个是字母、第一个不是'('、'|'都要添加
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//若第二个是'(',第一个不是'|'、'(',也要加
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//将最后一个字符写入second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "加'+'后的表达式：" << STRING << endl;
	return STRING;
}

//类里的各类元素定义
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
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
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
	postfix = ""; // 清空结果
	infToPost();
	return postfix;
}

/**表达式转NFA处理函数,返回最终的NFA集合
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
	cout << "已将正则表达式转换为NFA!" << endl;
	Elem = STACK.top();
	STACK.pop();

	return Elem;
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
void generateDotFile_NFA(const elem& nfa,string filename) {
	std::ofstream dotFile(filename);

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

// 最小化 DFA
void minimizeDFA(const vector<DFAState>& dfaStates,
	const vector<DFATransition>& transitions,
	vector<DFAState>& newdfaStates,
	vector<DFATransition>& newtransitions,
	string& endname) {

	// 构建DFA状态和转换映射
	unordered_map<DFAState, unordered_map<char, DFAState>> transitionMap;
	for (const auto& transition : transitions) {
		//状态1经过转换符到状态2
		transitionMap[transition.fromState][transition.transitionSymbol] = transition.toState;
	}

	// 1. 初始化接受和非接受状态分区
	set<DFAState> acceptStates, nonAcceptStates;
	//这里定义的状态集合中存储的也是定义的dfa状态的数据结构类型
	//而不是单纯的一个string类型的来存储dfa状态的名称
	//还需要存储这个状态对应nfa中的哪些状态组成的一个集合
	
	//遍历dfa的状态集，同时将dfa的状态集合分成两个集合
	//接受状态和非接受状态分别表示的就是dfa状态图中的终态和非终态
	for (const auto& state : dfaStates) {
		if (state.nfaStates.find(endname) != state.nfaStates.end()) {
			//如果dfa的一个状态中（通过子集法从nfa转化过来的）
			//包含nfa的终态，那么这个状态在nfa中也是接受状态（即终态）
			acceptStates.insert(state);
		}
		else {
			nonAcceptStates.insert(state);
		}
	}

	// 初始分区为接受状态集合和非接受状态集合
	//利用数据结构vector，每个vector中都是一个集合（最小化的时候需要将dfa的不同状态区分出来）
	vector<set<DFAState>> partitions = { acceptStates, nonAcceptStates };
	//利用队列来将状态集合进行更细的划分
	//最后的结果中，放在一个集合中的状态是可以合并的
	//最后有几个集合就代表最终的dfa最小化之后的状态有几个
	queue<pair<set<DFAState>, char>> workQueue;
	//将状态1和转换符作为一对存放在一个队列中
	//这两个能够很好的知道能够推出的结果状态集合（通过上面的transitionMAP）

	// 初始化工作队列，只遍历现有的转换符号
	set<char> symbols;
	//将dfa中的转换符号存储到一个集合中
	for (const auto& transition : transitions) {
		//transitions是还没有最小化的dfa的所有转换关系
		//里面vector中的每个元组中均存放了两个状态和状态之间的转换符
		//将所有的转换符存储起来
		symbols.insert(transition.transitionSymbol);
	}

	for (const auto& symbol : symbols) {
		//把最初分开的两种状态（接受状态和非接受状态）集合分别对应到所有的转换符并存放到工作队列中
		if (!acceptStates.empty()) workQueue.push(make_pair(acceptStates, symbol));
		if (!nonAcceptStates.empty()) workQueue.push(make_pair(nonAcceptStates, symbol));

	}

	// 2. 不断分割分区直至稳定
	while (!workQueue.empty()) {
		//利用队列先进先出的功能将最先存储的状态集合从队列中获取出来
		pair<set<DFAState>, char> currentPair = workQueue.front();
		//pop出来队列头中的集合
		workQueue.pop();
		//获取到队列中存储的每个单元中的第一部分即状态集合
		set<DFAState> currentSet = currentPair.first;
		//获得第二部分即这个状态集合对应的转换符
		char symbol = currentPair.second;

		// 找出可以到达currentSet中状态的状态集合
		unordered_map<DFAState, set<DFAState>> affectedStates;
		for (const auto& transition : transitions) {
			//将一个状态通过一个转换符找到的状态不在当前状态集合中
			//那么这个状态就应该和当前状态进行分离
			if (transition.transitionSymbol == symbol && (!currentSet.count(transition.toState))) {
				affectedStates[transition.fromState].insert(transition.toState);
			}
		}

		// 更新分区：将受影响的状态进一步细分
		vector<set<DFAState>> newPartitions;
		set<DFAState> intersect, difference;
		for (const auto& state : currentSet) {
			//state作为键是否存在于affectedStates中
			if (affectedStates.count(state)) intersect.insert(state);
			else difference.insert(state);
		}

		// 将细分结果加入新分区
		if (!intersect.empty()) newPartitions.push_back(intersect);
		if (!difference.empty()) newPartitions.push_back(difference);

		// 添加新的细分部分到工作队列
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

	// 3. 生成最小化后的DFA状态
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

	// 4. 生成新的转换关系
	for (const auto& transition : transitions) {
		auto from = stateMap[transition.fromState];
		auto to = stateMap[transition.toState];
		newtransitions.push_back({ from, to, transition.transitionSymbol });
	}
}

