#include "Core.hpp"

int main(int argc, char * argv[]) {
	if (daemonStart()) {
    	return 1;
    }

	initObserver("/");
	runObserver("/");
	return 0;
}
