#include "Skyrmion/tiling/TileMap.hpp"
#include "Skyrmion/tiling/NoiseIndexer.hpp"
#include "Skyrmion/debug/GridEditor.hpp"
#include "Skyrmion/input/Settings.h"

#include "MachineTypes.hpp"

void setMachine(int c, Vector2f pos) {
	//std::cout << "Placing " << c << " at " << pos << "\n";
	if(!machineTypeIndexes->inBounds(pos))
		return;
	if(machineTypeIndexes->getTile(pos)-' ' == MACHINE_HOLE)
		return;

	int type = (c - ' ')%MAX_MACHINE;
	int dir = (c - ' ')/MAX_MACHINE;
	if(machineLinkIndexes->getNode(pos) != NULL) {
		Machine *other = (Machine*)machineLinkIndexes->getNode(pos);
		if(type == MACHINE_EMPTY || type != other->getType()) {
			machineLinkIndexes->setNode(pos, NULL);
			machineTypeIndexes->setTile(pos, ' ');
			if(saveMachines)
				machineTypeIndexes->save("save/machine_grid.txt");
			other->setDelete();
		} else if(dir != other->rotation) {
			other->setDelete();
		} else
			return;
	}

	if(c != ' ') {
		int color = colorIndexes->getTile(pos)+1;
		if(color == 0 && type != MACHINE_BELT)
			return;

		Machine *machine = NULL;
		if(type == MACHINE_BELT)
			machine = new Belt(false);
		else if(type == MACHINE_MINE && ITEM_COLORS[color] != COLOR_EMPTY)
			machine = new Mine(color);
		else if(type == MACHINE_ROTATEL)
			machine = new Rotate(5);
		else if(type == MACHINE_ROTATER)
			machine = new Rotate(1);
		else if(type == MACHINE_MERGE)
			machine = new Merge();
		else if(type == MACHINE_DISTRIBUTE)
			machine = new Distribute();
		else if(type == MACHINE_CROSSOVER)
			machine = new Crossover();
		if(type == MACHINE_XBELT)
			machine = new Belt(true);

		if(machine != NULL) {
			UpdateList::addNode(machine);
			machine->setGridPosition(pos.x, pos.y, dir);
		}
	}
}

int machineFilter(int c) {
	int type = c%MAX_MACHINE;
	if(type>MACHINE_BELT && type<MACHINE_DISTRIBUTE)
		return c%MAX_MACHINE;
	return -1;
}
int beltFilter(int c) {
	int dir = c/MAX_MACHINE*MAX_MACHINE;
	int type = c%MAX_MACHINE;
	if(type==MACHINE_BELT)
		return 1+dir;
	if(type==MACHINE_MINE)
		return 2+dir;
	if(type==MACHINE_ROTATEL)
		return 1+dir;
	if(type==MACHINE_ROTATER)
		return 1+dir;
	if(type==MACHINE_MERGE)
		return 3+dir;
	if(type==MACHINE_DISTRIBUTE)
		return 6+dir;
	if(type==MACHINE_CROSSOVER)
		return 7;
	if(type==MACHINE_XBELT)
		return 5+dir;
	return -1;
}

#include "MachineSelector.hpp"

