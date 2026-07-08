#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/input/Settings.h"

#include "indexes.h"

void initialize() {
	//Load settings file
	Settings::loadSettings("res/settings.json");

	//Load base tile maps
	GridMaker grid("res/test_island.txt");
	MapIndexer collisionMap(&grid, displayIndex, 0, 16, 16);

	MapIndexer *groundIndexer = new MapIndexer(&grid, displayIndex, 0);
	TileMap ground(TEXTURE_GROUND_TILES, 48, 57, groundIndexer, GROUND, 0, true);
	UpdateList::addNode(&ground);

	//Finish engine setup
	UpdateList::globalLayer(PLAYER);
	UpdateList::globalLayer(TOUCHSCREENINPUT);

	initializeMachines();
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