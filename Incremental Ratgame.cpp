// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <thread>
#include <chrono>
#include <ftxui/ftxui.hpp>

class Generator {
	std::string name, key, upgradeKey;
	int count = 0, initialCost, scaledCost, prodRate, upgradeCost, upgradeThreshBase = 1, upgradeThreshCurrent;
	double costScalar, upgradeScalar = 1.0;
	double& curds;
	ftxui::Color windowColor;
	ftxui::Component buyButton;
	ftxui::Component upgradeButton;

public:
	Generator(std::string n, int ic, double s, int p, int uc, std::string k, std::string uk, ftxui::Color cl, double& cRef)
		: name(n), initialCost(ic), scaledCost(initialCost), costScalar(s), prodRate(p), upgradeCost(uc),
		upgradeThreshCurrent(upgradeThreshBase), key(k), upgradeKey(uk), windowColor(cl), curds(cRef)
	{
		buyButton = ftxui::Button("BUY", [this] { tryPurchase(curds); }, ftxui::ButtonOption::Ascii());
		upgradeButton = ftxui::Button("UPGRADE", [this] { tryUpgrade(curds); }, ftxui::ButtonOption::Ascii());
	}

	ftxui::Component getComponent() {
		return ftxui::Container::Vertical({ buyButton, upgradeButton }); //group object's buttons for component tree
	}

	ftxui::Element displayInfo() const {
		return ftxui::window(
			ftxui::text(name) | ftxui::bold,
			ftxui::vbox({
				ftxui::text("Count: " + std::to_string(count)),
				ftxui::text("Cost: " + std::to_string(scaledCost)),
				ftxui::text("CPS each: " + std::to_string(prodRate)),
				ftxui::text("Upgrd Cost: " + std::to_string(upgradeCost)),
				ftxui::text("Upgrd Mult: x" + std::format("{:.1f}", upgradeScalar)),
				buyButton->Render(),
				upgradeButton->Render(),
				})) | ftxui::color(windowColor);
	}

	double getCPS() const {
		double CPS = (count * prodRate) * upgradeScalar;
		return CPS;
	}
	void tryPurchase(double& currentCurds) {
		if (currentCurds >= scaledCost) {
			currentCurds -= scaledCost;
			count++;
			scaledCost = initialCost * (costScalar * (count + 1));
		}
	}
	void tryUpgrade(double& currentCurds) {
		if (currentCurds >= upgradeCost && count >= upgradeThreshCurrent) {
			currentCurds -= upgradeCost;
			upgradeScalar += 0.5; //needs tuning
			upgradeCost = upgradeCost * 2; //needs tuning
			upgradeThreshCurrent += upgradeThreshBase;
		}
	}
};

class Accumulator {
	std::chrono::steady_clock::time_point timeStamp, now;
	std::chrono::seconds thresh{ 10 }; //duration taken before granting curds
	bool hasPurchased = 0;
	int durationToGrant = 666; //seconds worth of generation given when thresh is reached
	int upgradeCount = 0, upgradeLimit = 5; //needs tuning
	double cost = 10, upgradeCost = 10;
	double& curds;
	ftxui::Component buyButton;
	ftxui::Component upgradeButton;
	

public:
	float accumProgress = 0.0;
	Accumulator(double& cRef, std::chrono::steady_clock::time_point n)
		: curds(cRef), now(n) {
		buyButton = ftxui::Button("BUY", [this] { tryPurchase(curds, now); }, ftxui::ButtonOption::Ascii());
		upgradeButton = ftxui::Button("UPGRADE", [this] { tryUpgrade(curds); }, ftxui::ButtonOption::Ascii());
	}

	ftxui::Component getComponent() {
		return ftxui::Container::Vertical({ buyButton, upgradeButton }); //group object's buttons for component tree
	}

	void tryPurchase(double& currentCurds, std::chrono::steady_clock::time_point now) {
		if (hasPurchased == 0 && currentCurds >= cost) {
			hasPurchased = 1;
			currentCurds -= cost;
			timeStamp = now;
		}
	}

	void tryUpgrade(double& currentCurds) {
		if (upgradeCount < upgradeLimit && currentCurds >= upgradeCost) {
			thresh -= std::chrono::seconds(5); //tune this
			upgradeCount++;
		}
		//else (you've hit the limit) message board?
	}

	void Call(std::chrono::steady_clock::time_point now, double& currentCurds, long& curdsPerSecond) {
		if (hasPurchased == 1) {
			accumProgress = static_cast<float>(std::chrono::duration<double>(now - timeStamp) / thresh); //lots of type conversions goin on
			if (now - timeStamp >= thresh) {
				currentCurds += curdsPerSecond * durationToGrant;
				timeStamp = now;
			}
		}
	}
	ftxui::Element progressBar() const {
		return ftxui::hbox({
		ftxui::text("Progress: "),
		ftxui::gauge(accumProgress) | ftxui::flex,
		ftxui::text(" " + std::to_string(int(accumProgress * 100)) + "%"),
			});
	}

