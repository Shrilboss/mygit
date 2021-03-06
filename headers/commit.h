/* implement commit here */

/*
form a tree
-backtrack it to compute the hash of all the obejcts
    - if it does exist, does not matter just recompute it.
- create new commit object
- get the previous commit object and save it to the new commit object as parent
- save the sha1 of the new commit object to .git/objects/refs/head

*/

string PARENT_SHA="";
unordered_map<char, int> alphaNumeric_order;

bool compare(pair<string, string> x, pair<string, string> y){ 
    // used in sorting strings
    for (int i = 0; i < min(x.first.size(), y.first.size()); i++){ 
        if (alphaNumeric_order[x.first[i]] == alphaNumeric_order[y.first[i]]) 
            continue; 
        return alphaNumeric_order[x.first[i]] < alphaNumeric_order[y.first[i]]; 
    } 
    return x.first.size() < y.first.size(); 
}


string sort_content(string content_){
    // sort the content passed
    //  split_index_line(char * line_, string delimiter_,  int * n=&DEFAULT){
    // vector<string> return_string_vector(char** line, int * n)
    char content[MAX_FILE_NAME_LENGTH];
    strcpy(content, content_.c_str());
    int n;
    char** lines_= split_index_line(content, "\n", &n);
    // vector<string> key_val = return_string_vector(lines_, &n);
    char** tokens_;
    // char line[MAX_FILE_NAME_LENGTH];
    map<string, string> hashMap;
    for (int i=0; i< n; i++){
        string entry(lines_[i]);
        tokens_= split_index_line(lines_[i], " ");
        string name(tokens_[3]); // sorting based on sha
        hashMap[name]= entry;
    }

    vector<pair<string, string>> key_val;
    for (auto& i: hashMap){
        key_val.push_back(i);
    }

    sort(key_val.begin(), key_val.end(), compare);

    string sorted_content;
    // cout<<"after sorting----"<<endl;
    for (auto i: key_val){
        // sorted_content+=i+"\n";
        sorted_content+=i.second+"\n";
        // cout<< i.second<<endl;
    }            
    // cout<<"................................."<<endl;

    return sorted_content;
}








class treeNode{

    public:
        string mode;

        // vector<string> ref_objects_type; // list of object names
        // vector<string> ref_objects_sha;
        // vector<string> ref_objects_name;
        string sha="";
        string type;
        string name;
        vector<treeNode> pointers;
        vector<string> childpaths;


        void print_attributes(){
            cout<<"mode: "<< mode <<endl;
            cout<<"type: "<< type <<endl;
            cout<<"sha: "<< sha <<endl;
            cout<<"name: "<< name <<endl;
            cout<<"-----------------"<<endl;

        }
};



class commit{
    public:
        string author;
        string commiter;
        string sha1;
        string root_sha;
        vector<string> parent_commit;
        //etc
};


class tree{
    public:
        treeNode root; //"."
        map<string, treeNode*> hashSet;

      tree(){

        root.name= "root";
        root.type = "tree";
        hashSet[ROOT_PATH]=&root;
        // alphaNumeric_order; 
        string str = "_AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789./-"; 
        for (int i=0; i< str.size(); i++)  
            alphaNumeric_order[str[i]] = i;
        }

