#include "Skyrmion/core/Node.h"

#include "indexes.h"

class Item : public Node {
public:
	int colors[12] = {0};
	float alpha = 1;

	Vector2i from;
	float travelProgress = 0;

	Item(int layer, float _alpha) : Node(layer, RENDER_COLOR_TEXTURE_ARRAY, Vector2i(32,32)) {
		setTexture(TEXTURE_TRIANGLES);
		getRenderComponent()->getTextureRects()->reserve(12);
		getRenderComponent()->getColors()->reserve(12);

		alpha = _alpha;

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
		for(int i = 0; i < 12; i++) {
			skColor color = ITEM_COLORS[colors[i]];
			color.alpha *= alpha;
			getRenderComponent()->setColor(color, i);
		}
	}

	bool rotate(int count) {
		if(count < 0)
			count += 12;
		count %= 6;
		if(count <= 0)
			return false;

		int colors2[12] = {0};
		for(int i = 0; i < 6; i++) {
			colors2[(i+count)%6] = colors[i];
			colors2[(i+count)%6+6] = colors[i+6];
		}
		setColors(colors2);
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
		other->setDelete();
		updateRects();
		return true;
	}

	bool compare(Item *other) {
		for(int i = 0; i < 12; i++) {
			if(colors[i] != other->colors[i])
				return false;
		}
		return true;
	}
};