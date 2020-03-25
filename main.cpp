#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>

using namespace std;

bool endFeast = false;

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
        while(!endFeast){
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
        }
    }
    int getId(){return this->id;}
    int getIdLeftFork(){return this->id_left_fork;}
    int getIdRightFork(){return this->id_right_fork;}
};

void endProgram(){
    printw("Press ESC to escape: ");
    do{
        if(getch()==27){
            endFeast=true;
            break;
        }
    }while(true);

}


int main(int argc, char* argv[]){
    initscr();
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

    threads.push_back(thread(endProgram));

    for(thread &thd : threads){
        thd.join();
    }

    endwin();
    return 0;
}

