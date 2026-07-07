#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/input/Settings.h"

#include "indexes.h"
#include "Machine.h"

void initialize() {
	//Load settings file
	Settings::loadSettings("res/settings.json");

	//Load base tile maps
	GridMaker grid("res/test_island.txt");
	MapIndexer collisionMap(&grid, displayIndex, 0, 16, 16);

	MapIndexer *groundIndexer = new MapIndexer(&grid, displayIndex, 0);
	TileMap ground(TEXTURE_GROUND_TILES, 48, 57, groundIndexer, GROUND, 0, true);
	UpdateList::addNode(&ground);

	if(false) {
		for(int i = 0; i < pow(6, MAX_COLOR); i++) {
			Item *test = new Item();
			int c = i;
			for(int s = 0; s<6; s++) {
				test->colors[s] = c%MAX_COLOR;
				c /= MAX_COLOR;
			}
			test->updateRects();
			test->setPosition(24+48*(i%32), 172+48*(i/32));
			UpdateList::addNode(test);
		}
	}

	GridMaker machineGrid(100, 100, 0);
	machineIndexes = new NodeIndexer(&machineGrid, MACHINES);
	//std::cout << machineIndexes->getSize() << machineIndexes->getScale() << "\n";

	if(true) {
		int colors[12] = {0};
		for(int i = 1; i < MAX_COLOR; i++) {
			colors[i] = i;
			Mine *mine = new Mine(colors, 5);
			UpdateList::addNode(mine);
			mine->setGridPosition(i*3,1, DOWN_RIGHT);

			for(int d = 0; d < 6; d++) {
				Belt *belt = new Belt();
				UpdateList::addNode(belt);
				belt->setGridPosition(i*3+1+d/2, 2+d, DOWN_RIGHT);
			}
		}
	}

	//machineGrid.printGrid();

	//Finish engine setup
	UpdateList::globalLayer(PLAYER);
	UpdateList::globalLayer(MACHINES);
	UpdateList::globalLayer(TOUCHSCREENINPUT);

	initializePlayer(&collisionMap);
}

WindowConfig windowConfig() {
	return {
		"Hex Combiner",
		Vector2i(1080, 1080),
		skColor(0,0,0),
		TEXTURE_FILES,
		LAYER_NAMES
	};
};