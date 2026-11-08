// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream> //consider removing. FTXUI will handle inputs/outputs/printing. Use logs for debugging
#include <string>
#include <thread>
#include <chrono>
#include <ftxui/ftxui.hpp> //this includes everything. consider reducing later if there are unneeded pkgs

int main()
{
	double currentCurds = 0;
	long displayCurds = 0;
	long curdsPerSecond = 0;
	int factoryCount = 0;
	int factoryCost = 10;
	int curdsPerClick = 1;
	bool running = 1;
	bool hasStarted = 0;

	//FTXUI setup
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//display persistent elements on screen
	ftxui::Component renderer = ftxui::Renderer([&] { //lambda?
		return ftxui::vbox({
			ftxui::text("Curds: " + std::to_string(displayCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
			ftxui::text("Factories: " + std::to_string(factoryCount) + " (cost: " + std::to_string(factoryCost) + ")") | ftxui::color(ftxui::Color::Red),
			ftxui::separator(),
			ftxui::text("press space to make a cheese curd\npress b to buy a factory\npress q to quit") | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) { //lambda?
		if (event == ftxui::Event::Character(' ')) {
			currentCurds+= curdsPerClick;
			return true;
		}
		if (event == ftxui::Event::Character('q')) {
			running = 0;
			return true;
		}
		if (event == ftxui::Event::Character('b')) {
			if (currentCurds >= factoryCost) {
				currentCurds -= factoryCost;
				factoryCount++;
			}
			
			return true;
		}
		return false;
		});

	//loop contains above functions, lets me run all their logic elsewhere
	//this doesn't actually "loop" in the C++ sense
	ftxui::Loop loop(&screen, app);

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		curdsPerSecond = (factoryCount * 10);
		
		auto now = std::chrono::steady_clock::now(); //take a timestamp
		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count(); //subtract time stored from time now, output seconds
		lastTime = now; //store a timestamp for the next compare, after the wait in this loop

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;
		
		screen.PostEvent(ftxui::Event::Custom); //push an event into the queue each frame. 
		//This is necessary when I have variables updating autonomously that I want displayed. "app" otherwise only checks if there are pending keyboard events.
		loop.RunOnce(); //do one pass of all my FTXUI functions. Doesn't hijack the program by waiting. If there are no pending inputs, moves on.
		
		

		//determines update rate for THIS LOOP (everything above in while (). 
		//Consider encapsulating for frame-dependent work, and putting game logic elsewhere
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	
	}

}

