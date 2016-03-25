#include <iostream>
#include "glovebox.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace glovebox;

	Param<float> x, fx;

	x = Param<float>(0.0f, ALWAYS,
		[&](){ return x.back() + 1.f; }
	);

	fx = Param<float>(0.0f, ALWAYS,
		[&](){ return x.back() * x.back(); }
	);

	x.notify(&fx);

	for (int i = 0; i < 10; i++) {
		x.update();
		cout << "f(" << x.back() << ")= " << fx.back() << endl;
	}
	system("PAUSE");
}
