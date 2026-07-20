#include "Skyrmion/input/MovementSystems.h"
#include "Skyrmion/input/InputHandler.h"
#include "Skyrmion/input/TouchscreenInput.hpp"
#include "Skyrmion/input/Settings.h"
#include "Skyrmion/util/AnimatedNode.hpp"

#include "indexes.h"
#include "InfoPage.hpp"

std::vector<std::string> zoomKeys = {
	"/zoom/in", "/zoom/out"
};

class PlayerBase : public Node {
private:
	FrameTimer timer;

public:
	int direction = 0;
	int variant = 0;

	PlayerBase(int layer, int _variant) : Node(layer, RENDER_TEXTURE_RECT, Vector2i(16,16)), timer(4, 0.2) {
		variant = _variant;

		setTexture(TEXTURE_PLAYER);
		setTextureVecRect(0,0);
		//setTextureVecRect(16 * direction, 24*(1+variant*4));
	}

	//Update animation frame
	void updateAnimation(double time, Vector2f movement) {
		if(movement != Vector2f(0,0)) {
			direction = topDownDirection(movement);
			int cDir = clockwiseDirection(direction);

			timer.next(time);
			if(timer.frame == 3)
				setTextureVecRect(16 * cDir, 24*(2+variant*4));
			else
				setTextureVecRect(16 * cDir, 24*(timer.frame+1+variant*4));
		} else {
			timer.frame = 1;
			timer.time = 0;
			setTextureVecRect(16 * clockwiseDirection(direction), 24*(2+variant*4));
		}
	}
};

class Player : public PlayerBase {
private:
	Indexer *collisionMap;

	DirectionHandler input;
	InputHandler zoomInput;
	TouchscreenJoystick joystick;

	Vector2f lastSent;
	int speed = 1;

public:
	Player(Indexer *_collisionMap) : PlayerBase(PLAYER, 1), collisionMap(_collisionMap), input("/movement", 0),
	 zoomInput(zoomKeys, 0), joystick(TEXTURE_JOYSTICK, "/movement/joystick", TOUCHSCREENINPUT) {
		//input.printKeys();

		speed = Settings::getInt("/playerSpeed", 100);
	}

	void update(double time) {
		Vector2f movement = input.getMovement(time * speed);
		setPosition(topDownMovement(this, movement, collisionMap));
		//setPosition(getPosition()+movement);

		//updateAnimation(time, movement);

		zoomInput.pressedFunc = [](int i) {
			Vector2i screenSize = UpdateList::getCameraRect().size();
			if(i == 0 && screenSize.x > 540)
				screenSize -= Vector2i(20,20);
			else if(i == 1)
				screenSize += Vector2i(20,20);
			UpdateList::setCamera(UpdateList::getNode(PLAYER), screenSize);
			UpdateList::getNode(PLAYERSCORE)->setPosition(screenSize/-2 + Vector2i(20,20));
			UpdateList::getNode(INFOBUTTON)->setPosition(screenSize.x/2-20, screenSize.y/-2+20);
		};
	}

	void recieveEvent(Event event) {

	}
};

void initializePlayer(Indexer *collisionMap) {
	//Vector2i screenSize = Settings::getVector("/screenSize", Vector2i(1080, 1080));
	Vector2i screenSize = Vector2i(540,540);

	//Player
	Player player(collisionMap);
	player.setPosition(collisionMap->getSize() * collisionMap->getScale() / 2);
	UpdateList::addNode(&player);
	UpdateList::setCamera(&player, screenSize);

	Node *scoreText = UpdateList::getNode(PLAYERSCORE);
	scoreText->setParent(&player);
	scoreText->setOrigin(0,0);
	scoreText->setPosition(screenSize/-2 + Vector2i(20,20));

	InfoPage infoPage(screenSize, &player);

	std::cout << "Added player\n";

	UpdateList::startEngine();
}