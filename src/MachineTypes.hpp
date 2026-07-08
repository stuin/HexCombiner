#include "Machine.h"

static const std::map<int, std::string> machineNames = {
	{' '+MACHINE_EMPTY, "Empty"},
	{' '+MACHINE_BELT, "Belt"},
	{' '+MACHINE_MINE, "Mine"},
	{' '+MACHINE_ROTATOR, "Rotator"}
};

class Belt : public Machine {
public:
	int getType() {
		return MACHINE_BELT;
	}

	void process(double time) override {
		for(int d = 0; d < 6; d++)
			if(d != rotation && content[d] != NULL)
				prepareOutput(d, rotation);
	}

	bool canAttach(int dir, bool output) override {
		if(dir == rotation)
			return output;
		return !output;
	}
};

class Mine : public Machine {
public:
	int colors[12];
	FrameTimer timer;

	Mine(int _colors[12], float time) : timer(0, time) {
		for(int i = 0; i < 12; i++)
			colors[i] = _colors[i];
		maxItems = 0;
	}

	int getType() {
		return MACHINE_MINE;
	}

	void process(double time) override {
		if(currentItems == 0 && timer.next(time)) {
			content[rotation] = new Item();
			content[rotation]->setPosition(getPosition());
			content[rotation]->setColors(colors);
			UpdateList::addNode(content[rotation]);
			currentItems++;
		}
	}

	bool canAttach(int dir, bool output) override {
		if(dir == rotation)
			return output;
		return false;
	}
};

class Rotator : public Machine {
	int angle = 1;
	float delay = 1;
	float maxDelay = 1;

public:
	int getType() {
		return MACHINE_ROTATOR;
	}

	void process(double time) override {
		delay -= time;
		if(delay <= 0) {
			for(int d = 0; d < 6; d++) {
				if(d != rotation && content[d] != NULL) {
					prepareOutput(d, rotation)->rotate(1);
					delay = maxDelay;
				}
			}
		}
	}

	bool canAttach(int dir, bool output) override {
		if(dir == rotation)
			return output;
		if(reverseDir(dir) == rotation)
			return !output;
		return false;
	}
};