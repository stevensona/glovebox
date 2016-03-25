#include <iostream>
#include "glovebox.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace glovebox;

	Param<float> x, fx;

	x = Param<float>(0.0f, 10, ALWAYS,
		[&](){ return x() + 1.f; }
	);

	fx = Param<float>(0.0f, 10, ALWAYS,
		[&](){ return x() * x(); }
	);

	x.notify(&fx);

	for (int i = 0; i < 10; i++) {
		x.update();
		cout << "f(" << x() << ")= " << fx() << endl;
	}
	system("PAUSE");
}
