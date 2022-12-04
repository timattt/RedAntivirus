#include "Core.hpp"

// local functions
//=======================================
void scanForFriends(const char *workDir) {
	syslog(LOG_NOTICE | LOG_USER, "Starting scanning for friends");
	stopKills();

	// PHASE 1
	long long phaseStart = getCurrentMillis();
	initObserver(workDir);
	while (getCurrentMillis() < phaseStart + OLD_LISTENING_TIME) {
		updateObserver();
	}

	parseOlds();
	allowKills();
	syslog(LOG_NOTICE | LOG_USER, "Finished listening for friends");
}

bool mayScan() {
	int fd = open("~/Desktop/mayScan", O_RDONLY);

	if (fd < 0) {
		return 0;
	}
	char buf[4] = {0};
	int count = read(fd, buf, 4);
	if (count != 4) {
		close(fd);
		return 0;
	}
	int val = *((int*)buf);
	close(fd);
	remove("~/Desktop/mayScan");
	if (val == 123456) {
		remove("~/Desktop/mayScan");
		//syslog(LOG_NOTICE | LOG_USER, "Found scanning flag");
		return 1;
	}
	return 0;
}
//=======================================

// global functions
//=======================================
void runObserver(const char *workDir) {
	//syslog(LOG_NOTICE | LOG_USER, "Starting protection");

	while (1) {
		updateObserver();
		//if (mayScan()) {
		//	scanForFriends(workDir);
		//}
		tryToFlush();
	}
}
//=======================================