        void build_tree(string path_, char** blob_data){

            // 0- MODE
            // 1- SHA
            // 2- STAGE
            // 3- path
            string present_path= ROOT_PATH;

            treeNode* iterator= &root;
            char* token;
            char path[MAX_FILE_NAME_LENGTH];
            strcpy(path, path_.c_str());
            int type_file;

            token=strtok(path,"/");

            while (token!=NULL){

                string token_(token);
                present_path= present_path+"/"+token_;
                char present_path_[MAX_FILE_NAME_LENGTH];
                strcpy(present_path_,present_path.c_str());


                type_file=isDir(present_path_);
                if (hashSet.find(present_path) != hashSet.end()){ //is present
                    iterator->pointers.push_back(*hashSet[present_path]);
                    iterator= hashSet[present_path];
                }
                else{
                    // treeNode newTree; //todo reinitialization issue
                    treeNode* newTree= new treeNode;
                    newTree->name= token_;

                    if(type_file==1){  // directory
                        /* create tree node */
                        newTree->mode= "040000";
                        newTree->type= "tree";
                        iterator->pointers.push_back(*newTree);
                        iterator->childpaths.push_back(present_path);
                        hashSet[present_path]= newTree;
                        iterator= newTree;


                    }
                    // else if(type_file==2){  // file
                    else{
                        /* create blob object */
                        newTree->mode= blob_data[0];
                        newTree->type= "blob";
                        string blob_data_(blob_data[1]);
                        newTree->sha= blob_data_;
                        iterator->pointers.push_back(*newTree);
                        iterator->childpaths.push_back(present_path);
                        hashSet[present_path]=newTree;
                        break;
                    }
                }

                token= strtok(NULL, "/");
                // cout<<iterator->name<<" after update and else "<<endl;


                // cout<<"   adsfsadf"<<endl;
                // string token_(token);
                // present_path= present_path+"/"+token_;
                // present_path_[MAX_FILE_NAME_LENGTH];
                // strcpy(present_path_,present_path.c_str());
            }
            // ;
            // cout<<"   "<<endl;
            // cout<<iterator->name<< " ignore"<<endl;


            // dfs(root);
            // print_hash();
        }

        void print_hash(){
            for(auto x: hashSet){
                cout<< x.first<<endl;
                if (x.second->type!="blob"){
                    x.second->pointers[0].print_attributes();
                    if (x.second->name=="demo"){
                        x.second->pointers[1].print_attributes();
                    }
                }
            cout<<" --- done --- "<<endl;

            }
        }



        string dfs(treeNode curNode){ // evaluates sha for each node (bottom up) and writes contents in corresponding object files
            // curNode.print_attributes();
            // when leaf content
            if (curNode.type=="blob"){
                return curNode.mode+" "+ "blob" + " "+ curNode.sha+ " "+ curNode.name;
            }


            // craeate object
            string content;
            for (auto ptr = curNode.childpaths.begin(); ptr <curNode.childpaths.end(); ptr++){
                content += dfs(*hashSet[*ptr]);
                if (ptr!= curNode.childpaths.end()-1)
                    content += "\n";
            }

            // cout<<"content of -- "<<curNode.name<<"\n"<< content<<"\n----------------" <<endl;
            // exit();
            // pass content to compute hash function
            string sorted_content= sort_content(content);
            curNode.sha= hash_object(sorted_content, curNode.type);
            // compress
            // make and write file
            if (write_object_flag)
                write_object(curNode.sha, sorted_content, "tree");
            // fstream f;
            // f.open("p.txt", fstream::out | fstream::app);
            // f << content<<endl;
            // f.close();
            // cout<<"upar"<<endl;
            // cout << curNode.mode+" "+ "tree" + " "+ curNode.sha+  " "+ curNode.name << endl;
            // cout<<"niche"<<endl;
            return curNode.mode+" "+ "tree" + " "+ curNode.sha+  " "+ curNode.name;
            // return curNode.mode+" "+ curNode.name +"\0" + curNode.sha;

        }



        void read_index(){ // calls build_tree for each line
            // read index file line by line
            string line;
            // read file content
            ifstream myfile(PATH_INDEX);
            if(myfile.is_open()){
                while(getline(myfile, line)){
                    // 100644 e69de29bb2d1d6434b8b29ae775ad8c2e48c5391 0       asjdk.txt

                    string curPath= mysplit(line);
                    char line_[MAX_FILE_NAME_LENGTH];
                    strcpy(line_, line.c_str());
                    // char delimiter_[MAX_FILE_NAME_LENGTH];
                    // strcpy(delimiter, delimiter_.c_str());
                    
                    // char space[]= " ";
                    char** blob_data= split_index_line(line_, " ");
                    build_tree(curPath, blob_data);

                }
                myfile.close();
            }
            else{
                cout<<"Error"<<endl;
                exit(1);
        }


    }
};

// blob object is the created file itself, instance of the object present physically

