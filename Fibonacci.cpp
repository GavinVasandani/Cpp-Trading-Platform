#include <chrono>
#include <iostream>

using namespace std;

int fib (int n) {

    if(n==1 || n==2) {
        return 1;
    }

    else {
        return fib(n-1) + fib(n-2);
    }

}


int main() {

    cout<< "Enter nth value of fibonacci: "<<endl;
    cin>>n;
    cout<<"The nth fibonacci is: "<<fib(n)<<endl;
    return 0;

}
