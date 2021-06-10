#include <bits/stdc++.h>
#include <dirent.h>
using namespace std;
#include "./../my_lib/scan.cc"
#include "./../my_lib/tf_idf.cc"

int main(int argc, char* argv[]) {
    string directory = "./../data/yahoo";
    int op1 = stoi(argv[1]); //0:整数値に変換 1:mining.ccの結果から復元
    vector<vector<vector<string>>> dbs;
    
    if (op1==0) {
        vector<string> file_names = scan_directory(directory+"/raw_data");

        for (auto& file_name:file_names) {
            vector<vector<string>> db = scan_file(directory+"/raw_data/"+file_name);
            dbs.push_back(db);
        }
        
        float op2 = stof(argv[2]); //tf-idfで削る割合
        TF_IDF tf_idf = TF_IDF(dbs);
        dbs = tf_idf.remove(op2);
        
        set<string> word_set;
        for (auto& db:dbs) for (auto& record:db) for (auto& word:record) word_set.insert(word);

        ofstream outputfile(directory+"/index.txt");
        ofstream outputfile2(directory+"/order.txt");
        map<string, int> idx;
        int i = 1;

        for (auto& word:word_set) {
            outputfile << i << " " << word+'\n';
            outputfile2 << i << " ";
            idx[word] = i++;
        }
        outputfile.close();
        outputfile2.close();
        i = 0;

        for (auto& file_name:file_names) {
            vector<vector<string>> db = dbs[i++];
            ofstream outputfile(directory+"/database/"+file_name);
            ofstream outputfile2(directory+"/removed/"+file_name);
    
            for (auto& record:db) {
                set<string> already;

                for (auto& word:record) {
                    if (already.find(word)==already.end()) {
                        outputfile << idx[word] << " ";
                        outputfile2 << word << " ";
                        already.insert(word);
                    }
                }
                outputfile << endl;
                outputfile2 << endl;
            }
            
            outputfile.close();
            outputfile2.close();
        }
    }
    else {
        map<int, string> rev_idx;
        ifstream ifs1(directory+"/index.txt");
        int i; string word;
        while (ifs1 >> i >> word) rev_idx[i] = word;

        ifstream ifs2(directory+"/result.txt");
        string line;
        while (getline(ifs2, line)) {
            if (line[0]=='.') continue;
            for (int i=0; i<line.size()-2; i++) if (line[i]=='1') cout << rev_idx[i+1] << " ";
            cout << endl;
        }
    }
    
}