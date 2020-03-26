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
int height=40, width=82, start_y=5, start_x=5;
mutex mtx_writing_in_box;
WINDOW * win;

struct Fork{
    Fork(){};
    bool isReady = true;
    mutex mtx;
    int owner;
};

class Philosopher{
    int id;
    int id_left_fork;
    int id_right_fork;

    WINDOW * win_eating;
    WINDOW * win_thinking;

public:
    Philosopher(int id, int id_left_fork, int id_right_fork) : id(id), id_left_fork(id_left_fork), id_right_fork(id_right_fork){
        this->win_eating = newwin(3, 22, 13+3*this->getId(), 35);
        this->win_thinking = newwin(3, 22, 13+3*this->getId(),60);
        refresh();
        box(win_eating, 0,0);
        box(win_thinking, 0,0);
        wrefresh(win_eating);
        wrefresh(win_thinking);
    }
    void eating(){
        werase(win_eating);
        box(win_eating, 0,0);
        srand (time(nullptr));
        int eatingTime = 2500 + rand()%1000;
        eatingTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (eatingTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_eating,1,i+1,"#");
                wrefresh(this->win_eating);
            }
        }
    }
    void thinking(){
        wclear(win_thinking);
        box(win_thinking, 0,0);
        srand (time(nullptr));
        int thinkingTime = 2500 + rand()%1000;
        thinkingTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (thinkingTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_thinking,1,i+1,"#");
                wrefresh(this->win_thinking);
            }
        }
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
    noecho();

    mvwprintw(win,height-3,1,"Press 'q' to escape.");

    wrefresh(win);
    do{
        if(getch()==113){

        	mvwprintw(win, height-2,1,"Program is ending. Please wait a few secounds.");
            wrefresh(win);
            endFeast=true;
            break;
        }

    }while(true);
}


int main(int argc, char* argv[]){
    int numberOfPhilosophers = 0;
    if (argc > 1){
        numberOfPhilosophers = stoi(argv[1]);
    } else{
        cout<<("Nie podano liczby filozofów!")<<endl;
        getch();
        return 0;
    }
    height = 15 + 3*numberOfPhilosophers;
    initscr();
    win = newwin(height, width, start_y, start_x);
    refresh();
    box(win, 0,0);
    mvwprintw(win, 1, 1, "SO2 Project - Dining philosophers problem");
    mvwprintw(win, 2, 1,"Karol Kulawiec 241281");
    mvwprintw(win, 3,1,"Number of philosophers: %d", numberOfPhilosophers);
    mvwprintw(win, 6, 5, "Philosopher's number:           Eating                  Thinking");
    wrefresh(win);
    vector<Philosopher> philosophers;
    vector<thread> threads;
    vector<Fork> forks(numberOfPhilosophers);
    vector<condition_variable> cVariables(numberOfPhilosophers);

    for(int i=0; i<numberOfPhilosophers; i++){
        philosophers.push_back(Philosopher(i, i, (i+1)%numberOfPhilosophers));
        mvwprintw(win, 9+3*i, 15, "%d", i);
    }
    wrefresh(win);
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