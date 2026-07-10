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
	skColor(0,0,200),
	skColor(100,100,100),
	COLOR_BLACK,
	COLOR_EMPTY
};
#define MAX_COLOR 5

void initializePlayer(Indexer *collisionMap);
void initializeMachines(Indexer *colorMap);