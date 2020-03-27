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

#define RED     1
#define YELLOW  2
#define GREEN   3
#define WHITE   4

bool endFeast = false;
int height=40, width=78, start_y=5, start_x=5;
mutex mtx_writing_in_box;
WINDOW * win;
int numberOfPhilosophers = 0;

struct Fork{
    Fork(){};
    bool isReady = true;
    mutex mtx;
    int owner = -1;
};

class Philosopher{
    int id;
    int id_left_fork;
    int id_right_fork;

    WINDOW * win_eating;
    WINDOW * win_thinking;

public:
    Philosopher(int id, int id_left_fork, int id_right_fork) : id(id), id_left_fork(id_left_fork), id_right_fork(id_right_fork){
        this->win_eating = newwin(3, 22, 13+3*this->getId(), 31);
        this->win_thinking = newwin(3, 22, 13+3*this->getId(),56);
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
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(RED));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(RED));

                    wattron(win, COLOR_PAIR(YELLOW));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(YELLOW));

                    wrefresh(win);
                }
                unique_lock<mutex> lock_l(forks[this->getIdLeftFork()].mtx);
                while(!forks[this->getIdLeftFork()].isReady)
                {
                    cVariables[this->getIdLeftFork()].wait(lock_l);
                }
                forks[this->getIdLeftFork()].owner = this->getId();
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(GREEN));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(GREEN));

                    wattron(win, COLOR_PAIR(RED));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(RED));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdLeftFork(),12,"%d", this->getId());

                    wrefresh(win);
                }
                forks[this->getIdLeftFork()].isReady = false;

                unique_lock<mutex> lock_r(forks[this->getIdRightFork()].mtx);
                while(!forks[this->getIdRightFork()].isReady)
                {
                    cVariables[this->getIdRightFork()].wait(lock_r);
                }
                forks[this->getIdRightFork()].owner = this->getId();
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(GREEN));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(GREEN));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdRightFork(),12,"%d", this->getId());

                    wrefresh(win);
                }
                forks[this->getIdRightFork()].isReady = false;
                this->eating();
            }else{
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(RED));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(RED));

                    wattron(win, COLOR_PAIR(YELLOW));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(YELLOW));

                    wrefresh(win);
                }
                unique_lock<mutex> lock_r(forks[this->getIdRightFork()].mtx);
                while(!forks[this->getIdRightFork()].isReady)
                {
                    cVariables[this->getIdRightFork()].wait(lock_r);
                }
                forks[this->getIdRightFork()].owner = this->getId();
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(GREEN));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(GREEN));

                    wattron(win, COLOR_PAIR(RED));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(RED));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdRightFork(),12,"%d", this->getId());

                    wrefresh(win);
                }
                forks[this->getIdRightFork()].isReady = false;

                unique_lock<mutex> lock_l(forks[this->getIdLeftFork()].mtx);
                while(!forks[this->getIdLeftFork()].isReady)
                {
                    cVariables[this->getIdLeftFork()].wait(lock_l);
                }
                forks[this->getIdLeftFork()].owner = this->getId();
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(GREEN));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(GREEN));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdLeftFork(),12,"%d", this->getId());

                    wrefresh(win);
                }
                forks[this->getIdLeftFork()].isReady = false;
                this->eating();
            }
            if(this->getId()%2==0){
                forks[this->getIdRightFork()].isReady = true;
                forks[this->getIdRightFork()].owner = -1;
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(WHITE));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(WHITE));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdRightFork(),12," ");
                    wrefresh(win);
                }
                cVariables[getIdRightFork()].notify_one();

                forks[this->getIdLeftFork()].isReady = true;
                forks[this->getIdLeftFork()].owner = -1;
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(WHITE));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(WHITE));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdLeftFork(),12," ");
                    wrefresh(win);
                }
                cVariables[getIdLeftFork()].notify_one();
            }else{
                forks[this->getIdLeftFork()].isReady = true;
                forks[this->getIdLeftFork()].owner = -1;
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(WHITE));
                    mvwprintw(win, 9+3*this->getId(), 7, "%d", this->getIdLeftFork());
                    wattroff(win, COLOR_PAIR(WHITE));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdLeftFork(),12," ");
                    wrefresh(win);
                }
                cVariables[getIdLeftFork()].notify_one();

                forks[this->getIdRightFork()].isReady = true;
                forks[this->getIdRightFork()].owner = -1;
                {
                    lock_guard<mutex> lock(mtx_writing_in_box);

                    wattron(win, COLOR_PAIR(WHITE));
                    mvwprintw(win, 9+3*this->getId(), 15, "%d", this->getIdRightFork());
                    wattroff(win, COLOR_PAIR(WHITE));

                    mvwprintw(win, 11+3*numberOfPhilosophers+2*this->getIdRightFork(),12," ");
                    wrefresh(win);
                }
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
    {
        lock_guard<mutex> lock(mtx_writing_in_box);
        mvwprintw(win,height-3,1,"Press 'q' to escape.");
        wrefresh(win);
    }

    do{
        if(getch()==113){
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(win, height - 2, 1, "Program is ending. Please wait a few secounds.");
                wrefresh(win);
            }
            endFeast=true;
            break;
        }

    }while(true);
}


int main(int argc, char* argv[]){
    if (argc > 1){
        numberOfPhilosophers = stoi(argv[1]);
    } else{
        cout<<("Nie podano liczby filozofów!")<<endl;
        getch();
        return 0;
    }
    height = 15 + 5*numberOfPhilosophers;
    initscr();
    if (has_colors() == FALSE) {
        endwin();
        printf("Your terminal does not support color.\n");
        exit(1);
    }
    start_color();
    init_pair(RED, COLOR_RED, COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(WHITE, COLOR_WHITE, COLOR_BLACK );
    init_pair(YELLOW, COLOR_YELLOW, COLOR_BLACK);
    win = newwin(height, width, start_y, start_x);
    refresh();
    box(win, 0,0);
    mvwprintw(win, 1, 1, "SO2 Project - Dining philosophers problem");
    mvwprintw(win, 2, 1, "Karol Kulawiec 241281");
    mvwprintw(win, 3, 1, "Number of philosophers: %d", numberOfPhilosophers);
    mvwprintw(win, 5, 1, "Philosopher's number:");
    mvwprintw(win, 6, 1, "          |                     Eating                  Thinking");
    mvwprintw(win, 7, 1, "  L-fork  V  R-fork");
    wrefresh(win);
    vector<Philosopher> philosophers;
    vector<thread> threads;
    vector<Fork> forks(numberOfPhilosophers);
    vector<condition_variable> cVariables(numberOfPhilosophers);

    for(int i=0; i<numberOfPhilosophers; i++){
        philosophers.push_back(Philosopher(i, i, (i+1)%numberOfPhilosophers));
        mvwprintw(win, 9+3*i, 11, "%d", i);
        mvwprintw(win, 9+3*i, 7, "%d", i);
        mvwprintw(win, 9+3*i, 15, "%d", (i+1)%numberOfPhilosophers);
    }
    mvwprintw(win, 9+3*numberOfPhilosophers, 1, "Fork and Owner");
    for(int i=0; i<numberOfPhilosophers; i++){
        mvwprintw(win, 11+3*numberOfPhilosophers+2*i, 3, "%d", i);
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