// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <thread>
#include <chrono>
#include <ftxui/ftxui.hpp>

int main()
{
	double currentCurds = 0, producedCurds = 0;
	long displayCurds = 0, curdsPerSecond = 0;
	int curdsPerClick = 1;
	bool running = 1, hasStarted = 0;

	class Generator {
		std::string name, key, upgradeKey;
		int count = 0, initialCost, scaledCost, prodRate, upgradeCost, upgradeThreshBase = 1, upgradeThreshCurrent;
		double costScalar, upgradeScalar = 1.0;
		ftxui::Color windowColor;

	public:
		Generator(std::string n, int ic, double s, int p, int uc, std::string k, std::string uk, ftxui::Color cl) {
			name = n;
			initialCost = ic;
			scaledCost = initialCost;
			costScalar = s;
			prodRate = p;
			upgradeCost = uc;
			upgradeThreshCurrent = upgradeThreshBase;
			key = k;
			upgradeKey = uk;
			windowColor = cl;
		}
		
		ftxui::Element displayInfo() const { //const means this never writes to the members, only reads
			return ftxui::window(
				ftxui::text(name) | ftxui::bold,
				ftxui::vbox({
					ftxui::text("Count: " + std::to_string(count)),
					ftxui::text("Cost: " + std::to_string(scaledCost)),
					ftxui::text("CPS each: " + std::to_string(prodRate)),
					ftxui::text("Upgrd Cost: " + std::to_string(upgradeCost)),
					ftxui::text("Upgrd Mult: x" + std::format("{:.1f}", upgradeScalar)),
					ftxui::text(key + " to buy"), //can't concatenate char + string
					ftxui::text(upgradeKey + " to upgrade"),
					})) | ftxui::color(windowColor);
			 
		}

		double getCPS() const {
			double CPS = (count * prodRate) * upgradeScalar;
			return CPS;
		}

		bool tryPurchase(ftxui::Event& event, double& currentCurds) {
			if (event == ftxui::Event::Character(key[0])) {
				if (currentCurds >= scaledCost) {
					currentCurds -= scaledCost;
					count++;
					scaledCost = initialCost * (costScalar * (count + 1));
				}
				return true;
			}
		}

		bool tryUpgrade(ftxui::Event& event, double& currentCurds) {
			if (event == ftxui::Event::Character(upgradeKey[0])) {
				if (currentCurds >= upgradeCost && count >= upgradeThreshCurrent) {
					currentCurds -= upgradeCost;
					upgradeScalar += 0.5; //needs tuning
					upgradeCost = upgradeCost * 2; //needs tuning
					upgradeThreshCurrent += upgradeThreshBase;
				}
				return true;
			}
		}
	};

	Generator gen1("Rats",				10,		1.07, 2,	9, "z", "a", ftxui::Color::White);
	Generator gen2("Mongers",			100,	1.07, 10,	9, "x", "s", ftxui::Color::DarkOrange);
	Generator gen3("Dairy Farms",		1000,	1.07, 100,	9, "c", "d", ftxui::Color::Yellow);
	Generator gen4("Industrial Plants",	10000,	1.07, 1000,	9, "v", "f", ftxui::Color::PaleTurquoise1);
	Generator gen5("Cheese Caves",		10000,	1.07, 1000, 9, "b", "g", ftxui::Color::BlueLight);
	Generator gen6("Fondue Fountains",	10000,	1.07, 1000, 9, "n", "h", ftxui::Color::GreenLight);

	//FTXUI setup, screen as persistent
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	auto curdButton = ftxui::Button("click for a cheese curd", [&] { currentCurds++; producedCurds++; });

	//display elements
	ftxui::Component renderer = ftxui::Renderer(curdButton, [&] {
		return ftxui::vbox({
			ftxui::text("Current Curds: " + std::to_string(displayCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
			ftxui::text("Produced Curds: " + std::to_string(producedCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold, //DEBUG hide in release version
			ftxui::text("Global CPS: " + std::to_string(curdsPerSecond)),
			ftxui::separator(),
			ftxui::hbox({
				gen1.displayInfo(), //maybe this could be a loop too
				gen2.displayInfo(),
				gen3.displayInfo(),
				gen4.displayInfo(),
				gen5.displayInfo(),
				gen6.displayInfo(),
			}),
			ftxui::separator(),
			curdButton -> Render(),
			ftxui::separator(),
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
		if (event == ftxui::Event::Character('0')) {
			running = 0;
			return true;
		}
		//could this be a loop?
		gen1.tryPurchase(event, currentCurds);
		gen2.tryPurchase(event, currentCurds);
		gen3.tryPurchase(event, currentCurds);
		gen4.tryPurchase(event, currentCurds);
		gen5.tryPurchase(event, currentCurds);
		gen6.tryPurchase(event, currentCurds);

		gen1.tryUpgrade(event, currentCurds);
		gen2.tryUpgrade(event, currentCurds);
		gen3.tryUpgrade(event, currentCurds);
		gen4.tryUpgrade(event, currentCurds);
		gen5.tryUpgrade(event, currentCurds);
		gen6.tryUpgrade(event, currentCurds);
		
		return false;
		});

	ftxui::Loop loop(&screen, app); //update screen, check manual inputs

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		curdsPerSecond = gen1.getCPS() + gen2.getCPS() + gen3.getCPS() + gen4.getCPS() + gen5.getCPS() + gen6.getCPS();
		
		auto now = std::chrono::steady_clock::now(); //finds time interval, used for calculating generated curds
		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count();
		lastTime = now;

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		producedCurds = producedCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;
		
		screen.PostEvent(ftxui::Event::Custom); //push autonomous updates to screen
		loop.RunOnce(); //non-blocking
		
		std::this_thread::sleep_for(std::chrono::milliseconds(16));	//60 "fps"
	}
}

