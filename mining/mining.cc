#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;
#include "./../my_lib/nfa.cc"
#include "./../my_lib/scan.cc"
#include "./../lib/CtoI.h"

int main() {
    //メモリの確保と入力変数の生成
    BDD_Init(1000000000, 1000000000);
    for (int i=0; i<10100; i++) BDD_NewVar();

    //パラメータ
    string directory = "./../data/yahoo/database";
    string order_txt = "./../data/yahoo/order.txt";
    float thres_ratio = 0.2;
    int item_num = 3286;
    string pattern = "(H|L)*H(H|L)*H(H|L)*H(H|L)*";
    int op1 = 1; //初期化オプション(0:頻出アイテム集合の和/1:全部分集合)
    int op2 = 1; //遷移オプション(0:補集合演算を使わない/1:使う)
    bool external_output = true; //解を外部ファイルに出力して復元する時はtrueにする

    //ファイル毎に頻出アイテム集合を求める
    vector<string> file_names = scan_directory(directory);
    int tran_num = file_names.size();
    ZBDD FI[tran_num];

    for (int i=0; i<tran_num; i++) {
        string data_path = directory+"/"+file_names[i];
        int threshold = ceil(scan_file(data_path).size()*thres_ratio);
        CtoI FI_ = CtoI_LcmA(const_cast<char*>(data_path.c_str()), const_cast<char*>(order_txt.c_str()), 2);
        if (FI_==CtoI_Null()) {cout << "BDD Buffer Overflow" << endl; exit(0);}
        FI[i] = FI_.GetZBDD();
        //FI[i].PrintPla();
        if (!external_output) cout << data_path << " Cardinality: " << FI[i].Card() << " Size: " << FI[i].Size() << endl;
    }

    //NFAの構築
    NFA nfa = NFA(pattern, false, false, false);

    //DPテーブルの初期化
    int n = nfa.state_num;
    ZBDD I[tran_num+1][n];
    for (int i=0; i<=tran_num; i++) for (int v=0; v<n; v++) I[i][v] = ZBDD(0);
    if (op1==0) for (int i=0; i<tran_num; i++) I[0][0] += FI[i];
    else {
        ZBDD all = ZBDD(1);
        for (int i=1; i<=item_num; i++) all *= ZBDD(1)+ZBDD(1).Change(i);
        I[0][0] = all;
    }
    if (I[0][0]==ZBDD(-1)) {cout << "BDD Buffer Overflow" << endl; exit(0);}

    //DP
    ZBDD result = ZBDD(0);

    if (op2==0) {
        for (int i=0; i<tran_num; i++) {
            for (int v=0; v<n; v++) {
                for (auto& nv:nfa.to[v]['H']) I[i+1][nv] += I[i][v]&FI[i];
                for (auto& nv:nfa.to[v]['L']) I[i+1][nv] += I[i][v]-FI[i];
            }
        }
        for (auto& v:nfa.accept_states) result += I[tran_num][v];
    }
    else {
        bool complement[tran_num+1][n];
        for (int i=0; i<=tran_num; i++) for (int j=0; j<n; j++) complement[i][j] = false;

        for (int i=0; i<tran_num; i++) {
            for (int v=0; v<n; v++) {
                for (auto& nv:nfa.to[v]['H']) {
                    if (complement[i][v]) {
                        ZBDD tmp = FI[i]-I[i][v];
                        if (complement[i+1][nv]) I[i+1][nv] = I[i+1][nv]-tmp;
                        else I[i+1][nv] = I[i+1][nv]+tmp;
                    }
                    else {
                        ZBDD tmp = FI[i]&I[i][v];
                        if (complement[i+1][nv]) I[i+1][nv] = I[i+1][nv]-tmp;
                        else I[i+1][nv] = I[i+1][nv]+tmp;
                    }
                }
                for (auto& nv:nfa.to[v]['L']) {
                    if (complement[i][v]) {
                        ZBDD tmp = FI[i]+I[i][v];
                        if (complement[i+1][nv]) I[i+1][nv] = I[i+1][nv]&tmp;
                        else {
                            complement[i+1][nv] = true;
                            I[i+1][nv] = tmp-I[i+1][nv];
                        }
                    }
                    else {
                        if (i==0 && v==0) {
                            if (complement[i+1][nv]) I[i+1][nv] = I[i+1][nv]&FI[0];
                            else {
                                complement[i+1][nv] = true;
                                I[i+1][nv] = FI[0]-I[i+1][nv];
                            }
                        }
                        else {
                            ZBDD tmp = I[i][v]-FI[i];
                            if (complement[i+1][nv]) I[i+1][nv] = I[i+1][nv]-tmp;
                            else I[i+1][nv] = I[i+1][nv]+tmp;
                        }
                    }
                }
            }
        }

        ZBDD all = ZBDD(1);
        for (int i=1; i<=item_num; i++)  all *= ZBDD(1)+ZBDD(1).Change(i);
        for (auto& v:nfa.accept_states) {
            if (complement[tran_num][v]) result += all-I[tran_num][v];
            else result += I[tran_num][v];
        }
    }

    //結果の表示
    if (!external_output) {
        cout << "Solution Count: " << result.Card() << endl;
        cout << "Solution ZDD Size: " << result.Size() << endl;
    }
    else result.PrintPla();
}