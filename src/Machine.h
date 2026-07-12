#include "Skyrmion/tiling/MathIndexers.hpp"
#include "Skyrmion/util/AnimatedNode.hpp"
#include "Skyrmion/util/VertexGraph.hpp"
#include "Skyrmion/util/ListFile.hpp"

#include "item.h"

NodeIndexer *machineLinkIndexes = NULL;
GridMaker *machineTypeIndexes = NULL;
Indexer *colorIndexes = NULL;
ListFile *scoreIndexes = NULL;
bool saveMachines = false;

#define MACHINE_DIRECTIONS_FOREACH(E) \
	E(RIGHT) \
	E(DOWN_RIGHT) \
	E(DOWN_LEFT) \
	E(LEFT) \
	E(UP_LEFT) \
	E(UP_RIGHT) \

NAMED_ENUM(MACHINE_DIRECTIONS);

class Machine : public Node, public Vertex<6> {
	Vector2i gridPosition;

	int lastInput = 0;
	int indexUpdate = 0;

public:
	Item* content[6] { NULL };

	int rotation = 0;
	int maxItems = 1;
	int currentItems = 0;
	int arrivedItems = 0;
	float beltSpeed = 0.75;

	Machine() : Node(MACHINENODES, RENDER_COLOR_SINGLE), Vertex(NULL) {
		setColor(COLOR_PURPLE);
		//setHidden();
	}

	~Machine() {
		for(int d = 0; d < 6; d++)
			if(content[d] != NULL)
				content[d]->setDelete();

		if(machineLinkIndexes->getNode(gridPosition) == this) {
			machineLinkIndexes->setNode(gridPosition, NULL);
			machineTypeIndexes->setTile(gridPosition, ' ');
		}
	}

	void setGridPosition(int x, int y, int _rotation) {
		gridPosition = Vector2i(x, y);
		rotation = _rotation;

		machineLinkIndexes->setNode(gridPosition, this);
		setPosition(gridPosition * Vector2i(48, 43) + Vector2i(0, 14));
		if(y%2==0)
			setPosition(getPosition()+Vector2f(24, 0));

		machineTypeIndexes->setTile(gridPosition, ' ' + getType() + rotation*MAX_MACHINE);
		if(saveMachines)
			machineTypeIndexes->save("save/machine_grid.txt");
	}

	void update(double time) {
		for(int d1 = lastInput+1; d1<=lastInput+6; d1++) {
			int d = d1 % 6;

			//Process inputs
			if(content[d] != NULL && canAttach(d, false)) {
				if(content[d]->getPosition() != getPosition()) {
					content[d]->travelProgress += beltSpeed * time;
					content[d]->setPosition(lerp(content[d]->from, getPosition(), content[d]->travelProgress));
				}
				if(content[d]->getPosition() == getPosition() && content[d]->travelProgress >= 1) {
					arrivedItems++;
					content[d]->travelProgress = 0;
				}
			}

			//Attach to output
			if(!hasEdge(d) && canAttach(d, true) && machineLinkIndexes != NULL) {
				Machine *other = (Machine*)machineLinkIndexes->getNode(dirPos(d));
				if(other != NULL && !other->isDeleted() && other->canAttach(reverseDir(d), false)) {
					setVertex(d, reverseDir(d), other);
					//std::cout << "Connected machine " << getId() << " to " << other << " with " << d << "\n";
				}
			}

			//Pull item from other output
			if(hasEdge(d) && canInput(d)) {
				Machine *other = ((Machine*)getVertex(d));
				if(other->canOutput(reverseDir(d)))
					input(d, other->output(reverseDir(d)));
			}
		}

		if(arrivedItems == maxItems || arrivedItems > 0)
			process(time);
	}

	void input(int dir, Item *item) {
		content[dir] = item;
		currentItems++;
		lastInput = dir;
	}

	Item *output(int dir) {
		Item *item = content[dir];
		content[dir] = NULL;
		item->from = item->getPosition();
		item->travelProgress = 0;
		currentItems--;
		if(currentItems < 0)
			currentItems = 0;
		onOutput(dir);
		return item;
	}

	Item *prepareOutput(int from, int to) {
		if(content[to] == NULL) {
			content[to] = content[from];
			content[from] = NULL;
			arrivedItems--;
			if(arrivedItems < 0)
				arrivedItems = 0;
			return content[to];
		}
		return NULL;
	}

	void deleteItem(int d) {
		if(content[d] != NULL) {
			content[d]->setDelete();
			content[d] = NULL;
			currentItems--;
			arrivedItems--;
		}
	}

	bool canInput(int dir) {
		return canAttach(dir, false) && content[dir] == NULL && currentItems < maxItems;
	}

	bool canOutput(int dir) {
		return canAttach(dir, true) && content[dir] != NULL;
	}

	virtual int getType() { return MACHINE_EMPTY; }
	virtual void process(double time) {}
	virtual void onOutput(int dir) {}
	virtual bool canAttach(int dir, bool output) { return false; }

	int reverseDir(int dir) override {
		return (dir + 3) % 6;
	}

	Vector2i dirPos(int dir) {
		int offset = (gridPosition.y%2==0) ? 1 : 0;
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