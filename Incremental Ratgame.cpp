// Incremental Ratgame.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <string>
#include <vector>
#include <functional>
#include <cmath>
#include <thread>
#include <chrono>
#include <ftxui/ftxui.hpp>

//used for message board
enum Message {
	intro,
	ratBuy, ratUpgrade,
	mongerBuy, mongerUpgrade,
	farmBuy, farmUpgrade,
	plantBuy, plantUpgrade,
	caveBuy, caveUpgrade,
	fountainBuy, fountainUpgrade,
	rocketBuy,
};

class MessageBoard {

public:
	Message current = intro;

	std::vector<std::string> messageBank = {
		"some say the moon is made of cheese",
		"you’ve started enlisting rats to pilfer cheese on your behalf",
		"with specialist training, your rats can now steal more effectively",
		"you’ve begun staffing full-time cheese brokers",
		"thanks to AI-augmented workflows, your cheese mongers can now buy and sell more efficiently",
		"you’ve gone straight to the source, controlling the milk supply",
		"by investing in sophisticated multi-milker technology, you can extract much more dairy product from your livestock",
		"organic production can’t keep up, you’ve decided to start producing “cheese products” at scale",
		"UPGRADE INDUSTRIAL PLANT MSG GOES HERE, something with nuclear fusion",
		"in your relentless pursuit, you’ve discovered massive cheese deposits deep within the earth",
		"you’ve paid off local politicians to approve your cheese fracking permits",
		"ponce de Leon dreamt of finding the legendary Fountains of Cheese, now you’ve claimed them for yourself",
		"your countless sacrifices to the divine cheese are paying off",
		"you’ve left the world behind, to find out for yourself. Is the moon truly made of cheese?",
	};

	ftxui::Element displayMessage() const {
		return ftxui::text(messageBank[current]);
	}

	void updateMessage(Message update) {
		current = update;
	}
};

//define button visual behaviors
class ShopButton {
	bool hovered = 0;
	std::function<bool()> canAfford; //callback, allows me to define the conditions in the gen class
	ftxui::Component component;

public:
	ShopButton(std::string label, std::function<bool()> af, std::function<void()> click)
		: canAfford(af)
	{
		ftxui::ButtonOption option;
		option.transform = [this](const ftxui::EntryState& state) {
			ftxui::Element e = ftxui::text(" " + state.label + " ");
			if (!canAfford()) {
				return e | ftxui::dim;
			}
			if (hovered) {
				return e | ftxui::inverted;
			}
			return e;
			};

		auto button = ftxui::Button(label, click, option);

		component = ftxui::Hoverable(button, &hovered);
	}

	ShopButton(const ShopButton&) = delete;
	ShopButton& operator = (const ShopButton&) = delete;

	ftxui::Component getComponent() const { return component; }
	ftxui::Element Render() const { return component->Render(); }
};

class Generator {
	std::string name;
	int count = 0, initialCost, scaledCost, prodRate, upgradeCost, upgradeThreshBase = 10, //TUNE
		upgradeThreshCurrent, upgradeCount = 0;
	double costScalar, upgradeScalar = 1.0;
	double& curds;
	ftxui::Color windowColor;
	Message buy, upgrade;
	MessageBoard& board;
	ShopButton buyButton;
	ShopButton upgradeButton;

public:
	Generator(std::string n, int ic, double s, int p, int uc, ftxui::Color cl, double& cRef, Message bm, Message um, MessageBoard& mb)
		: name(n), initialCost(ic), scaledCost(initialCost), costScalar(s), prodRate(p), upgradeCost(uc),
		upgradeThreshCurrent(upgradeThreshBase), windowColor(cl), curds(cRef), buy(bm), upgrade(um), board(mb),
		buyButton("[ BUY] ", 
			[this] { return curds >= scaledCost; },
			[this] { tryPurchase(curds); }),
		upgradeButton("[ UPGRADE ]", 
			[this] { return (curds >= upgradeCost) && (count >= upgradeThreshCurrent);  },
			[this] { tryUpgrade(curds); })
		{
	}

