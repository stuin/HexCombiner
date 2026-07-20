#include "Skyrmion/tiling/GridMaker.h"
#include "Skyrmion/input/InputHandler.h"
#include "Skyrmion/core/UpdateList.h"

//#include "MachineTypes.hpp"

std::vector<std::string> placeKeys = {
	"/placement/place", "/placement/select", "/placement/clear",
	"/placement/next", "/placement/previous",
	"/placement/rotateL", "/placement/rotateR",
	"/placement/numbers/rotate1", "/placement/numbers/rotate2", "/placement/numbers/rotate3",
	"/placement/numbers/rotate4", "/placement/numbers/rotate5", "/placement/numbers/rotate6",
	"/placement/numbers/select1", "/placement/numbers/select2", "/placement/numbers/select3",
	"/placement/numbers/select4", "/placement/numbers/select5", "/placement/numbers/select6",
	"/placement/numbers/select7", "/placement/numbers/select8", "/placement/numbers/select9",
	"/placement/numbers/select0"
};

class MachineSelector : public Node {
private:
	Vector2i tileSize;
	Vector2f cursorOffset;
	Vector2i tilePos;

	Node *cursor;
	Node *cursorBelt;
	Node *cursorMachine;

	int current = 0;
	int offset = 0;
	int offsetMax = 6;
	int offsetMult = MAX_MACHINE;
	int offsetSub = ' ';

	GridMaker *grid;
	InputHandler input;

public:
	MachineSelector(GridMaker *_grid, Vector2i _tileSize, Vector2f _cursorOffset, Node *_cursor) :
		Node(EDITOR, RENDER_NONE, _tileSize*_grid->getSize()), grid(_grid), input(placeKeys, 1) {

		UpdateList::addListener(this, EVENT_TOUCH);
		UpdateList::addListener(this, EVENT_MOUSE);
		UpdateList::addListener(this, EVENT_SCROLL);
		setOrigin(0,0);

		tileSize = _tileSize;
		cursorOffset = _cursorOffset;
		cursor = _cursor;

		cursorBelt = new Node(EDITOR, RENDER_TEXTURE_RECT, Vector2i(48, 57), cursor);
		cursorBelt->setTexture(TEXTURE_BELT_TILES);
		cursorBelt->setColor(COLOR_HALF);
		cursorBelt->setOrigin(0,0);
		UpdateList::addNode(cursorBelt);

		cursorMachine = new Node(EDITOR, RENDER_TEXTURE_RECT, Vector2i(48, 57), cursor);
		cursorMachine->setTexture(TEXTURE_MACHINE_TILES);
		cursorMachine->setColor(COLOR_HALF);
		cursorMachine->setOrigin(0,0);
		UpdateList::addNode(cursorMachine);

		setCurrent(0, 0);

		MachineSelector *selector = this;
		input.pressedFunc = [selector](int i) { selector->pressedKey(i); };
		input.heldFunc = [selector](int i) { selector->heldKey(i); };
		input.printKeys();
	}

	void setCurrent(int _current, int _offset) {
		current = _current;
		offset = _offset;

		int beltC = beltFilter(current);
		if(beltC < 6)
			cursorBelt->setTextureVecRect(beltC*48, 0);
		else
			cursorBelt->setTextureVecRect((beltC%6)*48, 57);
		cursorBelt->getRenderComponent()->getTextureRect()->rotation = -60*offset;
		cursorBelt->setHidden(beltC == -1);

		int machineC = machineFilter(current);
		if(machineC < 6)
			cursorMachine->setTextureVecRect(machineC*48, 0);
		else
			cursorMachine->setTextureVecRect((machineC%6)*48, 57);
		//cursorMachine->getRenderComponent()->getTextureRect()->rotation = -60*offset;
		cursorMachine->setHidden(machineC == -1);

		cursor->getRenderComponent()->getTextureRect(1)->rotation = -60*offset;
	}

	void heldKey(int i) {
		if(i == 0) {
			int c = current + offset*offsetMult;
			setMachine(c+offsetSub, tilePos);
		}
	}

	void pressedKey(int i) {
		if(i == 1) {
			current = grid->getTileI(tilePos.x, tilePos.y) - offsetSub;
			if(offsetMax != 0) {
				offset = current / offsetMult;
				current = current % offsetMult;
			}
			setCurrent(current, offset);
		} else if(i == 2) {
			setCurrent(0, offset);
		} else if(i == 3) {
			int c = (current + 1 + SELECTABLE_MACHINES) % SELECTABLE_MACHINES;
			setCurrent(c, offset);
		} else if(i == 4) {
			int c = (current - 1 + SELECTABLE_MACHINES) % SELECTABLE_MACHINES;
			setCurrent(c, offset);
		} else if(i == 5) {
			int o = (offset - 1 + offsetMax) % offsetMax;
			setCurrent(current, o);
		} else if(i == 6) {
			int o = (offset + 1 + offsetMax) % offsetMax;
			setCurrent(current, o);
		} else if(i >= 7 && i < 13) {
			int o = i-7;
			setCurrent(current, o);
		} else if(i >= 13 && i < 23) {
			int c = (i-12) % SELECTABLE_MACHINES;
			setCurrent(c, offset);
		}
	}

	void recieveEvent(Event event) {
		if(event.type == EVENT_MOUSE || event.type == EVENT_TOUCH) {
			Vector2f pos = screenToGlobal(event.x, event.y) - cursorOffset;
			tilePos = pos / tileSize;
			if(tilePos.y%2==0) {
				if(((int)pos.x % tileSize.x) < tileSize.x/2)
					tilePos.x -= 1;
				pos = tilePos * tileSize;
				pos.x += tileSize.x/2;
			} else
				pos = tilePos * tileSize;
			cursor->setPosition(pos + cursorOffset);
			//createPixelRect({pos.x, pos.y, (float)tileSize.x, (float)tileSize.y}, Vector2i(0,0), 0);

			if(event.type == EVENT_TOUCH && event.down) {
				int c = current + offset*offsetMult;
				setMachine(c+offsetSub, tilePos);
			}
		}
	}
};