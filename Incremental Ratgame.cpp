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
	int curdsPerClick = 1;
	int factory1Count = 0, factory1Cost = 20, factory1ProdRate = 2;
	int factory2Count = 0, factory2Cost = 100, factory2ProdRate = 10;
	int factory3Count = 0, factory3Cost = 1000, factory3ProdRate = 100;
	int factory4Count = 0, factory4Cost = 10000, factory4ProdRate = 1000;
	bool running = 1;
	bool hasStarted = 0;


	//FTXUI setup
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//button prototype
	auto curdButton = ftxui::Button("Click 4 Curd", [&] { currentCurds++; });

	//display persistent elements on screen
	ftxui::Component renderer = ftxui::Renderer(curdButton, [&] { //lambda?
		return ftxui::vbox({
			ftxui::text("Curds: " + std::to_string(displayCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
			ftxui::text("Factory1 Count: " + std::to_string(factory1Count) + " (cost: " + std::to_string(factory1Cost) + ")") | ftxui::color(ftxui::Color::LightYellow3),
			ftxui::text("Factory2 Count: " + std::to_string(factory2Count) + " (cost: " + std::to_string(factory2Cost) + ")") | ftxui::color(ftxui::Color::Gold1),
			ftxui::text("Factory3 Count: " + std::to_string(factory3Count) + " (cost: " + std::to_string(factory3Cost) + ")") | ftxui::color(ftxui::Color::Orange1),
			ftxui::text("Factory4 Count: " + std::to_string(factory4Count) + " (cost: " + std::to_string(factory4Cost) + ")") | ftxui::color(ftxui::Color::Red),
			ftxui::separator(),
			curdButton -> Render(),
			ftxui::separator(),
			ftxui::text("press z to buy a factory1\npress x to buy a factory2\npress c to buy a factory3\npress v to buy a factory4\npress q to quit") | ftxui::dim,
			}) | ftxui::border;
		});

	

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) { //lambda?
		/*if (event == ftxui::Event::Character(' ')) {
			currentCurds+= curdsPerClick;
			return true;
		}*/
		if (event == ftxui::Event::Character('q')) {
			running = 0;
			return true;
		}
		//see how these could be standardized as functions. turn 1.07 into scalar input
		//maybe these functions (along with the variables at the top & the text lines) can be defined in 1 place? as a template?
		//inputs: name, initial cost, cost scalar, initial prod, input key, color
		//each can be called to determine CPS in main game loop
		if (event == ftxui::Event::Character('z')) {
			if (currentCurds >= factory1Cost) {
				currentCurds -= factory1Cost;
				factory1Count++;
				factory1Cost = 20 * (1.07 * (factory1Count + 1));
			}
			return true;
		}
		if (event == ftxui::Event::Character('x')) {
			if (currentCurds >= factory2Cost) {
				currentCurds -= factory2Cost;
				factory2Count++;
				factory2Cost = 100 * (1.07 * (factory2Count + 1));
			}
			return true;
		}
		if (event == ftxui::Event::Character('c')) {
			if (currentCurds >= factory3Cost) {
				currentCurds -= factory3Cost;
				factory3Count++;
				factory3Cost = 1000 * (1.07 * (factory3Count + 1));
			}
			return true;
		}
		if (event == ftxui::Event::Character('v')) {
			if (currentCurds >= factory4Cost) {
				currentCurds -= factory4Cost;
				factory4Count++;
				factory4Cost = 10000 * (1.07 * (factory4Count + 1));
			}
			return true;
		}
		return false;
		});

	

	//"loop" contains above functions, lets me run all their logic elsewhere
	//this doesn't actually "loop" in the C++ sense
	ftxui::Loop loop(&screen, app);

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		//STANDARDIZE
		curdsPerSecond = (factory1Count * factory1ProdRate) + (factory2Count * factory2ProdRate) + (factory3Count * factory3ProdRate) + (factory4Count * factory4ProdRate);
		
		auto now = std::chrono::steady_clock::now(); //take a timestamp
		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count(); //subtract time stored from time now, output seconds
		lastTime = now; //store a timestamp for the next compare, after the wait in this loop

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;
		
		screen.PostEvent(ftxui::Event::Custom); //push an event into the queue each frame. 
		//This is necessary when I have variables updating autonomously that I want displayed. "app" otherwise only checks if there are pending keyboard events.
		loop.RunOnce(); //do one pass of all my FTXUI functions. Doesn't hijack the program by waiting. If there are no pending inputs, moves on.
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	
	}

}