	ftxui::Component getComponent() {
		return ftxui::Container::Vertical({ buyButton.getComponent(), upgradeButton.getComponent()}); //group object's buttons for component tree
	}

	ftxui::Element displayInfo() const {
		return ftxui::window(
			ftxui::text(name) | ftxui::bold,
			ftxui::vbox({
				ftxui::text("Count: " + std::to_string(count)),
				ftxui::text("Cost: " + std::to_string(scaledCost)),
				ftxui::text("CPS each: " + std::to_string(prodRate)),
				ftxui::text("UPG Cost: " + std::to_string(upgradeCost)),
				ftxui::text("UPG Mult: x" + std::format("{:.1f}", upgradeScalar)),
				buyButton.Render(),
				upgradeButton.Render(),
				})) | ftxui::color(windowColor);
	}

	double getCPS() const {
		double CPS = (count * prodRate) * upgradeScalar; //from MoIG
		return CPS;
	}

	void tryPurchase(double& currentCurds) { 
		if (currentCurds >= scaledCost) {
			currentCurds -= scaledCost;
			count++;
			scaledCost = static_cast<int>(initialCost * std::pow(costScalar, (count))); //from MoIG
			if (count == 1) {
				board.updateMessage(buy);
			}
		}
	}
	void tryUpgrade(double& currentCurds) {
		if (currentCurds >= upgradeCost && count >= upgradeThreshCurrent) {
			currentCurds -= upgradeCost;
			upgradeCount++;
			upgradeScalar *= 2; //TUNE
			upgradeCost *= 2; //TUNE
			upgradeThreshCurrent += upgradeThreshBase;
			if (upgradeCount == 1) {
				board.updateMessage(upgrade);
			}
		}
	}
};

class Accumulator {
	std::chrono::steady_clock::time_point timeStamp;
	std::chrono::steady_clock::time_point& now;
	std::chrono::seconds thresh{ 120 }; //duration taken before granting curds
	bool hasPurchased = 0;
	int durationToGrant = 60; //seconds worth of generation given when thresh is reached
	int upgradeCount = 0, upgradeLimit = 5; //TUNE
	double cost = 6000, upgradeCost = 25000;
	double& curds;
	ftxui::Component buyButton;
	ftxui::Component upgradeButton;

public:
	float accumProgress = 0.0;
	Accumulator(double& cRef, std::chrono::steady_clock::time_point& n)
		: curds(cRef), now(n) {
		buyButton = ftxui::Button("BUY", [this] { tryPurchase(curds, now); }, ftxui::ButtonOption::Ascii());
		upgradeButton = ftxui::Button("UPGRADE", [this] { tryUpgrade(curds); }, ftxui::ButtonOption::Ascii());
	}

	ftxui::Component getComponent() {
		return ftxui::Container::Vertical({ buyButton, upgradeButton }); //group object's buttons for component tree
	}

	void tryPurchase(double& currentCurds, std::chrono::steady_clock::time_point& now) {
		if (hasPurchased == 0 && currentCurds >= cost) {
			hasPurchased = 1;
			currentCurds -= cost;
			timeStamp = now;
		}
	}