	ftxui::Element displayInfo() const {
		return ftxui::window(
			ftxui::text("CURD ACCUMULATOR") | ftxui::bold,
			ftxui::vbox({
				ftxui::hbox({
					ftxui::text("Interval: " + std::format("{}", thresh)),
					ftxui::text(" Curds Granted: x" + std::to_string(durationToGrant)),
					buyButton->Render(),
					upgradeButton->Render(),
				}),
				progressBar(),
				})
				) | ftxui::color(ftxui::Color::Yellow);
	}
};

int main()
{
	double currentCurds = 0, producedCurds = 0, clickCostScalar = 1.07;
	long displayCurds = 0, curdsPerSecond = 0, clickUpgradeCost = 10;
	int curdsPerClick = 1, clickUpgradeCount = 0;
	bool running = 1;

	//				name				cost	cScalar	CPS		uCost
	Generator gen1("RATS",				10,		1.07,	2,		9,		"z", "a", ftxui::Color::White, currentCurds);
	Generator gen2("MONGERS",			100,	1.07,	10,		9,		"x", "s", ftxui::Color::DarkOrange, currentCurds);
	Generator gen3("DAIRY FARMS",		1000,	1.07,	100,	9,		"c", "d", ftxui::Color::Yellow, currentCurds);
	Generator gen4("INDUSTRIAL PLANTS",	10000,	1.07,	1000,	9,		"v", "f", ftxui::Color::PaleTurquoise1, currentCurds);
	Generator gen5("CHEESE CAVES",		10000,	1.07,	1000,	9,		"b", "g", ftxui::Color::BlueLight, currentCurds);
	Generator gen6("FONDUE FOUNTAINS",	10000,	1.07,	1000,	9,		"n", "h", ftxui::Color::GreenLight, currentCurds);

	auto now = std::chrono::steady_clock::now();  //finds time interval, used for calculating generated curds
	Accumulator curdAccum(currentCurds, now);
	

	//click button functions. Maybe move to class later
	auto curdButton = ftxui::Button("CLICK for cheese curds", [&] { currentCurds += curdsPerClick; producedCurds += curdsPerClick; }, ftxui::ButtonOption::Animated());
	auto clickUpgradeButton = ftxui::Button("UPGRADE", [&] { 
		if (currentCurds >= clickUpgradeCost) {
			currentCurds -= clickUpgradeCost;
			clickUpgradeCount++;
			curdsPerClick++;
			clickUpgradeCost = clickUpgradeCost * (clickCostScalar * (clickUpgradeCount + 1));
		}
		}, ftxui::ButtonOption::Ascii());
	auto allButtons = ftxui::Container::Horizontal({ //component tree
		curdButton,
		clickUpgradeButton,
		gen1.getComponent(),
		gen2.getComponent(),
		gen3.getComponent(),
		gen4.getComponent(),
		gen5.getComponent(),
		gen6.getComponent(),
		curdAccum.getComponent(),
		});

	//FTXUI setup, screen as persistent
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//display elements
	ftxui::Component renderer = ftxui::Renderer(allButtons, [&] {
		return ftxui::vbox({
			ftxui::text("Message Board Goes Here") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
				ftxui::separator(),
			ftxui::hbox({
				ftxui::vbox({
					ftxui::text("Current Curds: " + std::to_string(displayCurds)) | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
					ftxui::text("Produced Curds: " + std::to_string(producedCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold, //DEBUG hide in release version
					ftxui::text("Global CPS: " + std::to_string(curdsPerSecond)),
				}),
				ftxui::filler(),
				curdButton->Render(),
				ftxui::window(
					ftxui::text("Per Click") | ftxui::bold,
					ftxui::vbox({
						ftxui::text("curds produced: " + std::to_string(curdsPerClick)),
						ftxui::text("Upgrade Cost: " + std::to_string(clickUpgradeCost)),
						clickUpgradeButton->Render(),
					})
				)
			}),
			ftxui::separator(),
			ftxui::hbox({
				gen1.displayInfo(), //maybe this could be a loop
				gen2.displayInfo(),
				gen3.displayInfo(),
				gen4.displayInfo(),
				gen5.displayInfo(),
				gen6.displayInfo(),
			}),
			ftxui::separator(),
			curdAccum.displayInfo(),
			ftxui::text("press q to quit") | ftxui::color(ftxui::Color::Yellow) | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	ftxui::Component app = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
		if (event == ftxui::Event::Character('q')) {
			running = 0;
			return true;
		}
		return false;
		});

	ftxui::Loop loop(&screen, app); //update screen, check manual inputs

	auto lastTime = std::chrono::steady_clock::now();

	while (running == 1) { //MAIN GAME LOOP

		curdsPerSecond = gen1.getCPS() + gen2.getCPS() + gen3.getCPS() + gen4.getCPS() + gen5.getCPS() + gen6.getCPS();
		
		now = std::chrono::steady_clock::now(); //initialization moved up (used for accum)
		//accumulator step. make sure it's non-blocking
		curdAccum.Call(now, currentCurds, curdsPerSecond);

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

