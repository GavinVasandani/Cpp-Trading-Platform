#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <list>
#include <cstdio>

using namespace std; 

void visit (vector<int>vecInp, const F& visitor) {

    for(auto i: vecInp) {
        visitor(i);
    }

}


int main() {

    vector<int>vec = {0,1,2};
    visit(vec,[](int a) {return cout<<"Input value is: "<<a<<endl;});
    return 0;


}