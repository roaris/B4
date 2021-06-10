#include "epsilon_nfa.cc"

struct NFA {
    int state_num;
    map<int, map<char, set<int>>> epsilon_to;
    map<int, map<char, set<int>>> to;
    set<int> accept_states;
    set<int> alpha;

    NFA(string regex, bool parser_debug, bool epsilon_nfa_debug, bool nfa_debug) {
        Epsilon_nfa epsilon_nfa = Epsilon_nfa(regex, parser_debug, epsilon_nfa_debug);
        epsilon_to = epsilon_nfa.to;
        state_num = epsilon_nfa.state_num;

        for (int i=0; i<epsilon_nfa.state_num; i++) {
            for (auto it=epsilon_to[i].begin(); it!=epsilon_to[i].end(); it++) {
                char c = it->first;
                if (c!='#') alpha.insert(c);
            }
        }

        epsilon_remove();
        
        if (nfa_debug) {
            cout << "-----NFA Result-----" << endl;
            printf("state num is %d\n", state_num);
            printf("accept states: { ");
            for (auto& q:accept_states) printf("%d ", q);
            printf("}\n");
            
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

    set<int> closure(int state) {
        queue<int> q;
        q.push(state);
        set<int> res = {state};

        while (q.size()) {
            int state = q.front(); q.pop();
            for (auto& next_state:epsilon_to[state]['#']) {
                if (res.find(next_state)==res.end()) {
                    q.push(next_state);
                    res.insert(next_state);
                }
            }
        }

        return res;
    }

    set<int> my_union(set<int> a, set<int> b) {
        set<int> res;
        for (auto& ai:a) res.insert(ai);
        for (auto& bi:b) res.insert(bi);
        return res;
    }

    void epsilon_remove() {
        vector<set<int>> closures;
        for (int i=0; i<state_num; i++) closures.push_back(closure(i));

        for (int i=0; i<state_num; i++) {
            for (auto& c:alpha) {
                set<int> tmp;
                for (auto& state:closures[i]) tmp = my_union(tmp, epsilon_to[state][c]);
                for (auto& state:tmp) to[i][c] = my_union(to[i][c], closures[state]);
            }
            if (closures[i].find(1)!=closures[i].end()) accept_states.insert(i);
        }
    }

    bool match(string pattern) {
        queue<pair<int, int>> q;
        q.push(make_pair(0, 0));

        while (q.size()) {
            pair<int, int> p = q.front(); q.pop();
            int v = p.first, i = p.second;
            if (i==pattern.size()) {
                if (accept_states.find(v)!=accept_states.end()) return true;
            }
            else for (auto& nv:to[v][pattern[i]]) q.push(make_pair(nv, i+1));
        }

        return false;
    }
};