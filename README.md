# glovebox
small header-only c++ library for modelling complex relationships between values

## example usage
the following example defines a system that calculates the function ```f(x) = x ^ 2``` between 0 and 10.
```c++
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
```
Because ```parameter``` is just a thin wrapper around ```std::vector``` and stores and permits access to all previous values, in addition to conditional and dependent update schemes, glovebox is highly expressive and flexible.
