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
#include <condition_variable>

using namespace std;

struct Fork{
    Fork(){};
    bool isReady = true;
    mutex mtx;
};

class Philosopher{
    int id;
    int id_left_fork;
    int id_right_fork;

public:
    Philosopher(int id, int id_left_fork, int id_right_fork) : id(id), id_left_fork(id_left_fork), id_right_fork(id_right_fork){}
    void eating(){
        cout<<"Philosopher nr: "<<this->getId()<<" start eat."<<endl;
        srand (time(nullptr));
        int eatingTime = 2500 + rand()%1000;
        this_thread::sleep_for (chrono::milliseconds (eatingTime));
        cout<<"Philosopher nr: "<<this->id<<" stop eat."<<endl;
    }
    void thinking(){
        cout<<"Philosopher nr: "<<this->getId()<<" start think."<<endl;
        srand (time(nullptr));
        int thinkingTime = 2500 + rand()%1000;
        this_thread::sleep_for (chrono::milliseconds (thinkingTime));
        cout<<"Philosopher nr: "<<this->id<<" stop think."<<endl;
    }

    void feast(vector<Fork> &forks, vector<condition_variable> &cVariables){
        int i=0;
        while(i<1000){
            if(this->getId()%2==0){
                unique_lock<mutex> lock_l(forks[this->getIdLeftFork()].mtx);
                while(!forks[this->getIdLeftFork()].isReady)
                {
                    cVariables[this->getIdLeftFork()].wait(lock_l);
                }
                forks[this->getIdLeftFork()].isReady = false;
                unique_lock<mutex> lock_r(forks[this->getIdRightFork()].mtx);
                while(!forks[this->getIdRightFork()].isReady)
                {
                    cVariables[this->getIdRightFork()].wait(lock_r);
                }
                forks[this->getIdRightFork()].isReady = false;
                this->eating();
            }else{
                unique_lock<mutex> lock_r(forks[this->getIdRightFork()].mtx);
                while(!forks[this->getIdRightFork()].isReady)
                {
                    cVariables[this->getIdRightFork()].wait(lock_r);
                }
                forks[this->getIdRightFork()].isReady = false;
                unique_lock<mutex> lock_l(forks[this->getIdLeftFork()].mtx);
                while(!forks[this->getIdLeftFork()].isReady)
                {
                    cVariables[this->getIdLeftFork()].wait(lock_l);
                }
                forks[this->getIdLeftFork()].isReady = false;
                this->eating();
            }
            if(this->getId()%2==0){
                forks[this->getIdRightFork()].isReady = true;
                cVariables[getIdRightFork()].notify_one();
                forks[this->getIdLeftFork()].isReady = true;
                cVariables[getIdLeftFork()].notify_one();
            }else{
                forks[this->getIdLeftFork()].isReady = true;
                cVariables[getIdLeftFork()].notify_one();
                forks[this->getIdRightFork()].isReady = true;
                cVariables[getIdRightFork()].notify_one();
            }
            thinking();
            i++;
        }
    }
    int getId(){return this->id;}
    int getIdLeftFork(){return this->id_left_fork;}
    int getIdRightFork(){return this->id_right_fork;}
};


int main(int argc, char* argv[]){
    cout<<"SO2 Projekt - Problem ucztujących filozofów" <<endl;
    cout<<"Karol Kulawiec 241281"<< endl<<endl;

    int numberOfPhilosophers = 0;
    if (argc > 1){
        numberOfPhilosophers = stoi(argv[1]);
        cout << "Liczba filozofów: " << numberOfPhilosophers << endl;
    } else{
        cout<<"Nie podano liczby filozofów!"<<endl;
        return 0;
    }

    vector<Philosopher> philosophers;
    vector<thread> threads;
    vector<Fork> forks(numberOfPhilosophers);
    vector<condition_variable> cVariables(numberOfPhilosophers);


    for(int i=0; i<numberOfPhilosophers; i++){
        philosophers.push_back(Philosopher(i, i, (i+1)%numberOfPhilosophers));
    }

    for(int i=0; i<numberOfPhilosophers; i++){
        threads.push_back(thread(&Philosopher::feast, &philosophers[i], ref(forks), ref(cVariables)));
    }


    for(thread &thd : threads){
        thd.join();
    }


    return 0;
}

