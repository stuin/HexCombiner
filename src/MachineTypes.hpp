#include "Machine.h"

class Belt : public Machine {
	bool crossStyle = false;

public:
	Belt(bool _crossStyle) {
		crossStyle = _crossStyle;
	}

	int getType() {
		return crossStyle ? MACHINE_XBELT : MACHINE_BELT;
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

class Distribute : public Machine {
public:
	int lastOutput = 0;

	int getType() {
		return MACHINE_DISTRIBUTE;
	}

	void process(double time) override {
		int r = reverseDir(rotation);
		for(int d1 = lastOutput+1; d1<=lastOutput+6; d1++) {
			int d = d1 % 6;

			if(content[r] != NULL && content[d] == NULL && hasEdge(d)) {
				Machine *other = ((Machine*)getVertex(d));
				if(other->canInput(reverseDir(d))) {
					prepareOutput(r, d);
					//other->input(reverseDir(d), output(d));
				}
			}
		}
	}

	void onOutput(int dir) {
		lastOutput = dir;
	}

	bool canAttach(int dir, bool output) override {
		if(reverseDir(dir) == rotation)
			return !output;
		return output;
	}
};

class Crossover : public Machine {
	bool output[6] = {false};

public:
	Crossover() {
		maxItems = 3;
	}

	int getType() {
		return MACHINE_CROSSOVER;
	}

	void process(double time) override {
		for(int d = 0; d < 6; d++) {
			if(content[d] != NULL && !output[d]) {
				prepareOutput(d, reverseDir(d));
				output[reverseDir(d)] = true;
			}
		}
	}

	void onOutput(int dir) {
		output[dir] = false;
	}

	bool canAttach(int dir, bool output) override {
		int r = reverseDir(dir);
		if(hasEdge(r)) {
			Machine *other = ((Machine*)getVertex(r));
			return other->canAttach(dir, output);
		}
		return true;
	}
};

class Mine : public Machine {
public:
	int color;
	float delay = 0;
	float maxDelay = 4;

	Mine(int _color) {
		color = _color;
		maxItems = 0;
	}

	int getType() {
		return MACHINE_MINE;
	}

	void process(double time) override {
		delay -= time;
		if(currentItems == 0 && delay <= 0) {
			content[rotation] = new Item(ITEMS, 1);
			content[rotation]->setPosition(getPosition());
			content[rotation]->colors[reverseDir(rotation)] = color;
			content[rotation]->updateRects();
			UpdateList::addNode(content[rotation]);
			currentItems++;
			delay = maxDelay;
		}
	}

	bool canAttach(int dir, bool output) override {
		if(dir == rotation)
			return output;
		return false;
	}
};

class Rotate : public Machine {
	int angle = 1;
	float delay = 1;
	float maxDelay = 1;

public:
	Rotate(int _angle) {
		angle = _angle;
	}

	int getType() {
		return (angle == 1) ? MACHINE_ROTATER : MACHINE_ROTATEL;
	}

	void process(double time) override {
		delay -= time;
		if(delay <= 0) {
			for(int d = 0; d < 6; d++) {
				if(d != rotation && content[d] != NULL) {
					prepareOutput(d, rotation)->rotate(angle);
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

class Merge : public Machine {
	float delay = 1;
	float maxDelay = 1;

public:
	Merge() {
		maxItems = 5;
	}

	int getType() {
		return MACHINE_MERGE;
	}

	void process(double time) override {
		int outputEdge = (hasEdge(rotation)) ? 1 : 0;
		if(arrivedItems > 1 && arrivedItems + outputEdge == countEdges()) {
			delay -= time;
			if(delay <= 0) {
				Item *first = NULL;
				for(int d = 0; d < 6; d++) {
					if(d != rotation && content[d] != NULL) {
						if(first == NULL) {
							first = prepareOutput(d, rotation);
							delay = maxDelay;
						} else {
							if(!first->merge(content[d]))
								setDelete();
							deleteItem(d);
						}
					}
				}
			}
		}
	}

	bool canAttach(int dir, bool output) override {
		if(dir == rotation)
			return output;
		return !output;
	}
};

class Hole : public Machine {
public:
	Item display;
	Node scoreText;

	Hole(int _colors[12]) : display(HOLEITEMS, 0.5), scoreText(SCORES, RENDER_STRING, Vector2i(16, 16), this) {
		display.setColors(_colors);
		display.updateRects();
		display.setParent(this);
		UpdateList::addNode(&display);

		scoreText.setString("");
		scoreText.setTexture(FONT_POINTS);
		if(scoreIndexes->getInt(display.getId()+1) > 0)
			scoreIndexes->linkNode(display.getId()+1, &scoreText);
		UpdateList::addNode(&scoreText);

		maxItems = 1;
	}

	int getType() {
		return MACHINE_HOLE;
	}

	void process(double time) override {
		for(int d = 0; d < 6; d++) {
			if(content[d] != NULL) {
				if(display.compare(content[d])) {
					scoreIndexes->increment(0, false);
					scoreIndexes->increment(display.getId()+1);
					scoreIndexes->linkNode(display.getId()+1, &scoreText);
					scoreIndexes->linkNode(0, UpdateList::getNode(PLAYERSCORE));
					//std::cout << "Scored at " << display.getId() << "\n";
				} else if(false) {
					std::cout << "Did not score at " << display.getId() << " ";
					for(int c = 0; c < 6; c++)
						if(display.colors[c] != content[d]->colors[c])
							std::cout << display.colors[c] << "/" << content[d]->colors[c];
					std::cout << "\n";
				}
				deleteItem(d);
			}
		}
	}

	bool canAttach(int dir, bool output) override {
		return !output;
	}
};