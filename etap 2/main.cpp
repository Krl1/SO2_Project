#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>

// #include "global.h"
// #include "worker.h"
// #include "client.h"
// #include "basket.h"
// #include "bread.h"
// #include "butter.h"
// #include "jam.h"
// #include "shelf.h"
// #include "till.h"
// #include "trolley.h"

using namespace std;

#define RED     1
#define YELLOW  2
#define GREEN   3
#define WHITE   4

mutex mtx_writing_in_box;
mutex mtx_basket_trolley;
mutex mtx_shelf_breads, mtx_shelf_butters, mtx_shelf_jams;


bool endShopping = false, shoppingIsOver = false;
int height=80, width=90, start_y=1, start_x=1;

WINDOW * win;

int numberOfClients = 0;
int numberOfWorkers = 0;
int numberOfBaskets = 0;
int numberOfBreads = 0;
int numberOfButters = 0;
int numberOfJams = 0;
int numberOfTills = 0;
int numberOfTrolleys = 0;


struct Basket{
    Basket(){};
    int id;
};

struct Bread{
    Bread(){};
    int id;
};

struct Butter{
    Butter(){};
    int id;
};

struct Shelf{
    Shelf(){};
    int id;
    bool isReady = true;
};

struct Till{
    Till(){};
    int id;
};

struct Trolley{
    Trolley(){};
    int id;
};

struct Jam{
    Jam(){};
    int id;
};

struct Client{
    int id;
    int shopping_option;
    WINDOW * win_c;
    WINDOW * win_so;

    Client(int i){
        id=i;
        shopping_option=0;

        win_c = newwin(5, 22, 3+5*this->getId(), 21);
        win_so = newwin(3, 3, 3+5*this->getId()+1, 14);
        refresh();
        box(win_c, 0,0);
        wrefresh(win_c);
    }

    void shopping(vector<condition_variable> &cVariablesShelves, 
        vector<condition_variable> &cVariables, vector<Basket> &baskets, 
        vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams, 
        vector<Till> &tills, vector<Trolley> &trolleys){
        while(!endShopping){
            taking_a_trolley_or_basket(ref(baskets), ref(trolleys));
            taking_products(ref(cVariablesShelves), ref(breads), 
                ref(butters), ref(jams));
            buying(ref(baskets), ref(trolleys));
            shoppingBreak();
        }
        shoppingIsOver = true;
    }   