string get_content_merge_commit(string root_sha, string commit_msg, string parent_1, string parent_2, time_t cur_time, string count){
    string author= "imp_git";
    string commiter= "imp_git";
    string parent_content= "parent "+parent_1+"\nparent "+parent_2+"\n";
    string t = to_string(cur_time);

    string content= "tree "+root_sha + "\n"+ parent_content+"author "+ author + " " + t + " " + count + "\n" + "committer "+commiter+ " " + t + " " + count +"\n\n"+ commit_msg;
    return content;
}

string get_content_commit(string root_sha, string commit_msg, time_t cur_time, string count){
    /* TODO- adding parent pointers */
    // char PATH_[MAX_FILE_NAME_LENGTH];
    // strcpy(PATH_, PATH.c_str());
    string parent_content="";
    // loop over all the parents
    char* PATH_= get_cur_head();
    // cout<<"\n"<<PATH_<<" is the get_cur_head()"<<endl;
    string path(PATH_);
    char ref_path[MAX_FILE_NAME_LENGTH];
    strcpy(ref_path, GIT_PATH);
    strcat(ref_path, PATH_);
    if (isDir(ref_path)==2){  //2 -- PATH is a regular file
        // create a new file at ref
        parent_content+="parent ";
        string ref_path_(ref_path);
        ifstream myfile(ref_path_);
        if(myfile.is_open()){
            while(getline(myfile,PARENT_SHA)){
                // cout<<PARENT_SHA<<" is the parent sha"<<endl;
            }
            myfile.close();
        }
        else{
            cout<<"Error"<<endl;
            // exit();
        }
        parent_content+= PARENT_SHA+"\n";
    }
    string author= "imp_git";
    string commiter= "imp_git";
    // vector<string> parent_commit;  # get the parent sha from ref/heads
    string t = to_string(cur_time);
    string content= "tree "+root_sha + "\n"+ parent_content+"author "+ author + " " + t + " " + count + "\n" + "committer "+commiter+ " " + t + " " + count +"\n\n"+ commit_msg;
    return content;
}

string return_root_sha_from_index(){
    // read index file and build the tree
    // compute the hash
    tree myTree;
    myTree.read_index();
    string root_content = myTree.dfs(myTree.root);

    char root_content_[MAX_FILE_NAME_LENGTH];
    strcpy(root_content_, root_content.c_str());
    char** tokens_root_hash_= split_index_line(root_content_, " ");
    string root_sha(tokens_root_hash_[1]);
    return root_sha;
}




