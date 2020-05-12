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

public:
	Worker(int);
	void working(vector<condition_variable> &);
	void taking_products_from_the_warehouse();
	void placement_of_products_on_the_shelf();
	void customer_service_at_the_till();
	void putting_trolley_or_basket_back();
};