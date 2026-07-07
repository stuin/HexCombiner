#include "Skyrmion/input/MovementSystems.h"
#include "Skyrmion/input/InputHandler.h"
#include "Skyrmion/input/TouchscreenInput.hpp"
#include "Skyrmion/input/Settings.h"
#include "Skyrmion/util/AnimatedNode.hpp"

#include "indexes.h"

class PlayerBase : public Node {
private:
	FrameTimer timer;

public:
	int direction = 0;
	int variant = 0;

	PlayerBase(int layer, int _variant) : Node(layer, RENDER_TEXTURE_RECT, Vector2i(16,24)), timer(4, 0.2) {
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
	TouchscreenJoystick joystick;

	Vector2f lastSent;

public:
	Player(Indexer *_collisionMap) : PlayerBase(PLAYER, 1), collisionMap(_collisionMap), input("/movement", PLAYER), joystick(TEXTURE_JOYSTICK, "/movement/joystick", TOUCHSCREENINPUT) {

	}

	void update(double time) {
		Vector2f movement = input.getMovement(time * 60);
		//setPosition(topDownMovement(this, movement, collisionMap));
		setPosition(getPosition()+movement);

		//updateAnimation(time, movement);
	}

	void recieveEvent(Event event) {

	}
};

void initializePlayer(Indexer *collisionMap) {
	//Player
	Player player(collisionMap);
	//player.setPosition(Vector2f(16*16, 16*16));
	UpdateList::addNode(&player);
	UpdateList::setCamera(&player, Vector2f(450, 450));

	std::cout << "Added player\n";

	UpdateList::startEngine();
}