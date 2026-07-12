#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/tiling/GridMaker.h"
#include "Skyrmion/util/ListFile.hpp"

int machineGrid[7] = {
	2,	//MINE
	-1,	//BELT
	4,	//ROTATE
	5,	//MERGE
	-1,	//DISTRIBUTE
	-1,	//CROSSOVER
	11	//HOLE
};
int beltGrid[7] = {
	2,
	1,
	1,
	3,
	6,
	7,
	-1
};

class InfoPage : public Node {
	Node button;

public:
	InfoPage(Vector2i screenSize, Node *parent) : Node(INFOPAGE, RENDER_COLOR_SINGLE, Vector2i(486,486), parent),
	button(INFOBUTTON, RENDER_TEXTURE_SINGLE, Vector2i(16,19), parent) {

		setColor(skColor(100, 100, 100, 200));
		//setHidden();
		//UpdateList::hideLayer(INFOPAGE);
		UpdateList::addNode(this);

		button.setPosition(screenSize.x/2-20, screenSize.y/-2+20);
		//button.setColor(skColor(0,0,200,100));
		button.setTexture(TEXTURE_INFO);
		button.setOrigin(8, 0);
		UpdateList::addNode(&button);

		GridMaker *belts = new GridMaker(1, 7);
		belts->setGrid((int*)beltGrid);
		TileMap *beltMap = new TileMap(TEXTURE_BELT_TILES, 48, 57, belts, INFOPAGE);
		beltMap->setParent(this);
		beltMap->setPosition(getSize()/-2.0f+Vector2f(20,20));
		UpdateList::addNode(beltMap);

		GridMaker *machines = new GridMaker(1, 7);
		machines->setGrid((int*)machineGrid);
		TileMap *machineMap = new TileMap(TEXTURE_MACHINE_TILES, 48, 57, machines, INFOPAGE);
		machineMap->setParent(this);
		machineMap->setPosition(getSize()/-2.0f+Vector2f(20,20));
		UpdateList::addNode(machineMap);

		ListFile *machineDesc = new ListFile("res/MachineDesc.txt", "");
		for(sint i = 0; i < machineDesc->size(); i += 2) {
			Node *titleText = new Node(INFOPAGE, RENDER_STRING, Vector2i(16,16), this);
			titleText->setPosition(getSize()/-2.0f+Vector2f(90,30+57*i/2));
			titleText->setTexture(FONT_LABEL);
			machineDesc->linkNode(i, titleText);
			UpdateList::addNode(titleText);

			std::string s = machineDesc->getLine(i+1);
			for(int x = 0; x < s.size()-1; x++)
				if(s[x] == '\\' && s[x+1] == 'n') {
					s[x] = ' ';
					s[x+1] = '\n';
				}
			machineDesc->setLine(i+1, s, false);

			Node *bodyText = new Node(INFOPAGE, RENDER_STRING, Vector2i(16,16), this);
			bodyText->setPosition(getSize()/-2.0f+Vector2f(110,50+57*i/2));
			bodyText->setTexture(FONT_INFO);
			bodyText->getRenderComponent()->setSize(16);
			machineDesc->linkNode(i+1, bodyText);
			UpdateList::addNode(bodyText);
		}

		UpdateList::addListener(this, EVENT_MOUSE);
		UpdateList::addListener(this, EVENT_TOUCH);
		UpdateList::addListener(this, EVENT_KEYPRESS);
	}

	void recieveEvent(Event event) {
		if(!isHidden() && event.down) {
			setHidden();
			UpdateList::hideLayer(INFOPAGE);
		} else if((event.type == EVENT_MOUSE || event.type == EVENT_TOUCH) && event.down) {
			Vector2f pos = screenToGlobal(event.x, event.y);
			if(button.getRect().contains(pos)) {
				setHidden(false);
				UpdateList::hideLayer(INFOPAGE, false);
			}
		}
	}
};