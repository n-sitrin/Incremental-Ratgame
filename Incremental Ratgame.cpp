// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <conio.h>

int main()
{
	int genCount = 0;
	int genMult = 10; 
	long currentCurds = 0;
	long displayCurds = 0;
	bool running = 1;
	bool hasStarted = 0;
	char ch; //used for keyboard input

	while (running == 1) {
		while (hasStarted == 0) { //do once on startup
			std::cout << "the game has started\n" << "press esc to exit\n";
			hasStarted = 1;
		}
		if (_kbhit()) { //monitors keyboard input
			ch = _getch();
			if (int(ch) == 27) { //esc to exit
				running = 0;
				break;
			}
			if (int(ch) == 32) { //space to increment
				currentCurds++;
				std::cout << currentCurds << "\n";
			}
				

		}
	}

}

