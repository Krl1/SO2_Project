#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>

#include "worker.h"
#include "client.h"

using namespace std;

#define RED     1
#define YELLOW  2
#define GREEN   3
#define WHITE   4

bool endShopping = false;
int height=40, width=78, start_y=5, start_x=5;
mutex mtx_writing_in_box;
WINDOW * win;
int numberOfClients = 0;
int numberOfWorkers = 0;


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
            endShopping=true;
            break;
        }

    }while(true);
}

int main(int argc, char* argv[]){
	if (argc > 1){
		numberOfClients = stoi(argv[1]);
		if (argc == 3){
			numberOfWorkers = stoi(argv[2]);
		}
    } else{
        cout<<("Nie podano liczby Pracowników lub Klientów!")<<endl;
        getch();
        return 0;
    }
    height = 15 + 5*numberOfWorkers;
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
    mvwprintw(win, 1, 1, "SO2 Project - Supermarket");
    mvwprintw(win, 2, 1, "Karol Kulawiec 241281");
    mvwprintw(win, 3, 1, "Number of Clients: %d", numberOfClients);
    mvwprintw(win, 4, 1, "Number of Workers: %d", numberOfWorkers);
    wrefresh(win);
    vector<Client> clients;
    vector<Worker> workers;
    vector<thread> threads;

    threads.push_back(thread(endProgram));

    Worker* worker = new Worker();
    worker->function();

    Client* client = new Client();
    client->function();




    for(thread &thd : threads){
        thd.join();
    }

	endwin();
    return 0;	
}
