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

void Worker::function(){
	cout<<endl<<"worker work"<<endl;

}