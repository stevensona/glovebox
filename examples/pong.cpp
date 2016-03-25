#include <iostream>
#include "glovebox.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace glovebox;

	struct vec2 {
		float x;
		float y;
		vec2(float x, float y) :x(x), y(y) {}
	};

	Param<vec2> pos, speed;

	pos = Param<vec2>(vec2(0, 0), ALWAYS, [&]() {
		return vec2(pos().x + speed().x, pos().y + speed().y);
	});

	speed = Param<vec2>(vec2(-1.f, 0.75f), ALWAYS, [&]() {
		vec2 r(speed().x, speed().y);
		if (pos().x < -4 || pos().x > 4)
			r.x *= -1;
		if (pos().y < -4 || pos().y > 4)
			r.y *= -1;
		return r;
	});

	speed.notify(&pos);

	for (int i = 0; i < 10; i++) {
		
		speed.update();
		cout << pos().x << ", " << pos().y << endl;
	}
	system("PAUSE");
}
