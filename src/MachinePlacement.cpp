#include "Skyrmion/tiling/MathIndexers.hpp"
#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/debug/GridEditor.hpp"

#include "MachineTypes.hpp"

void setMachine(int c, Vector2f pos) {
	if(machineLinkIndexes->getNode(pos) != NULL) {
		Machine *other = (Machine*)machineLinkIndexes->getNode(pos);
		if(c == ' ' || ((c - ' ')%MAX_MACHINE) != other->getType()) {
			other->setDelete();
		} else if((c - ' ')/MAX_MACHINE != other->rotation) {
			other->rotation = (c - ' ')/MAX_MACHINE;
		} else
			return;
	}

	if(c != ' ') {
		int type = (c - ' ')%MAX_MACHINE;
		int dir = (c - ' ')/MAX_MACHINE;
		if(type == MACHINE_BELT) {
			Belt *belt = new Belt();
			UpdateList::addNode(belt);
			belt->setGridPosition(pos.x, pos.y, dir);
		} else if(type == MACHINE_MINE) {
			int colors[12] = {0};
			colors[0] = 1;
			Mine *mine = new Mine(colors, 5);
			UpdateList::addNode(mine);
			mine->setGridPosition(pos.x, pos.y, dir);
		} else if(type == MACHINE_ROTATOR) {
			Rotator *rotate = new Rotator();
			UpdateList::addNode(rotate);
			rotate->setGridPosition(pos.x, pos.y, dir);
		}
	}
}

void initializeMachines() {
	machineTypeIndexes = new GridMaker("res/machine_grid.txt");

	GridMaker *machineLinkGrid = new GridMaker(machineTypeIndexes->getSize().x, machineTypeIndexes->getSize().y, 0);
	machineLinkIndexes = new NodeIndexer(machineLinkGrid, MACHINENODES);
	//std::cout << machineIndexes->getSize() << machineIndexes->getScale() << "\n";

	machineTypeIndexes->mapGrid([](int c, Vector2f pos) { setMachine(c, pos); });

	machineTypeIndexes->save("res/machine_grid.txt");

	std::function<int(int)> machineFilter = [](int c) {
		if((c%MAX_MACHINE)>MACHINE_BELT)
			return c;
		return -1;
	};
	std::function<int(int)> beltFilter = [](int c) {
		int dir = c/MAX_MACHINE*MAX_MACHINE;
		int type = c%MAX_MACHINE;
		if(type==MACHINE_BELT)
			return 1+dir;
		if(type==MACHINE_MINE)
			return 2+dir;
		if(type>=MACHINE_ROTATOR)
			return 1+dir;
		return -1;
	};

	LinearIndexer *spaceIndexer = new LinearIndexer(machineTypeIndexes, 1, 0-' ', 0);
	FuncIndexer *machineIndexer = new FuncIndexer(spaceIndexer, machineFilter, -1);
	TileMap *machineMap = new TileMap(TEXTURE_MACHINE_TILES, 48, 57, machineIndexer, MACHINETOPS, 0, true);
	machineMap->setPosition(24, 0);
	UpdateList::addNode(machineMap);

	FuncIndexer *beltIndexer = new FuncIndexer(spaceIndexer, beltFilter, -1);
	TileMap *beltMap = new TileMap(TEXTURE_BELT_TILES, 48, 57, beltIndexer, BELTS, 0, true);
	beltMap->setPosition(24, 0);
	UpdateList::addNode(beltMap);

	Node *cursor = new Node(EDITOR, RENDER_TEXTURE_SINGLE, Vector2i(48, 57));
	cursor->setOrigin(0,0);
	cursor->setTexture(TEXTURE_CURSOR);
	UpdateList::addNode(cursor);

	GridEditor *editor = new GridEditor(EDITOR, machineTypeIndexes, machineNames, Vector2i(48, 43), cursor, true);
	editor->setPosition(24, -14);
	editor->saveFile = "res/machine_grid.txt";
	editor->loadFile = "res/machine_grid.txt";
	editor->setFunction([](int c, Vector2f pos) { setMachine(c, pos); });
	editor->setupOffset(6, 6, ' ', MACHINE_DIRECTIONS_NAMES);
	UpdateList::addNode(editor);

	UpdateList::globalLayer(MACHINENODES);
	UpdateList::hideLayer(MACHINENODES);

	//saveMachines = true;
	//machineGrid.printGrid();
}