void initializeMachines(Indexer *colorMap) {
	Vector2i worldSize = Settings::getVector("/world/worldSize", Vector2i(100,100));
	colorIndexes = colorMap;

	//Read scores from file
	scoreIndexes = new ListFile("save/score_list.txt", "0");
	int seed = scoreIndexes->getInt(1);
	//scoreIndexes->save("save/score_list.txt");

	//Read machine locations from file
	if(IO::hasFile("save/machine_grid.txt"))
		machineTypeIndexes = new GridMaker("save/machine_grid.txt");
	else
		machineTypeIndexes = new GridMaker(worldSize.x, worldSize.y);

	//Store grid of machine ids
	GridMaker *machineLinkGrid = new GridMaker(machineTypeIndexes->getSize().x, machineTypeIndexes->getSize().y, 0);
	machineLinkIndexes = new NodeIndexer(machineLinkGrid, MACHINENODES);
	//std::cout << machineIndexes->getSize() << machineIndexes->getScale() << "\n";

	//Load machines from file
	machineTypeIndexes->mapGrid([](int c, Vector2f pos) { setMachine(c, pos); });
	machineTypeIndexes->save("save/machine_grid.txt");

	//Machine rendering index values
	//std::function<int(int)> machineFilter = [](int c) { return machineFilter(c); };
	//std::function<int(int)> beltFilter = [](int c) { return beltFilter(c); };

	//Render machines to grid
	LinearIndexer *spaceIndexer = new LinearIndexer(machineTypeIndexes, 1, 0-' ', 0);
	FuncIndexer *machineIndexer = new FuncIndexer(spaceIndexer, machineFilter, -1);
	TileMap *machineMap = new TileMap(TEXTURE_MACHINE_TILES, 48, 57, machineIndexer, MACHINETOPS, 0, true);
	machineMap->setPosition(-24, 0);
	UpdateList::addNode(machineMap);

	//Render belts to grid
	FuncIndexer *beltIndexer = new FuncIndexer(spaceIndexer, beltFilter, -1);
	TileMap *beltMap = new TileMap(TEXTURE_BELT_TILES, 48, 57, beltIndexer, BELTS, 0, true);
	beltMap->setPosition(-24, 0);
	UpdateList::addNode(beltMap);

	//Generate target holes from seed
	colorMap->mapGrid([worldSize, seed](int c, Vector2f pos) {
		int x = pos.x;
		int y = pos.y;
		int emptyChance = 2;
		if(c == 5) {
			double input = RandomIndexer::IntegerNoise(x + y*worldSize.x + seed*worldSize.y*worldSize.x);
			int permutation = pow(MAX_COLOR+emptyChance, 6) * input;
			int colors[12] = {0};
			for(int i = 0; i < 6; i++) {
				colors[i] = permutation % (MAX_COLOR+emptyChance)-emptyChance;
				if(colors[i] < 0)
					colors[i] = 0;
				permutation /= MAX_COLOR;
			}

			Hole *hole = new Hole(colors);
			UpdateList::addNode(hole);
			hole->setGridPosition(pos.x, pos.y, 0);
		}
	});

	//Cursor for placing machines
	Node *cursor = new Node(EDITOR, RENDER_TEXTURE_ARRAY, Vector2i(48, 57));
	cursor->setOrigin(0,0);
	cursor->setTexture(TEXTURE_CURSOR);
	cursor->setTextureVecRect(48,0, 1);
	cursor->setTextureVecRect(0,0, 0);

	//Tool for placing machines
	//GridEditor *editor = new GridEditor(EDITOR, machineTypeIndexes, machineNames, Vector2i(48, 43), cursor, true);
	//editor->setPosition(24, -14);
	//editor->saveFile = "save/machine_grid.txt";
	//editor->loadFile = "save/machine_grid.txt";
	//editor->setFunction([](int c, Vector2f pos) { setMachine(c, pos); });
	//editor->setupOffset(6, MAX_MACHINE, ' ', MACHINE_DIRECTIONS_NAMES);
	//UpdateList::addNode(editor);

	MachineSelector *editor = new MachineSelector(machineTypeIndexes, Vector2i(48, 43), Vector2f(-24, -14), cursor);
	UpdateList::addNode(editor);
	UpdateList::addNode(cursor);

	//Display player score
	Node *scoreText = new Node(PLAYERSCORE, RENDER_STRING);
	scoreText->setTexture(FONT_POINTS);
	scoreIndexes->linkNode(0, scoreText);
	UpdateList::addNode(scoreText);

	UpdateList::globalLayer(MACHINENODES);
	UpdateList::hideLayer(MACHINENODES);

	saveMachines = true;
	//machineGrid.printGrid();
}