void run_commit(string message, int merge_flag=0, string parent_1="", string parent_2=""){

    // read index file and build the tree
    // compute the hash
    tree myTree;
    myTree.read_index();
    time_t cur_time = time_in_sec();
    string count = to_string(update_commit_count());
    string root_content = myTree.dfs(myTree.root);

    char root_content_[MAX_FILE_NAME_LENGTH];
    strcpy(root_content_, root_content.c_str());
    char** tokens_root_hash_= split_index_line(root_content_, " ");
    // char** tokens_root_hash= split_index_line(tokens_root_hash_[1], "\t");
    // cout<<tokens_root_hash[0]<<endl;
    string root_sha(tokens_root_hash_[1]);
    string commit_sha;
    string commit_content;

    if (merge_flag){
        commit_content= get_content_merge_commit(root_sha, message, parent_1, parent_2, cur_time, count);
        commit_sha=hash_object(commit_content,"commit");

    }
    else{
        commit_content= get_content_commit(root_sha, message, cur_time, count);
        commit_sha=hash_object(commit_content,"commit");

        if (PARENT_SHA.compare("")!=0){
            vector<string> entries= return_split_content_from_sha(PARENT_SHA);
            // split entries[0] and get sha
            // char** split_index_line(char * line_, string delimiter_,  int * n=&DEFAULT){
            char entries_[MAX_FILE_NAME_LENGTH];
            strcpy(entries_, entries[0].c_str());
            
            // char first_line[MAX_FILE_NAME_LENGTH];
            // strcpy(first_line, entries_);
            char** splitted_line= split_index_line(entries_," ");
            char* tree_sha=splitted_line[1];
            string tree_sha_(tree_sha);
            // cout <<"----------"<<endl;
            // cout<<tree_sha_<<"\n"<<root_sha<<endl;
            // cout <<"----------"<<endl;
            if (tree_sha_.compare(root_sha)==0){
                cout<<"Nothing to commit."<<endl;
                return;
            }
        }
    }

    /*  tree 9b5a3d2570f0b61a9aca5188cc4e33c3a0b3f84b
        parent ahs
        parent sha
        author imp_git 12312311233 <count>
        committer imp_git 161238123 <count>

        first commit
    */
    if (write_object_flag)
        write_object(commit_sha, commit_content, "commit");
    // overwrite commit at head ////////
    char* path_= get_cur_head();
    string PATH(path_);
    // ofstream MyFile("git/"+PATH);
    // MyFile << commit_sha;
    // MyFile.close();
    ofstream f;
    f.open("git/"+PATH);
    f << commit_sha;
    f.close();

   
    //definining commit log content
    string log_content;
    log_content+=commit_sha+" imp_git imp_git commit : "+message;

    //writing in git/logs/HEAD
    ofstream log_head;
    log_head.open(LOG_HEAD_PATH,ofstream::app);
    log_head<<log_content<<'\n';
    log_head.close();

    //writing in git/logs/refs/current_branch

    //get current branch name
    char current_branch[MAX_FILE_NAME_LENGTH];
    get_cur_branch_name(current_branch);
    string branch_(current_branch);
    string LOG_branch_path= LOG_PATH+branch_;

    ofstream branch_head;
    branch_head.open(LOG_branch_path,ofstream::app);
    branch_head<<log_content<<'\n';
    branch_head.close();
    

}
string stash_commit(string message){

    // read index file and build the tree
    // compute the hash
    tree myTree;
    myTree.read_index();
    time_t cur_time = time_in_sec();
    string count = to_string(update_commit_count());
    string root_content = myTree.dfs(myTree.root);

    char root_content_[MAX_FILE_NAME_LENGTH];
    strcpy(root_content_, root_content.c_str());
    char** tokens_root_hash_= split_index_line(root_content_, " ");
    // char** tokens_root_hash= split_index_line(tokens_root_hash_[1], "\t");
    // cout<<tokens_root_hash[0]<<endl;
    string root_sha(tokens_root_hash_[1]);
    string commit_sha;
    string commit_content;


        commit_content= get_content_commit(root_sha, message, cur_time, count);
        commit_sha=hash_object(commit_content,"commit");

        if (PARENT_SHA.compare("")!=0){
            vector<string> entries= return_split_content_from_sha(PARENT_SHA);
            // split entries[0] and get sha
            // char** split_index_line(char * line_, string delimiter_,  int * n=&DEFAULT){
            char entries_[MAX_FILE_NAME_LENGTH];
            strcpy(entries_, entries[0].c_str());
            
            // char first_line[MAX_FILE_NAME_LENGTH];
            // strcpy(first_line, entries_);
            char** splitted_line= split_index_line(entries_," ");
            char* tree_sha=splitted_line[1];
            string tree_sha_(tree_sha);
            // cout <<"----------"<<endl;
            // cout<<tree_sha_<<"\n"<<root_sha<<endl;
            // cout <<"----------"<<endl;
            if (tree_sha_.compare(root_sha)==0){
                cout<<"Nothing to commit."<<endl;
                return "";
            }
        }
    

    /*  tree 9b5a3d2570f0b61a9aca5188cc4e33c3a0b3f84b
        parent ahs
        parent sha
        author imp_git 12312311233 <count>
        committer imp_git 161238123 <count>

        first commit
    */
    if (write_object_flag)
        write_object(commit_sha, commit_content, "commit");
    

    return commit_sha;
    

}

void commit(int argc, char* argv[]){
   if(argc==2){
        printf("No arguments given\n");
        exit(1);
    }
    else{
        string flag(argv[2]);
        if (flag!= "-m"){
            cout<< "not supported!"<<endl;
            exit(1);
        }
        else{
            run_commit(argv[3]);
            // cout<< "some print"<<endl;
        }
    }
}


