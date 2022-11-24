#include "Core.hpp"

// global functions
//=======================================
void runObserver(const char *workDir) {
	fansyPrint("Starting listening to default processes. You have 15 seconds to do some default stuff", 3);

	// PHASE 1
	long long phaseStart = getCurrentMillis();
	initObserver(workDir);
	while (getCurrentMillis() < phaseStart + OLD_LISTENING_TIME) {
		updateObserver();
	}

	fansyPrint("Finished listening to default processes", 3);

	parseOlds();
	allowKills();

	// PHASE 2
	fansyPrint("Starting protection", 3);

	// phase 2 - protecting
	while (1) {
		updateObserver();
		tryToPrintInfo();
	}
}
//=======================================
