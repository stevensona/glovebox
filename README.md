# glovebox
small header-only c++ library for modelling complex relationships between values

## example usage
the following simple example defines a system that calculates the function ```f(x) = x ^ 2``` between 0 and 10.
```c++
#include <iostream>
#include "glovebox.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace glovebox;

	Param<float> x, fx;

	x = Param<float>(0.0f, 10, ALWAYS, [&](){ return x() + 1.f; });
	fx = Param<float>(0.0f, 10, ALWAYS, [&](){ return x() * x(); });
	x.notify(&fx);

	for (int i = 0; i < 10; i++) {
		x.update();
		cout << "f(" << x() << ")= " << fx() << endl;
	}
	system("PAUSE");
}
```
The following is an example of the system wrapper for clearer grouping of related values:
```c++
#include <iostream>
#include <math.h>
#include "glovebox.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace glovebox;
	
	enum class params { tick, func };
	System<params, float> sys;

	sys.define(params::tick, [&sys]() { return sys(params::tick) + 1; });
	sys.define(params::func, [&sys]() { return sys(params::tick) * 3 + 1; });
	sys.setRoot(params::tick);
	sys.link(params::tick, params::func);

	for (int i = 0; i < 10; i++) {
		sys.update();
		cout << sys(params::tick) << ", " << sys(params::func) << endl;

	}
	system("PAUSE");

}
```

