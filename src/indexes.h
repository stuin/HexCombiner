#pragma once

//#include "Skyrmion/input/MovementEnums.h"
#include "Skyrmion/tiling/GridMaker.h"

#define LAYER_FOREACH(E) \
    E(GROUND) \
    E(MACHINES) \
    E(ITEMS) \
    E(PLAYER) \
	E(TOUCHSCREENINPUT) \

NAMED_ENUM(LAYER);

#define TEXTURE_FOREACH(E, F) \
	E(TEXTURE_INVALID) \
	F(TEXTURE_PLAYER, "res/player.png") \
	F(TEXTURE_GROUND_TILES, "res/hextiles.png") \
	F(TEXTURE_TRIANGLES, "res/triangles.png") \
	F(TEXTURE_JOYSTICK, "res/debug/touchscreen_joystick.png") \

FILE_ENUM(TEXTURE);

static const std::map<int, std::string> tileNames = {
	{'~', "shallow water with sand"}
};

static const std::map<int, int> displayIndex = {
	{' ', -1},
	{'~', 0},
	{'w', 3},
	{'W', 3},
	{',', 0},
	{'.', 1},
	{'_', 2},
	{'-', 3},
	{'d', 4},
	{'f', 1},
	{'F', 2},
	{'t', 1},
	{'T', 2},
	{'p', 3}
};

//static const std::map<int, int> collisionIndex = {
//	{' ', FULL},
//	{'~', FULL},
//	{'w', FULL},
//	{'W', FULL},
//	{',', EMPTY},
//	{'.', EMPTY},
//	{'_', EMPTY},
//	{'-', EMPTY},
//	{'d', EMPTY},
//	{'t', FULL},
//	{'T', FULL},
//	{'p', FULL}
//};

void initializePlayer(Indexer *collisionMap);