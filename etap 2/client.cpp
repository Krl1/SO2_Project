#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <mutex>
#include <condition_variable>
#include <ncurses.h>

#include "client.h"

using namespace std;

Client::Client(int i){
	id=i;
	shopping_option=0;
}

void Client::shopping(vector<condition_variable> &cVariables){
	taking_a_trolley_or_basket();
	void taking_products();
	void buying();
}

void Client::taking_a_trolley_or_basket(){}

void Client::taking_products(){}

void Client::buying(){}