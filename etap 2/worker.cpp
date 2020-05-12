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

using namespace std;


Worker::Worker(int i){
	id=i;
}

void Worker::working(vector<condition_variable> &cVariables){
	taking_products_from_the_warehouse();
	placement_of_products_on_the_shelf();
	customer_service_at_the_till();
	putting_trolley_or_basket_back();
}

void Worker::taking_products_from_the_warehouse(){}

void Worker::placement_of_products_on_the_shelf(){}

void Worker::customer_service_at_the_till(){}

void Worker::putting_trolley_or_basket_back(){}