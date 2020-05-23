// SO2.2.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>
#include<string>
#include <thread>
#include <mutex>
#include <thread>
#include <iostream>
#include <vector>
#include "PDCurses-3.8/curses.h"
#include <time.h> 
using namespace std;

const int cash_desk_count = 2;
const int client_count = 5;

mutex cash_desk[cash_desk_count];
mutex queue_mutex;
//mutex client[client_count]; 
mutex cout_mutex;
vector<int> queue;



void print_func(string s) {
	cout_mutex.lock();
	cout << s << endl;
	cout_mutex.unlock();
}

void client_thread(int id) {
	bool done = false; //zakupy zrobione lub nie

	while (true) {

		this_thread::sleep_for(chrono::seconds(10)); //robi zakupy 
		done = true; //zrobił

		bool at_queue = true;
		while (at_queue) { //wejście do kolejki 
			if (queue_mutex.try_lock()) {
				queue.push_back(id);
				at_queue = false;
				queue_mutex.unlock();
			}
		}
	
		while(done){  //PŁATNOŚĆ
			if (queue_mutex.try_lock()) {

				if (id == queue.front()) {
					queue.erase(queue.begin()); //usuwa sie z kolejki
					queue_mutex.unlock(); // odblokowuje kolejke
					bool payed = true;
					while (payed) {
						for (int i = 0; i < cash_desk_count; i++) {
							if (cash_desk[i].try_lock()) { //zakupy
								print_func("KLIENT " + to_string(id) + " robi zakupy w kasie" + to_string(i));
								this_thread::sleep_for(chrono::seconds(5));
								//wstaw funcke rysuącą zakupy
								
								cash_desk[i].unlock();
								print_func("KLIENT " + to_string(id) + " skonczyl zakupy w kasie " + to_string(i));
								done = false; // 
								payed = false;
								break;
							}


						}
					}

				}
			}
			
		}

	}
}

void cash_blocker() {
	while (true) {
		int p = rand() % cash_desk_count;
		this_thread::sleep_for(chrono::seconds(15));
		bool locked = true;
		while (locked) {

			if (cash_desk[p].try_lock()) {
				locked = false;
				print_func("KASA ZABLOKOWANA: " + to_string(p));
				this_thread::sleep_for(chrono::seconds(10));
				print_func("KASA ODBLOKOWANA: " + to_string(p));
				cash_desk[p].unlock();
				break;
			}
		}

	}
}


int main()
{
	initscr();
	WINDOW* win = newwin(200, 600, 10, 10);
	refresh();
	box(win, 0, 0);
	mvwprintw(win, 1, 1, "supermarket");
	wrefresh(win);


    thread client_threads[client_count];
    srand(time(NULL));
    for (int i = 0; i < client_count; i++) {


        client_threads[i] = thread(client_thread, i);
        //queue.push_back(i);
    }

    thread blocker = thread(cash_blocker);
    blocker.join();
    for (int i = 0; i < client_count; i++) {
        client_threads[i].join();
    }



}


