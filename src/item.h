#include "Skyrmion/core/Node.h"

#include "indexes.h"

class Item : public Node {
public:
	int colors[12] = {0};

	Vector2i from;
	float travelProgress = 0;

	Item() : Node(ITEMS, RENDER_COLOR_TEXTURE_ARRAY, Vector2i(32,32)) {
		setTexture(TEXTURE_TRIANGLES);
		getRenderComponent()->getTextureRects()->reserve(12);
		getRenderComponent()->getColors()->reserve(12);

		for(int i = 0; i < 6; i++) {
			setTextureVecRect(32*(i+1), 0, i);
			setTextureVecRect(0, 0, i+6);
			getRenderComponent()->setColor(COLOR_EMPTY, i);
			getRenderComponent()->setColor(COLOR_EMPTY, i);
		}
	}

	void setColors(int _colors[12]) {
		for(int i = 0; i < 12; i++)
			colors[i] = _colors[i];
		updateRects();
	}

	void updateRects() {
		for(int i = 0; i < 12; i++)
			getRenderComponent()->setColor(ITEM_COLORS[colors[i]], i);
	}

	bool rotate(int count) {
		if(count < 0)
			count += 12;
		count %= 6;
		if(count <= 0)
			return false;

		int c1 = colors[0];
		int c2 = colors[6];
		for(int i = 0; i < 6; i++) {
			colors[i] = colors[(6-count+i)%6];
			colors[i+6] = colors[(6-count+i)%6+6];
		}
		colors[6-count] = c1;
		colors[12-count] = c2;
		updateRects();
		return true;
	}

	bool merge(Item *other) {
		for(int i = 0; i < 12; i++) {
			if((colors[i]!=0) && (other->colors[i]!=0))
				return false;
		}

		for(int i = 0; i < 12; i++) {
			if(colors[i] == 0)
				colors[i] = other->colors[i];
		}
		other->setHidden();
		updateRects();
		return true;
	}
};