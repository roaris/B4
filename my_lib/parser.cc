struct Node {
    int id, left, right;
    string op;
    char c;

    Node(int id_, string op_, int left_, int right_, char c_) {
        id = id_; op = op_; left = left_; right = right_; c = c_;
    }
};

struct Parser {
    queue<char> regex;
    vector<Node> nodes;

    Parser(string regex_, bool debug) {
        for (auto& c:regex_) regex.push(c);
        int root = expr();
        if (debug) {
            cout << "-----Parse Result-----" << endl;
            dump(root);
            cout << endl;
        }
    }

    int expr() { //expr -> seq|expr, seq
        int seq_node = seq();

        if (regex.size() && regex.front()=='|') {
            regex.pop();
            nodes.push_back(Node(nodes.size(), "union", seq_node, expr(), '.'));
            return nodes.size()-1;
        }
        return seq_node;
    }

    int seq() { //seq -> subseq, 空文字
        if (regex.size() && (regex.front()=='(' || iswalpha(regex.front()))) return subseq();
        else {
            nodes.push_back(Node(nodes.size(), "empty", -1, -1, '.'));
            return nodes.size()-1;
        }
    }

    int subseq() { //subseq -> repeat subseq, repeat
        int repeat_node = repeat();

        if (regex.size() && (regex.front()=='(' || iswalpha(regex.front()))) {
            nodes.push_back(Node(nodes.size(), "concat", repeat_node, subseq(), '.'));
            return nodes.size()-1;
        }
        return repeat_node;
    }

    int repeat() { //repeat -> factor, factor*, factor+, factor?, factor[m,n], factor[m], factor[,m], factor[m,]
        int factor_node = factor();

        if (regex.size()) {
            if (regex.front()=='*') {
                regex.pop();
                nodes.push_back(Node(nodes.size(), "closure", factor_node, -1, '.'));
                return nodes.size()-1;
            }
            else if (regex.front()=='+') {
                regex.pop();
                nodes.push_back(Node(nodes.size(), "closure", factor_node, -1, '.'));
                nodes.push_back(Node(nodes.size(), "concat", factor_node, nodes.size()-1, '.'));
                return nodes.size()-1;
            }
            else if (regex.front()=='?') {
                regex.pop();
                nodes.push_back(Node(nodes.size(), "empty", -1, -1, '.'));
                nodes.push_back(Node(nodes.size(), "union", factor_node, nodes.size()-1, '.'));
                return nodes.size()-1;
            }
            else if (regex.front()=='[') {
                regex.pop();
                string s;
                bool split = false;
                while (true) {
                    if (regex.front()==']') {regex.pop(); break;}
                    if (regex.front()==',') split = true;
                    s += regex.front(); regex.pop();
                }

                int m = -1, n = -1;
                if (!split) {m = stoi(s), n = stoi(s);}
                else {
                    bool flag = true;
                    string s1, s2;
                    for (int i=0; i<s.size(); i++) {
                        if (s[i]==',') flag = false;
                        else {
                            if (flag) s1 += s[i];
                            else s2 += s[i];
                        }
                    }
                    if (s1.size()) m = stoi(s1);
                    if (s2.size()) n = stoi(s2);
                }
                
                if (m==-1 && n!=-1) m = 0;
                
                if (n==-1) {
                    nodes.push_back(Node(nodes.size(), "closure", factor_node, -1, '.'));
                    for (int i=0; i<m; i++) {
                        nodes.push_back(Node(nodes.size(), "concat", factor_node, nodes.size()-1, '.'));
                    }
                }
                else if (m==n) {
                    if (m==1) return factor_node;
                    nodes.push_back(Node(nodes.size(), "concat", factor_node, factor_node, '.'));
                    for (int i=0; i<m-2; i++) {
                        nodes.push_back(Node(nodes.size(), "concat", factor_node, nodes.size()-1, '.'));
                    }
                }
                else if (m<n) {
                    int empty_id = nodes.size();
                    nodes.push_back(Node(empty_id, "empty", -1, -1, '.'));
                    nodes.push_back(Node(nodes.size(), "union", empty_id, factor_node, '.'));
                    
                    for (int i=0; i<n-m-1; i++) {
                        nodes.push_back(Node(nodes.size(), "concat", factor_node, nodes.size()-1, '.'));
                        nodes.push_back(Node(nodes.size(), "union", empty_id, nodes.size()-1, '.'));
                    }

                    for (int i=0; i<m; i++) {
                        nodes.push_back(Node(nodes.size(), "concat", factor_node, nodes.size()-1, '.'));
                    }
                }

                return nodes.size()-1;
            }
        }

        return factor_node;
    }

    int factor() { //factor -> (expr), CHARACTER
        if (regex.front()=='(') {
            regex.pop();
            int expr_node = expr();
            regex.pop();
            return expr_node;
        }
        else {
            char c = regex.front();
            regex.pop();
            nodes.push_back(Node(nodes.size(), "char", -1, -1, c));
            return nodes.size()-1;
        }
    }

    void dump(int node_id) {
        string op = nodes[node_id].op;

        if (op=="char") printf("'%c'", nodes[node_id].c);
        else if (op=="concat") {printf("(concat "); dump(nodes[node_id].left); dump(nodes[node_id].right); printf(")");}
        else if (op=="union") {printf("(union "); dump(nodes[node_id].left); dump(nodes[node_id].right); printf(")");}
        else if (op=="closure") {printf("(closure "); dump(nodes[node_id].left); printf(")");}
        else if (op=="empty") printf("empty ");
    }
};