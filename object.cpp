/* object defination of:
    - commit
    - blob
        - hash_object
        - compress the new content
        -
    - tree
and helper classes and definations
*/




#include "object.h"
// #include <string>
// #include <iostream>
// using namespace std;

//test hash_object
int main(int argc, char* argv[]){
    string file_name=argv[2];
    cout<<file_name<<endl;
    add(file_name);
    return 0;
}