    void taking_a_trolley_or_basket(vector<Basket> &baskets, 
        vector<Trolley> &trolleys){
        {
            lock_guard<mutex> lock(mtx_basket_trolley);
            if(trolleys.size() > 0){
                this->shopping_option = 3;
                trolleys.erase(trolleys.begin());
            } 
            else if(baskets.size() > 0){
                this->shopping_option = 2;
                baskets.erase(baskets.begin());
            } else {
                this->shopping_option = 1;
            }
        }
        werase(win_so);
        box(win_so, 0,0);
        {
            lock_guard<mutex> lock(mtx_writing_in_box);
            mvwprintw(this->win_so,1,1,int_to_char(shopping_option));
            wrefresh(this->win_so);
        }
        

        werase(win_c);
        box(win_c, 0,0);
        srand (time(nullptr));
        int time = 1000 + rand()%500;
        time /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (time));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_c,1,i+1,"#");
                wrefresh(this->win_c);
            }
        }
    }

    void taking_products(vector<condition_variable> &cVariablesShelves, 
        vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams){
        werase(win_c);
        box(win_c, 0,0);
        srand (time(nullptr));
        int time = 1000*shopping_option + rand()%500;
        time /= 20;

        if(shopping_option==3){
            {
                unique_lock<mutex> lock_breads(mtx_shelf_breads);
                while(breads.size()==0)
                {
                    cVariablesShelves[0].wait(lock_breads);
                }
                breads.erase(breads.begin());
                for(int i=0; i<6; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_1(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }   
            //2////////////////////////////////////////////////////////
            {
                unique_lock<mutex> lock_butters(mtx_shelf_butters);
                while(butters.size()==0)
                {
                    cVariablesShelves[1].wait(lock_butters);
                }
                butters.erase(butters.begin());
                for(int i=6; i<13; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_2(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }   
            //3//////////////////////////////////////////////////////////
            {
                unique_lock<mutex> lock_jams(mtx_shelf_jams);
                while(jams.size()==0)
                {
                    cVariablesShelves[2].wait(lock_jams);
                }
                jams.erase(jams.begin());
                for(int i=13; i<20; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_3(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }
        }
        else if(shopping_option==2){
            {
                unique_lock<mutex> lock_breads(mtx_shelf_breads);
                while(breads.size()==0)
                {
                    cVariablesShelves[0].wait(lock_breads);
                }
                breads.erase(breads.begin());
                for(int i=0; i<10; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_1(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }
            //2////////////////////////////////////////////////////////
            {
                unique_lock<mutex> lock_butters(mtx_shelf_butters);
                while(butters.size()==0)
                {
                    cVariablesShelves[1].wait(lock_butters);
                }
                butters.erase(butters.begin());
                for(int i=10; i<20; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_2(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }
        }

        else if(shopping_option==1){
            {
                unique_lock<mutex> lock_breads(mtx_shelf_breads);
                while(breads.size()==0)
                {
                    cVariablesShelves[0].wait(lock_breads);
                }
                breads.erase(breads.begin());
                for(int i=0; i<20; i++){
                    this_thread::sleep_for (chrono::milliseconds (time));
                    {
                        lock_guard<mutex> lock_1(mtx_writing_in_box);
                        mvwprintw(this->win_c,2,i+1,"#");
                        wrefresh(this->win_c);
                    }
                }
            }
        }
    }

    void buying(vector<Basket> &baskets, vector<Trolley> &trolleys){
        werase(win_c);
        box(win_c, 0,0);
        srand (time(nullptr));
        int time = 1000*shopping_option + rand()%500;
        time /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (time));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_c,3,i+1,"#");
                wrefresh(this->win_c);
            }
        }
        if(shopping_option==3)
        {
            lock_guard<mutex> lock(mtx_basket_trolley);
            trolleys.push_back(Trolley());
        }
        else if(shopping_option==2)
        {
            lock_guard<mutex> lock(mtx_basket_trolley);
            baskets.push_back(Basket());
        }
        werase(win_so);
        box(win_so, 0,0);
    }

    void shoppingBreak(){this_thread::sleep_for (chrono::milliseconds (1000));}

    int getId(){ return id;}
    
    const char* int_to_char(int i){
        if(i==1) return "1";
        if(i==2) return "2";
        else return "3";
    }
};

struct Worker{
    int id;
    WINDOW * win_w;

    Worker(int i){
        id=i;
        win_w = newwin(6, 22, 3+6*this->getId(), 65);
        refresh();
        box(win_w, 0,0);
        wrefresh(win_w);
    }

    void working(vector<condition_variable> &cVariablesShelves, 
        vector<condition_variable> &cVariables, vector<Basket> &baskets, 
        vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams, 
        vector<Till> &tills, vector<Trolley> &trolleys){
        while(!endShopping){
            taking_products_from_the_warehouse();
            placement_of_products_on_the_shelf();
            customer_service_at_the_till();
            putting_trolley_or_basket_back();
        }
    }

    void taking_products_from_the_warehouse(){
        werase(win_w);
        box(win_w, 0,0);
        srand (time(nullptr));
        int warehouseTime = 1000 + rand()%500;
        warehouseTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (warehouseTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_w,1,i+1,"#");
                wrefresh(this->win_w);
            }
        }
    }
    
    void placement_of_products_on_the_shelf(){
        werase(win_w);
        box(win_w, 0,0);
        srand (time(nullptr));
        int shelfTime = 1000 + rand()%500;
        shelfTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (shelfTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_w,2,i+1,"#");
                wrefresh(this->win_w);
            }
        }
    }
    
    void customer_service_at_the_till(){
        werase(win_w);
        box(win_w, 0,0);
        srand (time(nullptr));
        int tillTime = 1000 + rand()%500;
        tillTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (tillTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_w,3,i+1,"#");
                wrefresh(this->win_w);
            }
        }
    }
    
    void putting_trolley_or_basket_back(){
        werase(win_w);
        box(win_w, 0,0);
        srand (time(nullptr));
        int puttingBackTime = 1000 + rand()%500;
        puttingBackTime /= 20;
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (puttingBackTime));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_w,4,i+1,"#");
                wrefresh(this->win_w);
            }
        }
    }

    int getId(){
        return id;
    }
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
            endShopping=true;
            break;
        }

    }while(true);
}

