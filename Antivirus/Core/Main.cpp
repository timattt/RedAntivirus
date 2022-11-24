#include "Core.hpp"

int main(int argc, char * argv[]) {
	ENSURE(argc == 2, "bad args count");

	runObserver(argv[1]);
	return 0;
}
