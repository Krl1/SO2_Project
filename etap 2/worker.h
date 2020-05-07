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

class Worker{
    int id;
	string name;   

public:
	int amount_of_item;
	void function();
};