#include <iostream>    
#include <vector>       
#include <string>      
#include <algorithm>    
using namespace std;

class TrieNode{
    public:
        char data;
        TrieNode* children[26];
        bool isTerminal;

    TrieNode(char ch){
        data=ch;
        for(int i=0;i<26;i++)
        {
            children[i]=NULL;
        }
        isTerminal=false;
    }
};

class Trie{
    public:
        TrieNode* root;

    Trie(){
        root=new TrieNode('\0');
    }
    //INSERT
    void insertUtil(TrieNode* root, string word)
    {
        if(word.length()==0){
            root->isTerminal=true;
            return;
        }
        int index=word[0]-'a';
        TrieNode* child;

        //present
        if(root->children[index]!=NULL){
            child=root->children[index];
        }
        else{
            //absent
            child=new TrieNode(word[0]);
            root->children[index]=child;
        }

        //recursion
        insertUtil(child,word.substr(1));
    }

    void insertWord(string word){
        insertUtil(root, word);
    }
    //SEARCH
    bool searchUtil(TrieNode * root, string word)
    {
        if(word.length()==0){
            return root->isTerminal;
        }
        int index=word[0]-'a';
        TrieNode* child;

        //present
        if(root->children[index]!=NULL){
            child=root->children[index];
        }
        else{
            return false;
        }

        return searchUtil(child, word.substr(1));
    }

    bool search(string word)
    {
        return searchUtil(root,word);
    }

    //AUTOCOMPLETE
    void dfs(TrieNode* node, string prefix, vector<string>& results) {
        if (node->isTerminal) results.push_back(prefix);
        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                dfs(node->children[i], prefix + char('a' + i), results);
            }
        }
    }

    vector<string> autoComplete(const string &prefix) {
        TrieNode* curr = root;
        for (char ch : prefix) {
            int index = ch - 'a';
            if (curr->children[index] == NULL) return {};
            curr = curr->children[index];
        }
        vector<string> results;
        dfs(curr, prefix, results);
        return results;
    }

    //AUTOCORRECT
    int editDistance(const string &str1, const string &str2) {
        int n = str1.size();
        int m = str2.size();
        vector<vector<int>> dp(n+1, vector<int>(m+1, 0));
        for (int i=0; i<=n; i++) dp[i][0] = i;
        for (int j=0; j<=m; j++) dp[0][j] = j;

        for (int i=1; i<=n; i++) {
            for (int j=1; j<=m; j++) {
                if (str1[i-1] == str2[j-1]) 
                    dp[i][j] = dp[i-1][j-1];
                else 
                    dp[i][j] = 1 + min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]}); //delete, insert, replace
            }
        }
        return dp[n][m];
    }

    void gatherAllWords(TrieNode* node, string curr, vector<string>& words) {
        if (node->isTerminal) words.push_back(curr);
        for (int i = 0; i < 26; i++) {
            if (node->children[i]) {
                gatherAllWords(node->children[i], curr + char('a' + i), words);
            }
        }
    }

    vector<string> autoCorrect(const string &word) {
        vector<string> allWords;
        gatherAllWords(root, "", allWords);
        vector<string> autoCorrectWords;
        for(int iter=0;iter<allWords.size();iter++){
            if (editDistance(word,allWords[iter]) <= 3)
                autoCorrectWords.push_back(allWords[iter]);
        }
        return autoCorrectWords;
    }

};

int main()
{
    int n , q;
    cin>>n>>q;
    Trie* tr=new Trie();
    for(int i=0;i<n;i++)
    {
        string s;
        cin>>s;
        tr->insertWord(s);
    }
    while(q--)
    {
        int flag; 
        string word;
        cin>>flag; cin>>word;
        if(flag==1){
            if(tr->search(word)){
                cout<<"1"<<endl;
            }
            else{
                cout<<"0"<<endl;
            }
        }
        else if(flag==2){
            vector<string> output=tr->autoComplete(word);
            int len=output.size();
            cout<<len<<endl;
            // if(len==0) { cout<<"No word in dictionary found for autocomplete, starting with this word:"<<word<<"."<<endl;}
            for(int it=0;it<len;it++)
            {
                cout<<output[it]<<endl;
            }
        }
        else if(flag==3){
            vector<string> output=tr->autoCorrect(word);
            int length=output.size();
            cout<<length<<endl;
            for(int it=0;it<length;it++)
            {
                cout<<output[it]<<endl;
            }
        }
        else{
            cout<<"Incorrect Flag / Input"<<endl;
        }
    }
    return 0;
}