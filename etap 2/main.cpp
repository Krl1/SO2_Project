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
int height=80, width=150, start_y=1, start_x=5;
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
    height = 13 + 2*numberOfClients;
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
    mvwprintw(win, 6, 1, "Client's number:");
    mvwprintw(win, 6, 75, "Worker's number:");
    wrefresh(win);
    vector<Client> clients;
    vector<Worker> workers;
    vector<thread> threads;
    vector<condition_variable> cVariablesClients(numberOfClients);
    vector<condition_variable> cVariablesWorkers(numberOfWorkers);


    for(int i=0; i<numberOfClients; i++){
        clients.push_back(Client(i));
        mvwprintw(win, 9+2*i, 11, "%d", i);
    }
    for(int i=0; i<numberOfWorkers; i++){
        workers.push_back(Worker(i));
        mvwprintw(win, 9+2*i, 83, "%d", i);
    }
    wrefresh(win);
    for(int i=0; i<numberOfClients; i++){
        threads.push_back(thread(&Client::shopping, &clients[i], ref(cVariablesClients)));
    }
    for(int i=0; i<numberOfWorkers; i++){
        threads.push_back(thread(&Worker::working, &workers[i], ref(cVariablesWorkers)));
    }

    threads.push_back(thread(endProgram));

    for(thread &thd : threads){
        thd.join();
    }

	endwin();
    return 0;	
}
