#include "parser.cc"

struct Epsilon_nfa {
    vector<Node> nodes;
    int state_num = 2;
    map<int, map<char, set<int>>> to;

    Epsilon_nfa(string regex, bool parser_debug, bool epsilon_nfa_debug) {
        nodes = Parser(regex, parser_debug).nodes;
        build_nfa(nodes.size()-1, 0, 1);

        if (epsilon_nfa_debug) {
            cout << "-----Epsilon NFA Result-----" << endl;
            printf("state num is %d\n", state_num);

            for (int i=0; i<state_num; i++) {
                printf("%d | ", i);
                for (auto it=to[i].begin(); it!=to[i].end(); it++) {
                    printf("%c: { ", it->first);
                    for (auto& j:it->second) printf("%d ", j);
                    printf("} ");
                }
                printf("\n");
            }
        }
    }

    void build_nfa(int node_id, int entry, int out) {
        string op = nodes[node_id].op;

        if (op=="char") to[entry][nodes[node_id].c].insert(out);
        else if (op=="concat") {
            int mid = state_num++;
            build_nfa(nodes[node_id].left, entry, mid);
            build_nfa(nodes[node_id].right, mid, out);
        }
        else if (op=="union") {
            build_nfa(nodes[node_id].left, entry, out);
            build_nfa(nodes[node_id].right, entry, out);
        }
        else if (op=="closure") {
            int mid = state_num++;
            build_nfa(nodes[node_id].left, entry, mid);
            to[mid]['#'].insert(entry);
            to[entry]['#'].insert(out);
        }
        else if (op=="empty") to[entry]['#'].insert(out);
    }
};