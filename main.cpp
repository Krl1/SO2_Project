#include <iostream>
#include <thread>

using namespace std;

class BackgroundTask
{
public:
    void operator()() const
    {
        cout << "Hello from a thread..." << endl;
    }
};


void my_thred_func(){
        cout << "First thread!" << endl;
}

int main(){
    thread t0(&my_thred_func);
    t0.join();

    BackgroundTask bt;
    thread t1(bt);
    t1.join();


    thread t([] { cout << "My first thread..." << endl; });
    t.join();

    return 0;
}
