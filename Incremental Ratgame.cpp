// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <thread>
#include <chrono>
#include <ftxui/ftxui.hpp>

int main()
{
	double currentCurds = 0;
	long displayCurds = 0, curdsPerSecond = 0;
	int curdsPerClick = 1;
	bool running = 1, hasStarted = 0;

	class Generator {
		std::string name;
		int count = 0, initialCost, scaledCost, prodRate, upgradeCost, upgradeThreshBase = 50, upgradeThreshCurrent;
		double costScalar, upgradeScalar = 1.0;

	public:

		Generator(std::string n, int ic, double s, int p, int uc) {
			name = n;
			initialCost = ic;
			scaledCost = initialCost;
			costScalar = s;
			prodRate = p;
			upgradeCost = uc;
			upgradeThreshCurrent = upgradeThreshBase;
		}
		
		ftxui::Element displayInfo() const { //const means this never writes to the members, only reads
			return ftxui::window(
				ftxui::text(name) |ftxui::bold, 
				ftxui::vbox({
					ftxui::text("Count: " + std::to_string(count)),
					ftxui::text("Cost: " + std::to_string(scaledCost)),
					ftxui::text("Indiv. CPS: " + std::to_string(prodRate)),
					ftxui::text("Upgrade Mult: x" + std::format("{:.1f}", upgradeScalar)),
					}
			)) | ftxui::color(ftxui::Color::LightYellow3);
		}
		std::string getName() {
			return name;
		}

		double getCPS() const {
			double CPS = (count * prodRate) * upgradeScalar;
			return CPS;
		}

		void increment(double &currentCurds) {
			if (currentCurds >= scaledCost) {
				currentCurds -= scaledCost;
				count++;
				scaledCost = initialCost * (costScalar * (count + 1));
			}
		}

		void upgrade(double &currentCurds) {
			if (currentCurds >= upgradeCost && count >= upgradeThreshCurrent) {
				currentCurds -= upgradeCost;
				upgradeScalar += 0.5;
				upgradeThreshCurrent += upgradeThreshBase;
			}
		}
	};

	Generator gen1("Rats", 10, 1.07, 2, 9);
	Generator gen2("Goats", 100, 1.07, 10, 9);
	Generator gen3("Cows", 1000, 1.07, 100, 9);
	Generator gen4("Mongers", 10000, 1.07, 1000, 9);
	//Generator gen5("Dairy Farms", 10000, 1.07, 1000, 9);
	//Generator gen6("Industrial Plants", 10000, 1.07, 1000, 9);
	//Generator gen7("Cheese Caves", 10000, 1.07, 1000, 9);

	//FTXUI setup, screen as persistent
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	auto curdButton = ftxui::Button("click for a cheese curd", [&] { currentCurds++; });

	//display elements
	ftxui::Component renderer = ftxui::Renderer(curdButton, [&] {
		return ftxui::vbox({
			ftxui::text("Current Curds: " + std::to_string(displayCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
			ftxui::text("Global CPS: " + std::to_string(curdsPerSecond)),
			ftxui::separator(),
			ftxui::hbox({
				gen1.displayInfo(), //maybe this could be a loop too
				gen2.displayInfo(),
				gen3.displayInfo(),
				gen4.displayInfo(),
				//gen5.displayInfo(),
				//gen6.displayInfo(),
				//gen7.displayInfo(),
			}),
			ftxui::separator(),
			curdButton -> Render(),
			ftxui::separator(),
			ftxui::text(
				"press z to buy " + gen1.getName() + "\n"
				"press a to buy " + gen1.getName() + " upgrade\n"
				"press x to buy " + gen2.getName() + "\n"
				"press s to buy " + gen2.getName() + " upgrade\n"
				"press c to buy " + gen3.getName() + "\n"
				"press d to buy " + gen3.getName() + " upgrade\n"
				"press v to buy " + gen4.getName() + "\n"
				"press f to buy " + gen4.getName() + " upgrade\n"
				"press 0 to quit") | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) { //lambda?
		if (event == ftxui::Event::Character('0')) {
			running = 0;
			return true;
		}
		//these have the same shape... how could this be a loop?
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
		if (event == ftxui::Event::Character('a')) {
			gen1.upgrade(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('s')) {
			gen2.upgrade(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('d')) {
			gen3.upgrade(currentCurds);
			return true;
		}
		if (event == ftxui::Event::Character('f')) {
			gen4.upgrade(currentCurds);
			return true;
		}
		return false;
		});

	ftxui::Loop loop(&screen, app); //update screen, check manual inputs

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		curdsPerSecond = gen1.getCPS() + gen2.getCPS() + gen3.getCPS() + gen4.getCPS();
		
		auto now = std::chrono::steady_clock::now(); //finds time interval, used for calculating generated curds
		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count();
		lastTime = now;

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;
		
		screen.PostEvent(ftxui::Event::Custom); //push autonomous updates to screen
		loop.RunOnce(); //non-blocking
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	//60 "fps"
	}
}

