#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>
#include <algorithm>

using namespace std;

#define RED     1
#define YELLOW  2
#define GREEN   3
#define WHITE   4

mutex mtx_writing_in_box;
mutex mtx_basket_trolley;
mutex mtx_shelf_breads, mtx_shelf_butters, mtx_shelf_jams, mtx_trolleys, mtx_baskets, mtx_future, mtx_baskets_used, mtx_trolleys_used, mtx_tills, mtx_finished_clients;


bool endShopping = false, shoppingIsOver = false;
int height=80, width=90, start_y=1, start_x=1;

WINDOW * win;

int numberOfClients = 0;
int numberOfWorkers = 0;

int numberOfBreads = 0;
int numberOfButters = 0;
int numberOfJams = 0;

int numberOfBreadsFuture = 0;
int numberOfButtersFuture = 0;
int numberOfJamsFuture = 0;

int numberOfTills = 0;

int numberOfBaskets = 0;
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
    vector <int> clients_id;
    int id;
    bool used = false;
    int worker_id;
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
        shopping_option=i%3 +1;

        win_c = newwin(5, 22, 3+5*this->getId(), 21);
        win_so = newwin(3, 3, 3+5*this->getId()+1, 14);
        refresh();
        box(win_c, 0,0);
        wrefresh(win_c);
    }

    void shopping(vector<condition_variable> &cVariablesShelves, vector<condition_variable> &cVariablesClients, vector<condition_variable> &cVariablesWorkers, vector<condition_variable> &cVariablesCarriage, vector<Basket> &baskets, vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams, vector<Till> &tills, vector<Trolley> &trolleys, vector<Basket> &basketsUsed, vector<Trolley> &trolleysUsed, vector<bool> &finishedClients){
        while(!endShopping){
            taking_a_trolley_or_basket(ref(baskets), ref(trolleys), ref(cVariablesCarriage));
            taking_products(ref(cVariablesShelves), ref(breads), ref(butters), ref(jams));
            buying(ref(baskets), ref(trolleys), ref(cVariablesCarriage), ref(basketsUsed), ref(trolleysUsed), ref(tills), ref(cVariablesClients), ref(cVariablesWorkers), ref(finishedClients));
            shoppingBreak();
        }
    }   

    void taking_a_trolley_or_basket(vector<Basket> &baskets, vector<Trolley> &trolleys, vector<condition_variable> &cVariablesCarriage){
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

        if(shopping_option==3){
            {
                unique_lock<mutex> lock_trolleys(mtx_trolleys);
                while(trolleys.size()==0)
                {
                    cVariablesCarriage[2].wait(lock_trolleys);
                }
                trolleys.erase(trolleys.begin());
            } 
        }
        else if(shopping_option==2){
            {
                unique_lock<mutex> lock_baskets(mtx_baskets);
                while(baskets.size()==0)
                {
                    cVariablesCarriage[0].wait(lock_baskets);
                }
                baskets.erase(baskets.begin());
            } 
        }
        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (time));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_c,1,i+1,"#");
                wrefresh(this->win_c);
            }
        }
    }

    void taking_products(vector<condition_variable> &cVariablesShelves, vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams){
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

    void buying(vector<Basket> &baskets, vector<Trolley> &trolleys, vector<condition_variable> &cVariablesCarriage, vector<Basket> &basketsUsed, vector<Trolley> &trolleysUsed, vector<Till> &tills, vector<condition_variable> &cVariablesClients, vector<condition_variable> &cVariablesWorkers, vector<bool> &finishedClients){

        {
            lock_guard<mutex> lock_finished(mtx_finished_clients);
            finishedClients[id] = false;
        }
        werase(win_c);
        box(win_c, 0,0);
        srand (time(nullptr));
        int time = 1000*shopping_option + rand()%500;
        time /= 20;

        int no_till = id%(numberOfWorkers/2); 
        {
            lock_guard<mutex> lock_till(mtx_tills);
            tills[no_till].clients_id.push_back(id);
        }
        {
            unique_lock<mutex> lock_till(mtx_tills);
            while(std::count(tills[no_till].clients_id.begin(), tills[no_till].clients_id.end(), id)){
                cVariablesClients[id].wait(lock_till);
            }
        }


        for(int i=0; i<20; i++){
            this_thread::sleep_for (chrono::milliseconds (time));
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_c,3,i+1,"#");
                wrefresh(this->win_c);
            }
        }
        {
            lock_guard<mutex> lock_finished(mtx_finished_clients);
            finishedClients[id]=true;
        }
        
        cVariablesWorkers[tills[no_till].worker_id].notify_one();

        if(shopping_option==3)
        {
            {
                lock_guard<mutex> lock(mtx_trolleys_used);
                trolleysUsed.push_back(Trolley());
            }
        }
        else if(shopping_option==2)
        {
            {
                lock_guard<mutex> lock(mtx_baskets_used);
                basketsUsed.push_back(Basket());
            }
        }
        werase(win_so);
        box(win_so, 0,0);
    }

    void shoppingBreak(){
        this_thread::sleep_for (chrono::milliseconds (1000));
        shopping_option = rand()%3 + 1;
    }

    int getId(){ return id;}
    
    const char* int_to_char(int i){
        if(i==1) return "1";
        if(i==2) return "2";
        else return "3";
    }
};

