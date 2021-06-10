vector<string> scan_directory(string directory) {
    const char* path = directory.c_str();
    DIR * dp;
    dp = opendir(path);
    if (dp==NULL) exit(1);
    dirent* entry = readdir(dp);
    vector<string> file_names;
    while (entry!=NULL) {
        string name = entry->d_name;
        if (name[0]!='.') file_names.push_back(name);
        entry = readdir(dp);
    }
    sort(file_names.begin(), file_names.end());
    return file_names;
}

vector<vector<string>> scan_file(string file) { //末尾改行はカウントしない
    ifstream ifs(file, ios::in);
    string line, word;
    vector<vector<string>> sentences;
    while (getline(ifs, line)) {
        stringstream ss;
        ss << line;
        vector<string> sentence;
        while (ss >> word) sentence.push_back(word);
        sentences.push_back(sentence);
    }
    return sentences;
}