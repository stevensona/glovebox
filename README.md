# mission
small header-only c++ library for modelling complex relationships between values

## example usage

```c++
#include <iostream>
#include <math.h>
#include <chrono>
#include <thread>
#include <future>
#include "mission.h"

int main(int argc, char ** argv) {

	using namespace std;
	using namespace std::chrono_literals;
	using namespace mission;

	enum class params { tick, func };
	System<params, int> sys;

	sys.define(params::tick, [&sys] { return sys(params::tick) + 1; });
	sys.define(params::func, [&sys] { return sys(params::tick) * 10; });
	sys.setRoot(params::tick);
	sys.link(params::tick, params::func);
	
	auto start = chrono::steady_clock::now();
	
	sys.startCapture(10ms); //capture values every 10ms
	for (int i = 0; i < 100000; i++) {
		if (sys(params::tick) > 100) {
			sys.set(params::tick, 0); //demonstrates external manipulation of values
		}
		sys.update();
	}
	sys.stopCapture();

	chrono::duration<double, milli> elapsed = chrono::steady_clock::now() - start;
	cout << "finished in " << elapsed.count() << endl;

	sys.display();
	system("PAUSE");

}
```

