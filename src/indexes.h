#pragma once

//#include "Skyrmion/input/MovementEnums.h"
#include "Skyrmion/tiling/GridMaker.h"

#define LAYER_FOREACH(E) \
    E(GROUND) \
    E(BELTS) \
    E(MACHINENODES) \
    E(ITEMS) \
    E(MACHINETOPS) \
    E(PLAYER) \
    E(EDITOR) \
	E(TOUCHSCREENINPUT) \

NAMED_ENUM(LAYER);

#define TEXTURE_FOREACH(E, F) \
	E(TEXTURE_INVALID) \
	F(TEXTURE_PLAYER, "res/player.png") \
	F(TEXTURE_CURSOR, "res/cursor.png") \
	F(TEXTURE_GROUND_TILES, "res/hextiles.png") \
	F(TEXTURE_BELT_TILES, "res/hexbelts.png") \
	F(TEXTURE_MACHINE_TILES, "res/hexfactories.png") \
	F(TEXTURE_TRIANGLES, "res/triangles.png") \
	F(TEXTURE_JOYSTICK, "res/debug/touchscreen_joystick.png") \

FILE_ENUM(TEXTURE);

static const skColor ITEM_COLORS[] = {
	COLOR_EMPTY,
	COLOR_WHITE,
	skColor(200,0,0),
	skColor(0,200,0),
	skColor(0,0,200)
};
#define MAX_COLOR 5

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
void initializeMachines();