#pragma once

//#include "Skyrmion/input/MovementEnums.h"
#include "Skyrmion/tiling/GridMaker.h"

#define LAYER_FOREACH(E) \
    E(GROUND) \
    E(BELTS) \
    E(MACHINENODES) \
    E(HOLEITEMS) \
    E(ITEMS) \
    E(MACHINETOPS) \
    E(SCORES) \
    E(PLAYER) \
    E(PLAYERSCORE) \
    E(EDITOR) \
	E(TOUCHSCREENINPUT) \
	E(INFOBUTTON) \
	E(INFOPAGE) \

NAMED_ENUM(LAYER);

#define TEXTURE_FOREACH(E, F) \
	E(TEXTURE_INVALID) \
	F(TEXTURE_PLAYER, "res/player.png") \
	F(TEXTURE_CURSOR, "res/cursor.png") \
	F(TEXTURE_INFO, "res/infobutton.png") \
	F(TEXTURE_GROUND_TILES, "res/hextiles.png") \
	F(TEXTURE_BELT_TILES, "res/hexbelts.png") \
	F(TEXTURE_MACHINE_TILES, "res/hexfactories.png") \
	F(TEXTURE_TRIANGLES, "res/triangles.png") \
	F(TEXTURE_JOYSTICK, "res/debug/touchscreen_joystick.png") \
	F(FONT_POINTS, "res/small_pixel.ttf") \
	F(FONT_LABEL, "res/RobotoCondensed-Bold.ttf") \
	F(FONT_INFO, "res/RobotoCondensed-Regular.ttf") \

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

enum MACHINE_TYPE {
	MACHINE_EMPTY,
	MACHINE_BELT,
	MACHINE_MINE,
	MACHINE_ROTATEL,
	MACHINE_ROTATER,
	MACHINE_MERGE,
	MACHINE_DISTRIBUTE,
	MACHINE_CROSSOVER,
	MACHINE_XBELT,
	MACHINE_HOLE
};
#define MAX_MACHINE 12
#define SELECTABLE_MACHINES 9

static const std::map<int, std::string> machineNames = {
	{' '+MACHINE_EMPTY, "Empty"},
	{' '+MACHINE_BELT, "Belt"},
	{' '+MACHINE_MINE, "Mine"},
	{' '+MACHINE_ROTATEL, "Rotate Left"},
	{' '+MACHINE_ROTATER, "Rotate Right"},
	{' '+MACHINE_MERGE, "Merge"},
	{' '+MACHINE_DISTRIBUTE, "Distribute"},
	{' '+MACHINE_CROSSOVER, "Crossover"},
};

static const std::map<int, int> collisionIndex = {
	{-1, 0},
	{0, 0},
	{1, 0},
	{2, 0},
	{3, 0},
	{4, 0},
	{5, 1},
	{6, 1},
	{7, 1}
};

void initializePlayer(Indexer *collisionMap);
void initializeMachines(Indexer *colorMap);