struct Worker{
    int id;
    int item;
    WINDOW * win_w;

    Worker(int i){
        id=i;
        win_w = newwin(6, 22, 3+6*this->getId(), 65);
        refresh();
        box(win_w, 0,0);
        wrefresh(win_w);
    }

    void working(vector<condition_variable> &cVariablesShelves, vector<condition_variable> &cVariablesClients, vector<condition_variable> &cVariablesWorkers, vector<condition_variable> &cVariablesCarriage,  vector<Basket> &baskets, vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams, vector<Till> &tills, vector<Trolley> &trolleys, vector<Basket> &basketsUsed, vector<Trolley> &trolleysUsed, vector<bool> &finishedClients){
        this_thread::sleep_for (chrono::milliseconds (5000));
        while(!shoppingIsOver){
            taking_products_from_the_warehouse();
            placement_of_products_on_the_shelf(ref(cVariablesShelves),ref(breads), ref(butters), ref(jams));
            customer_service_at_the_till(ref(cVariablesClients), ref(cVariablesWorkers), ref(tills), ref(finishedClients));
            putting_trolley_or_basket_back(ref(cVariablesCarriage), ref(baskets), ref(trolleys), ref(basketsUsed), ref(trolleysUsed));
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
    
    void placement_of_products_on_the_shelf(vector<condition_variable> &cVariablesShelves, vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams){
        {
            lock_guard<mutex> lock_br(mtx_shelf_breads);
            lock_guard<mutex> lock_bu(mtx_shelf_butters);
            lock_guard<mutex> lock_ja(mtx_shelf_jams);
            numberOfBreadsFuture=breads.size();
            numberOfButtersFuture=butters.size();
            numberOfJamsFuture=jams.size();

            if(numberOfBreadsFuture*2<numberOfButtersFuture*3){
                if(numberOfBreadsFuture*3<numberOfJamsFuture*6){
                    //pracownik bierze chleb numberOfClients*3/4
                    item=1;
                    numberOfBreadsFuture = breads.size() + numberOfBreads/2;
                    if(numberOfBreadsFuture>numberOfBreads)
                        numberOfBreadsFuture=numberOfBreads;
                }
                else{
                    //pracownik bierze dzem numberOfClients/4
                    item=3;
                    numberOfJamsFuture = jams.size() + numberOfJams/2;
                    if(numberOfJamsFuture>numberOfJams)
                        numberOfJamsFuture=numberOfJams;
                }
            }
            else if(numberOfJamsFuture*6<numberOfButtersFuture*3){
                //pracownik bierze dzem numberOfClients/4
                item=3;
                numberOfJamsFuture = jams.size() + numberOfJams/2;
                if(numberOfJamsFuture>numberOfJams) 
                    numberOfJamsFuture=numberOfJams;
            }
            else{
                //pracownik bierze maslo numberOfClients/2
                item=2;
                numberOfButtersFuture = butters.size() + numberOfButters/2;
                    if(numberOfButtersFuture>numberOfButters)
                        numberOfButtersFuture=numberOfButters;
            }
        }
        if(item==1){
            {
                lock_guard<mutex> lock_breads(mtx_shelf_breads);
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
                while(breads.size()<numberOfBreadsFuture){
                    breads.push_back(Bread());
                }

            }
            cVariablesShelves[0].notify_one();
        }
        else if(item==2){
            {
                lock_guard<mutex> lock_butters(mtx_shelf_butters);
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
                while(butters.size()<numberOfButtersFuture){
                    butters.push_back(Butter());
                }

            }
            cVariablesShelves[1].notify_one();
        }
        else if(item==3){
            {
                lock_guard<mutex> lock_jams(mtx_shelf_jams);
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
                while(jams.size()<numberOfJamsFuture){
                    jams.push_back(Jam());
                }

            }
            cVariablesShelves[2].notify_one();
        }
    }
    
    void customer_service_at_the_till(vector<condition_variable> &cVariablesClients, vector<condition_variable> &cVariablesWorkers, vector<Till> &tills, vector<bool> &finishedClients){
        werase(win_w);
        box(win_w, 0,0);
        {
            lock_guard<mutex> lock(mtx_writing_in_box);
            wrefresh(this->win_w);
        }
        int no_till = id%(numberOfWorkers/2); 
        {
            lock_guard<mutex> lock_till(mtx_tills);
            if(!tills[no_till].used){
                tills[no_till].used = true;
                tills[no_till].worker_id = id;
            }
            else{
                return;
            }
        }
        bool tillIsEmpty; 
        
        {
            lock_guard<mutex> lock(mtx_tills);
            tillIsEmpty = tills[no_till].clients_id.empty(); 
        }
        
        while(!tillIsEmpty){
            int client_id;
            {
                lock_guard<mutex> lock_till(mtx_tills);
                client_id = tills[no_till].clients_id.front();
                tills[no_till].clients_id.erase(tills[no_till].clients_id.begin());
            }
            {
                lock_guard<mutex> lock(mtx_writing_in_box);
                mvwprintw(this->win_w,3,1,"WORKING WITH %1d", client_id);
                wrefresh(this->win_w);
            }
            cVariablesClients[client_id].notify_one();

            {
                unique_lock<mutex> lock_finished(mtx_finished_clients);
                while(!finishedClients[client_id]){
                    cVariablesWorkers[id].wait(lock_finished);
                }
            }


            {
                lock_guard<mutex> lock_till(mtx_tills);
                tillIsEmpty = tills[no_till].clients_id.empty();  
            }
        }


        {
            lock_guard<mutex> lock_till(mtx_tills);
            tills[no_till].used = false;
        }
    }
    
    void putting_trolley_or_basket_back(vector<condition_variable> &cVariablesCarriage, vector<Basket> &baskets, vector<Trolley> &trolleys, vector<Basket> &basketsUsed, vector<Trolley> &trolleysUsed){
        int option = 1;
        int count = 0;
        {
            lock_guard<mutex> lock_b(mtx_baskets_used);
            lock_guard<mutex> lock_t(mtx_trolleys_used);

            if(basketsUsed.size() > trolleysUsed.size()){
                option = 0;
                count = basketsUsed.size();
                basketsUsed.clear();
            }
            else{
                count = trolleysUsed.size();
                trolleysUsed.clear();
            }
        }
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
        if(option==1){
            {
                lock_guard<mutex> lock(mtx_trolleys);
                int i=0;
                while(i<count){
                    trolleys.push_back(Trolley());
                    i++;
                }
            }
            cVariablesCarriage[2].notify_all();
        }
        else{
            {
                lock_guard<mutex> lock(mtx_baskets);
                int i=0;
                while(i<count){
                    baskets.push_back(Basket());
                    i++;
                }   
            }
            cVariablesCarriage[0].notify_all();
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

void shelvesBaskedTrolleyCondition(vector<Basket> &baskets, vector<Bread> &breads, vector<Butter> &butters, vector<Jam> &jams, vector<Till> &tills, vector<Trolley> &trolleys){
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

void usedTills(vector<Till> &tills, int numberOfTills){
    while(!shoppingIsOver){
        {
            lock_guard<mutex> lock(mtx_writing_in_box);
            for(int i=0; i<numberOfTills; i++){
                if(tills[i].used)
                    mvwprintw(win, height-13+i, width-46, "Used till %1d? Yes", i);
                else
                    mvwprintw(win, height-13+i, width-46, "Used till %1d?  No", i);

            }
            wrefresh(win);
        }
    }
}

void finishedClientsPrint(vector<bool> &finishedClients, vector<Till> &tills){
    while(!shoppingIsOver){
        {
            lock_guard<mutex> lock(mtx_writing_in_box);
            {
                lock_guard<mutex> lock_tills(mtx_tills);
                int no_tills=0;
                for(Till &till :tills){
                    mvwprintw(win, height-11+no_tills, width-46, "Till %d:           ", no_tills);
                    for(int i=0; i<till.clients_id.size(); i++){
                        mvwprintw(win, height-11+no_tills, width-38+2*i, "%d", till.clients_id[i]);
                    }
                    no_tills++;
                }

            }
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
    mvwprintw(win, 1, 1, "Client's | Shopping");
    mvwprintw(win, 2, 1, "number:  | option:");
    mvwprintw(win, 1, 45, "Worker's number:");
    wrefresh(win);
    vector<Client> clients;
    vector<Worker> workers;
    vector<thread> threadsC;
    vector<thread> threads;
    vector<condition_variable> cVariablesClients(numberOfClients);
    vector<bool> finishedClients(numberOfClients);
    vector<condition_variable> cVariablesWorkers(numberOfWorkers);
    vector<condition_variable> cVariablesShelves(3);
    vector<condition_variable> cVariablesCarriage(4); //0-Baskets, 1-BasketsUsed, 2-Trolley, 3-TrolleyUsed

    condition_variable cVariablesTrolley;
    condition_variable cVariablesBaskets;

    numberOfBaskets = numberOfClients/3;
    numberOfBreads = (numberOfClients*3)/2;
    numberOfButters = numberOfClients;
    numberOfJams = numberOfClients/2;
    numberOfTills = numberOfWorkers/2;
    numberOfTrolleys = numberOfClients/3;

    vector<Shelf> shelves(3);
    vector<Till> tills(numberOfTills);

    vector<Bread> breads(numberOfBreads);
    vector<Butter> butters(numberOfButters);
    vector<Jam> jams(numberOfJams);
    
    vector<Basket> baskets(numberOfBaskets);
    vector<Basket> basketsUsed;
    vector<Trolley> trolleys(numberOfTrolleys);
    vector<Trolley> trolleysUsed;

    for(int i=0; i<numberOfClients; i++){
        clients.push_back(Client(i));
        mvwprintw(win, 4+5*i, 6, "%d", i);
        finishedClients[i]=true;
    }
    for(int i=0; i<numberOfWorkers; i++){
        workers.push_back(Worker(i));
        mvwprintw(win, 4+6*i, 53, "%d", i);
    }
    wrefresh(win);
    for(int i=0; i<numberOfClients; i++){
        threadsC.push_back(thread(&Client::shopping, &clients[i], ref(cVariablesShelves), ref(cVariablesClients), ref(cVariablesWorkers), ref(cVariablesCarriage), ref(baskets), ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys), ref(basketsUsed), ref(trolleysUsed), ref(finishedClients)));
    }
    for(int i=0; i<numberOfWorkers; i++){
        threads.push_back(thread(&Worker::working, &workers[i], ref(cVariablesShelves), ref(cVariablesClients), ref(cVariablesWorkers), ref(cVariablesCarriage), ref(baskets), ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys), ref(basketsUsed), ref(trolleysUsed), ref(finishedClients)));
    }

    threads.push_back(thread(endProgram));
    threads.push_back(thread(shelvesBaskedTrolleyCondition, ref(baskets), ref(breads), ref(butters), ref(jams), ref(tills), ref(trolleys)));
    threads.push_back(thread(usedTills, ref(tills), ref(numberOfTills)));
    threads.push_back(thread(finishedClientsPrint, ref(finishedClients),ref(tills)));

    for(thread &thd : threadsC){
        thd.join();
    }
    shoppingIsOver = true;
    for(thread &thd : threads){
        thd.join();
    }

	endwin();
    return 0;	
}
