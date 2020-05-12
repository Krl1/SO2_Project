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

class Client{
    int id;
	int shopping_option;

public:
	Client(int);
	void shopping(vector<condition_variable> &);
	void taking_a_trolley_or_basket();
	void taking_products();
	void buying();
};