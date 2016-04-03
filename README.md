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
###sample output
```
finished in 1032.41
78 39 87 20 40 52 30 26 41 35 19 31 84 50 59 95 97 50 19 89 55 24 93 63 17 88 55 25 93 60 23 84 25 93 62 9 79 46 14 82 45 15 83 50 17 88 53 23 90 49 16 73 38 69 26 89 50 7 75 40 4 70 37 5
800 400 890 210 410 530 310 270 430 360 200 320 850 510 610 960 970 510 200 890 560 250 940 630 180 880 570 250 930 600 250 850 270 940 630 100 790 470 150 820 460 160 840 510 180 890 540 230 900 500 170 740 390 700 270 890 510 80 770 410 50 710 380 50
Press any key to continue . . .
```
Note that the second line of output (corresponding to ```func``` parameter (x * 10) does not always appear to actually be x * 10 due to multithreaded execution.