	void tryUpgrade(double& currentCurds) {
		if (upgradeCount < upgradeLimit && currentCurds >= upgradeCost) {
			currentCurds -= upgradeCost;
			thresh -= std::chrono::seconds(15); //TUNE
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
	long displayCurds = 0, curdsPerSecond = 0, clickUpgradeCost = 10, rocketCost = 400000000; //TUNE
	int curdsPerClick = 1, clickUpgradeCount = 0;
	bool running = 1;

	MessageBoard msgBoard({});

	//				name				cost		cScalar	CPS		uCost
	Generator gen1("RATS",				15,			1.15,	1,		250,			ftxui::Color::White, currentCurds, ratBuy, ratUpgrade, msgBoard);
	Generator gen2("MONGERS",			150,		1.15,	5,		3000,			ftxui::Color::DarkOrange, currentCurds, mongerBuy, mongerUpgrade, msgBoard);
	Generator gen3("DAIRY FARMS",		2500,		1.15,	30,		40000,			ftxui::Color::Yellow, currentCurds, farmBuy, farmUpgrade, msgBoard);
	Generator gen4("INDUSTRIAL PLANTS",	40000,		1.15,	180,	600000,			ftxui::Color::PaleTurquoise1, currentCurds, plantBuy, plantUpgrade, msgBoard);
	Generator gen5("CHEESE CAVES",		600000,		1.15,	1100,	8000000,		ftxui::Color::BlueLight, currentCurds, caveBuy, caveUpgrade, msgBoard);
	Generator gen6("FONDUE FOUNTAINS",	9000000,	1.15,	6500,	120000000,		ftxui::Color::GreenLight, currentCurds, fountainBuy, fountainUpgrade, msgBoard);

	auto now = std::chrono::steady_clock::now();  //init, used for finding time intervals
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
	
	auto rocketButton = ftxui::Button("BUY", [&] { 
		if (currentCurds >= rocketCost) { 
			msgBoard.updateMessage(rocketBuy); 
		}
		}, ftxui::ButtonOption::Ascii()) | ftxui::color(ftxui::Color::Blue);
	
	auto allButtons = ftxui::Container::Horizontal({ //component tree?
		curdButton,
		clickUpgradeButton,
		gen1.getComponent(),
		gen2.getComponent(),
		gen3.getComponent(),
		gen4.getComponent(),
		gen5.getComponent(),
		gen6.getComponent(),
		curdAccum.getComponent(),
		rocketButton,
		});

	//FTXUI setup, screen as persistent
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::Fullscreen();

	//display elements
	ftxui::Component renderer = ftxui::Renderer(allButtons, [&] {
		return ftxui::vbox({
			msgBoard.displayMessage(),
				ftxui::separator(),
			ftxui::hbox({
				ftxui::vbox({
					ftxui::text("Current Curds: " + std::to_string(displayCurds)) | ftxui::color(ftxui::Color::Yellow) | ftxui::bold,
					ftxui::text("Produced Curds: " + std::to_string(producedCurds) + "\n") | ftxui::color(ftxui::Color::Yellow) | ftxui::bold, //DEBUG hide in release version, used for revealing
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
				gen1.displayInfo(),
				gen2.displayInfo(),
				gen3.displayInfo(),
				gen4.displayInfo(),
				gen5.displayInfo(),
				gen6.displayInfo(),
			}),
			ftxui::separator(),
			curdAccum.displayInfo(),
			ftxui::window(
				ftxui::text("ROCKET") | ftxui::bold,
				ftxui::vbox({
					ftxui::text("To the moon"),
					ftxui::text("Cost: " + std::to_string(rocketCost)),
					rocketButton->Render(),
				}) 
			) | ftxui::color(ftxui::Color::Blue),
			
			ftxui::text("press q to quit") | ftxui::color(ftxui::Color::Yellow) | ftxui::dim,
			}) | ftxui::border;
		});

	//handle keyboard input
	//the only keyboard input now is to quit... maybe unecessary?
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
		
		now = std::chrono::steady_clock::now(); //init moved up (used for accum)
		curdAccum.Call(now, currentCurds, curdsPerSecond);

		double elapsedSeconds = std::chrono::duration<double>(now - lastTime).count();
		lastTime = now;

		currentCurds = currentCurds + (curdsPerSecond * elapsedSeconds);
		producedCurds = producedCurds + (curdsPerSecond * elapsedSeconds);
		displayCurds = currentCurds;

		screen.PostEvent(ftxui::Event::Custom); //push autonomous (gen) updates to screen
		loop.RunOnce(); //non-blocking

		std::this_thread::sleep_for(std::chrono::milliseconds(16));	//60 "fps"
	}
}

