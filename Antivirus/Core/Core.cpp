#include "Core.hpp"

// local functions
//=======================================
void getCommandFilePath(char * dest) {
	DIR * dir = opendir("/home");

	struct dirent * d = NULL;

	while ((d = readdir(dir)) != NULL) {
		if (strcmp(d->d_name, "..") && strcmp(d->d_name, ".")) {
			sprintf(dest, "/home/%s/Desktop/mayScan.txt", d->d_name);
			break;
		}
	}

	closedir(dir);
}

void scanForFriends(const char *workDir) {
	syslog(LOG_NOTICE, "[Starting scanning for friends]");
	stopKills();

	long long phaseStart = getCurrentMillis();
	while (getCurrentMillis() < phaseStart + OLD_LISTENING_TIME) {
		updateObserver();
	}

	parseFriends();
	allowKills();
}

bool parseCommands(const char * workDir) {
	char path[BUFSIZ] = {0};
	getCommandFilePath(path);

	int fd = open(path, O_RDONLY);

	if (fd < 0) {
		return 0;
	}

	char buf[4] = {0};
	int count = read(fd, buf, 4);
	close(fd);
	remove(path);

	if (count != 4) {
		return 0;
	}
	int val = *((int*)buf);
	syslog(LOG_NOTICE, "Received command code=%d", val);
	if (val == 123456) {
		scanForFriends(workDir);
	}
	if (val == 23456) {
		syslog(LOG_NOTICE, "[Clearing friends]");
		clearFriends();
	}
	if (val == 345678) {
		syslog(LOG_NOTICE, "[Reseting max actions to default value]");
		resetMaxActions();
	}
	return 0;
}
//=======================================

// global functions
//=======================================
void runObserver(const char *workDir) {
	allowKills();

	long long lastScan = getCurrentMillis();

	while (1) {
		updateObserver();
		tryToFlush();

		if (lastScan + SCAN_TIME < getCurrentMillis()) {
			parseCommands(workDir);
		}
	}
}
//=======================================
