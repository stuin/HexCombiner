#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/input/Settings.h"

#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/tiling/NoiseIndexer.hpp"
#include "Skyrmion/tiling/MathIndexers.hpp"

#include "indexes.h"

void initialize() {
	//Load settings file
	Settings::loadSettings("res/settings.json");

	Vector2i worldSize = Settings::getVector("/worldSize", Vector2i(100,100));

	if(!IO::hasFile("res/ground_colors.txt")) {
		noise::module::Perlin testNoise;
		NoiseGrid initGrid(&testNoise, worldSize, 6);
		initGrid.save("res/ground_colors.txt", '0');
		//IO::deleteFile("res/machine_grid.txt");
	}

	//Load base tile maps
	GridMaker grid("res/ground_colors.txt");
	Indexer *groundIndexer = new LinearIndexer(&grid, 1, 0-'1', 0);

	int seed = 1000;
	groundIndexer->mapGrid([worldSize, groundIndexer, seed](int c, Vector2f pos) {
		int x = pos.x;
		int y = pos.y;
		double input = RandomIndexer::IntegerNoise(x + y*worldSize.x + seed*worldSize.y*worldSize.x);
		if(c == 4 && input > 0.9)
			groundIndexer->setTileI(x, y, '0'+6);
	});

	TileMap ground(TEXTURE_GROUND_TILES, 48, 57, groundIndexer, GROUND, 0, true);
	ground.setPosition(-24, 0);
	UpdateList::addNode(&ground);

	//Finish engine setup
	UpdateList::globalLayer(PLAYER);
	UpdateList::globalLayer(TOUCHSCREENINPUT);

	initializeMachines(groundIndexer);
	initializePlayer(groundIndexer);
}

WindowConfig windowConfig() {
	if(!Settings::hasLoaded())
		Settings::loadSettings("res/settings.json");

	return {
		"Hex Combiner",
		Settings::getVector("/screen", Vector2i(1080, 1080)),
		skColor(0,0,0),
		TEXTURE_FILES,
		LAYER_NAMES
	};
};