void shelvesBaskedTrolleyCondition(vector<Basket> &baskets, vector<Bread> &breads, 
    vector<Butter> &butters, vector<Jam> &jams, vector<Till> &tills, 
    vector<Trolley> &trolleys){
    while(!shoppingIsOver){
        {
            lock_guard<mutex> lock(mtx_writing_in_box);
            mvwprintw(win, height-13, width-26, "Number of Breads: %4d", breads.size());
            mvwprintw(win, height-12, width-26, "Number of Butters: %3d", butters.size());
            mvwprintw(win, height-11, width-26, "Number of Jams: %6d", jams.size());

            mvwprintw(win, height-9, width-26, "Number of Baskets: %3d", baskets.size());
            mvwprintw(win, height-8, width-26, "Number of Trolleys: %2d", trolleys.size());

            wrefresh(win);
        }
    }      
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
    height = 6 + 5*numberOfClients;
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
    mvwprintw(win, height-5, width-26, "SO2 Project - Supermarket");
    mvwprintw(win, height-4, width-26, "Karol Kulawiec 241281");
    mvwprintw(win, height-3, width-26, "Number of Clients: %d", numberOfClients);
    mvwprintw(win, height-2, width-26, "Number of Workers: %d", numberOfWorkers);
    mvwprintw(win, 1, 1, "Client's number:");
    mvwprintw(win, 1, 45, "Worker's number:");
    wrefresh(win);
    vector<Client> clients;
    vector<Worker> workers;
    vector<thread> threads;
    vector<condition_variable> cVariablesClients(numberOfClients);
    vector<condition_variable> cVariablesWorkers(numberOfWorkers);
    vector<condition_variable> cVariablesShelves(3);

    numberOfBaskets = numberOfClients/3;
    numberOfBreads = (numberOfClients*3)/2;
    numberOfButters = numberOfClients;
    numberOfJams = numberOfClients/2;
    numberOfTills = numberOfWorkers/2;
    numberOfTrolleys = numberOfClients/3;

    vector<Shelf> shelves(3);
    vector<Basket> baskets(numberOfBaskets);
    vector<Bread> breads(numberOfBreads);
    vector<Butter> butters(numberOfButters);
    vector<Jam> jams(numberOfJams);
    vector<Till> tills(numberOfTills);
    vector<Trolley> trolleys(numberOfTrolleys);


    for(int i=0; i<numberOfClients; i++){
        clients.push_back(Client(i));
        mvwprintw(win, 4+5*i, 9, "%d", i);
    }
    for(int i=0; i<numberOfWorkers; i++){
        workers.push_back(Worker(i));
        mvwprintw(win, 4+6*i, 53, "%d", i);
    }
    wrefresh(win);
    for(int i=0; i<numberOfClients; i++){
        threads.push_back(thread(&Client::shopping, &clients[i], 
            ref(cVariablesShelves), ref(cVariablesClients), ref(baskets), 
            ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys)));
    }
    for(int i=0; i<numberOfWorkers; i++){
        threads.push_back(thread(&Worker::working, &workers[i], 
            ref(cVariablesShelves), ref(cVariablesWorkers), ref(baskets), 
            ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys)));
    }

    threads.push_back(thread(endProgram));
    threads.push_back(thread(shelvesBaskedTrolleyCondition, ref(baskets), 
        ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys)));

    for(thread &thd : threads){
        thd.join();
    }

	endwin();
    return 0;	
}
