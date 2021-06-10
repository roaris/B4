struct TF_IDF {
    vector<vector<vector<string>>> dbs;
    vector<map<string, float>> tf;
    map<string, int> df;
    map<string, float> idf;

    TF_IDF(vector<vector<vector<string>>> databases) {
        dbs = databases;
        int D = dbs.size();
        tf.resize(D);

        for (int i=0; i<D; i++) {
            int siz = 0;

            for (auto& record:dbs[i]) {
                for (auto& word:record) tf[i][word]++;
                siz += record.size();
            }

            for (auto it=tf[i].begin(); it!=tf[i].end(); it++) {
                string word = it->first;
                tf[i][word] /= siz; df[word]++;
            }
        }

        for (auto it=df.begin(); it!=df.end(); it++) {
            string word = it->first;
            idf[word] = log(D/(float)df[word]);
        }
    }

    vector<vector<vector<string>>> remove(float threshold) {
        vector<vector<vector<string>>> new_dbs;

        for (int i=0; i<dbs.size(); i++) {
            vector<pair<float, string>> words;

            for (auto& record:dbs[i]) for (auto& word:record) {
                words.push_back(make_pair(tf[i][word]*idf[word], word));
            }

            words.erase(unique(words.begin(), words.end()), words.end());
            sort(words.begin(), words.end());
            set<string> erase_words;

            for (int i=0; i<(int)(words.size()*threshold); i++) {
                erase_words.insert(words[i].second);
            }

            vector<vector<string>> db;

            for (auto& record:dbs[i]) {
                vector<string> new_record;

                for (auto& word:record) {
                    if (erase_words.find(word)==erase_words.end()) new_record.push_back(word);
                }

                db.push_back(new_record);
            }

            new_dbs.push_back(db);
        }

        return new_dbs;
    }
};