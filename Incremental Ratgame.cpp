// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

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
	bool running = 1;
	bool hasStarted = 0;

	class Generator {
		std::string name;
		int count = 0;
		int initialCost = 10;
		int scaledCost = initialCost;
		double costScalar = 1.07;
		int prodRate = 2;

	public:

		Generator(std::string n, int ic, double s, int p) {
			name = n;
			initialCost = ic;
			costScalar = s;
			prodRate = p;
		}
		
		ftxui::Element displayInfo() const { //const means this never writes to the members, only reads
			return ftxui::text(name + "Count: " + std::to_string(count) + " (cost : " + std::to_string(scaledCost) + ")") | ftxui::color(ftxui::Color::LightYellow3);
		}

		double getCPS() const {
			double CPS = count * prodRate;
			return CPS;
		}

		void increment(double &currentCurds) {
			if (currentCurds >= scaledCost) {
				currentCurds -= scaledCost;
				count++;
				scaledCost = initialCost * (costScalar * (count + 1));
			}
		}
	};

	Generator gen1("gen1", 10, 1.07, 2);
	Generator gen2("gen2", 100, 1.07, 10);
	Generator gen3("gen3", 1000, 1.07, 100);
	Generator gen4("gen4", 10000, 1.07, 1000);

	//FTXUI setup
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//button 
	auto curdButton = ftxui::Button("Click for a cheese curd", [&] { currentCurds++; });

	//display persistent elements on screen
	ftxui::Component renderer = ftxui::Renderer(curdButton, [&] { //lambda?
		return ftxui::vbox({
			ftxui::text("Curds: " + std::to_string(displayCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
			gen1.displayInfo(),
			gen2.displayInfo(),
			gen3.displayInfo(),
			gen4.displayInfo(),
			ftxui::separator(),
			curdButton -> Render(),
			ftxui::separator(),
			ftxui::text("press z to buy a gen1\npress x to buy a gen2\npress c to buy a gen3\npress v to buy a gen4\npress q to quit") | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) { //lambda?
		if (event == ftxui::Event::Character('q')) {
			running = 0;
			return true;
		}
		if (event == ftxui::Event::Character('z')) {
			gen1.increment(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('x')) {
			gen2.increment(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('c')) {
			gen3.increment(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('v')) {
			gen4.increment(currentCurds);
			return true;
		}
		return false;
		});

	//contains above functions, lets me run all their logic elsewhere
	ftxui::Loop loop(&screen, app);

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		curdsPerSecond = gen1.getCPS() + gen2.getCPS() + gen3.getCPS() + gen4.getCPS();
		
		auto now = std::chrono::steady_clock::now(); //take a timestamp
		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count(); //subtract time stored from time now, output seconds
		lastTime = now; //store a timestamp for the next subtraction, which happens after the wait in this loop

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;
		
		screen.PostEvent(ftxui::Event::Custom); //push an event into the queue each frame. 
		//This is necessary when I have variables updating autonomously that I want displayed. "app" otherwise only checks if there are pending keyboard events.
		loop.RunOnce(); //do one pass of all my FTXUI functions. Doesn't hijack the program by waiting. If there are no pending inputs, moves on.
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	//60 "fps"
	}

}

