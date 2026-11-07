// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream> //consider removing. FTXUI will handle inputs/outputs/printing. Use logs for debugging
#include <string>
#include <thread> 
#include <chrono>
#include <ftxui/ftxui.hpp>

int main()
{

	long currentCurds = 0;
	bool running = 1;
	bool hasStarted = 0;

	//FTXUI setup
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//display persistent elements on screen
	ftxui::Component renderer = ftxui::Renderer([&] { //lambda?
		return ftxui::vbox({
			ftxui::text("Curds: " + std::to_string(currentCurds)) | ftxui::bold,
			ftxui::separator(),
			ftxui::text("press space to make a cheese curd\npress q to quit") | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) { //lambda?
		if (event == ftxui::Event::Character(' ')) {
			currentCurds++;
			return true;
		}
		if (event == ftxui::Event::Character('q')) {
			running = 0;
			return true;
		}
		return false;
		});

	//loop contains above functions, lets me run all their logic elsewhere
	//this doesn't actually "loop" in the C++ sense
	ftxui::Loop loop(&screen, app);

	while (running == 1) { //MAIN GAME LOOP

		loop.RunOnce(); //do one pass of all my FTXUI functions. Doesn't hijack the program by waiting. If there are no pending inputs, moves on.
		screen.PostEvent(ftxui::Event::Custom); //push an event into the queue each frame. 
		//This is necessary when I have variables updating autonomously that I want displayed. "app" otherwise only checks if there are pending keyboard events.

		//determines update rate for THIS LOOP (everything above in while (). 
		//Consider encapsulating for frame-dependent work, and putting game logic elsewhere
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	
	}

}

