#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <random>
#include <unistd.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <sstream>
#include <semaphore.h>
#include <cstddef>
#include <mutex>
#include <curses.h>

using namespace std;

class Fork{
    mutex * mtx;
public:
    Fork(int id): id(id){
        mtx = new mutex();
    }
    int id;
    bool isDirty = 1;
};

class Philosopher{
    int id;
    int id_left_fork;
    int id_right_fork;

public:
    Philosopher(int id, int id_left_fork, int id_right_fork) : id(id), id_left_fork(id_left_fork), id_right_fork(id_right_fork){}
    void thinking(){}
    void eating(){}
    void initialConditions(vector<Fork> forks){
        if(this->id == 0){

        }
    }
    void feast(vector<Fork> forks){
        this->initialConditions(forks);
        while(true){
            cout<<this->id<<endl;
            this_thread::sleep_for(chrono::seconds (1));
        }
    }
};

//vector<Fork> forks(10);

int main(int argc, char* argv[]){
    cout<<"SO2 Projekt - Problem ucztujących filozofów" <<endl;
    cout<<"Karol Kulawiec 241281"<< endl<<endl;

    int numberOfphilosophers = 0;
    if (argc > 1){
        numberOfphilosophers = stoi(argv[1]);
        cout << "Liczba filozofów: " << numberOfphilosophers << endl;
    } else{
        cout<<"Nie podano liczby filozofów!"<<endl;
        return 0;
    }

    vector<Philosopher> philosophers;
    thread threads[numberOfphilosophers];
    vector<Fork> forks;
    for(int i=0; i<numberOfphilosophers; i++){
        forks.push_back(Fork(i));
    }

    for(int i=0; i<numberOfphilosophers; i++){
        philosophers.push_back(Philosopher(i, i, (i+1)%numberOfphilosophers));
        threads[i] = thread(&Philosopher::feast, &philosophers[i], ref(forks));
    }

    for(thread &thd : threads){
        thd.join();
    }


    return 0;
}

