#include "Skyrmion/tiling/NodeIndexer.hpp"
#include "Skyrmion/util/AnimatedNode.hpp"
#include "Skyrmion/util/VertexGraph.hpp"

#include "item.h"

enum MACHINE_DIRECTIONS {
	UP_RIGHT,
	DOWN_LEFT,
	LEFT,
	RIGHT,
	DOWN_RIGHT,
	UP_LEFT
};

NodeIndexer *machineIndexes = NULL;

class Machine : public Node, public Vertex<6> {
	Vector2i gridPosition;

	int arrivedItems = 0;

public:
	Item* content[6];

	int rotation = 0;
	int maxItems = 1;
	int currentItems = 0;
	float travelSpeed = 0.5;

	Machine() : Node(MACHINES, RENDER_COLOR_SINGLE), Vertex(NULL) {
		setColor(COLOR_PURPLE);
	}

	void setGridPosition(int x, int y, int _rotation) {
		gridPosition = Vector2i(x, y);
		rotation = _rotation;

		machineIndexes->setNode(gridPosition, this);
		setPosition(gridPosition * Vector2i(48, 43) + Vector2i(24, 12));
		if(y%2==1)
			setPosition(getPosition()+Vector2f(24, 0));
	}

	void update(double time) {
		for(int d = 0; d<6; d++) {
			//Process inputs
			if(content[d] != NULL && canAttach(d, false)) {
				if(content[d]->getPosition() != getPosition()) {
					content[d]->travelProgress += travelSpeed * time;
					content[d]->setPosition(lerp(content[d]->from, getPosition(), content[d]->travelProgress));
				}
				if(content[d]->getPosition() == getPosition() && content[d]->travelProgress >= 1) {
					arrivedItems++;
					content[d]->travelProgress = 0;
				}
			}

			//Attach to output
			if(!hasEdge(d) && canAttach(d, true) && machineIndexes != NULL) {
				Machine *other = (Machine*)machineIndexes->getNode(dirPos(d));
				if(other != NULL && other->canAttach(reverseDir(d), false)) {
					setVertex(d, reverseDir(d), other);
					//std::cout << "Connected machine " << getId() << " to " << other->getId() << " with " << d << "\n";
				}
			}

			//Send item to output
			if(hasEdge(d) && canOutput(d)) {
				Machine *other = ((Machine*)getVertex(d));
				if(other->canInput(reverseDir(d)))
					other->input(reverseDir(d), output(d));
			}
		}

		if(arrivedItems == maxItems)
			process(time);
	}

	void input(int dir, Item *item) {
		content[dir] = item;
		currentItems++;
	}

	Item *output(int dir) {
		Item *item = content[dir];
		content[dir] = NULL;
		item->from = item->getPosition();
		item->travelProgress = 0;
		currentItems--;
		return item;
	}

	Item *prepareOutput(int from, int to) {
		if(content[to] == NULL) {
			content[to] = content[from];
			content[from] = NULL;
			arrivedItems--;
			return content[to];
		}
		return NULL;
	}

	bool canInput(int dir) {
		return canAttach(dir, false) && content[dir] == NULL && currentItems < maxItems;
	}

	bool canOutput(int dir) {
		return canAttach(dir, true) && content[dir] != NULL;
	}

	virtual void process(double time) {}
	virtual bool canAttach(int dir, bool output) { return false; }

	int reverseDir(int dir) {
		if(dir % 2 == 0)
			return dir + 1;
		return dir - 1;
	}

	Vector2i dirPos(int dir) {
		int offset = (gridPosition.y%2==1) ? 1 : 0;
		switch(dir) {
		case UP_RIGHT:
			return gridPosition + Vector2i(offset, -1);
		case DOWN_RIGHT:
			return gridPosition + Vector2i(offset, 1);
		case UP_LEFT:
			return gridPosition + Vector2i(offset-1, -1);
		case DOWN_LEFT:
			return gridPosition + Vector2i(offset-1, 1);
		case RIGHT:
			return gridPosition + Vector2i(1, 0);
		case LEFT:
			return gridPosition + Vector2i(-1, 0);
		}
		return gridPosition;
	}
};

class Belt : public Machine {
public:
	void process(double time) override {
		for(int d = 0; d < 6; d++)
			if(content[d] != NULL)
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