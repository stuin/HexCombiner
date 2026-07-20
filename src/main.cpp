#include "Skyrmion/core/UpdateList.h"
#include "Skyrmion/input/Settings.h"
#include "Skyrmion/util/ListFile.hpp"

#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/tiling/NoiseIndexer.hpp"
#include "Skyrmion/tiling/MathIndexers.hpp"

#include "indexes.h"

void initialize() {
	//Load settings file
	//Settings::loadSettings("res/settings.json");

	Vector2i worldSize = Settings::getVector("/world/worldSize", Vector2i(100,100));
	int seed = 0;

	//Generate new base colors
	IO::createFolder("save");
	if(!IO::hasFile("save/color_grid.txt")) {
		int seed = Settings::getInt("/world/seed", 0);
		if(seed == 0)
			seed = time(NULL);

		noise::module::Perlin testNoise;
		testNoise.SetSeed(seed);
		testNoise.SetFrequency(Settings::getInt("/world/frequency", 1));
		NoiseGrid initGrid(&testNoise, worldSize, 6);
		initGrid.save("save/color_grid.txt", '0');
		IO::deleteFile("save/machine_grid.txt");
		IO::deleteFile("save/score_list.txt");
		ListFile list("save/score_list.txt", "0");
		list.setInt(1, seed, true);
		seed = list.getInt(1);
		//std::cout << seed << "\n";
	} else {
		ListFile list("save/score_list.txt", "0");
		seed = list.getInt(1);
		//std::cout << seed << "\n";
	}

	//Load base tile map
	GridMaker grid("save/color_grid.txt");
	Indexer *groundIndexer = new LinearIndexer(&grid, 1, 0-'1', 0);
	Indexer *collisionMap1 = new MapIndexer(groundIndexer, collisionIndex, 1, 48, 43);
	Indexer *collisionMap = new HexIndexer(collisionMap1, 1);
	//collisionMap1->printGrid();

	double holeChance = Settings::getInt("/world/holeChance") / 100.0f;
	groundIndexer->mapGrid([worldSize, groundIndexer, seed, holeChance](int c, Vector2f pos) {
		int x = pos.x;
		int y = pos.y;
		if(c == 4) {
			double input = RandomIndexer::IntegerNoise(x + y*worldSize.x + seed*worldSize.y*worldSize.x);
			if(input > holeChance)
				groundIndexer->setTileI(x, y, '0'+6);
		}
	});

	TileMap ground(TEXTURE_GROUND_TILES, 48, 57, groundIndexer, GROUND, 0, true);
	ground.setPosition(-24, 0);
	UpdateList::addNode(&ground);

	//Finish engine setup
	UpdateList::globalLayer(PLAYER);
	UpdateList::globalLayer(EDITOR);
	UpdateList::globalLayer(TOUCHSCREENINPUT);

	initializeMachines(groundIndexer);
	initializePlayer(collisionMap);
}

WindowConfig windowConfig() {
	if(!Settings::hasLoaded())
		Settings::loadSettings("res/settings.json");

	return {
		"Hex Combiner",
		Settings::getVector("/screenSize", Vector2i(1080, 1080)),
		skColor(0,0,0),
		TEXTURE_FILES,
		LAYER_NAMES
	};
};