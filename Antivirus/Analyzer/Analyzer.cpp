#include "../Core/Core.hpp"

// local variables
//========================================
std::map<int, int> frequency;
std::set<int> olds;
std::map<int, int> killed;
long long lastFlush = 0;
int maxAllowedAction = 1000;
char mayKill = 0;
//========================================


// local functions
//========================================
void addPid(int pid) {
	if (frequency.count(pid) == 0) {
		frequency[pid] = 1;
	} else {
		frequency[pid]++;
	}
}

void checkValid(int pid) {
	if (mayKill && frequency[pid] > maxAllowedAction && olds.count(pid) == 0 && pid != getpid()) {
		killed[pid] = frequency[pid];
		//orderKill(pid);
	}
}

void parseOlds() {
	olds.clear();
	for (std::pair<int, int> pr : frequency) {
		int pid = pr.first;
		maxAllowedAction = std::max(maxAllowedAction, pr.second);
		olds.insert(pid);
		//syslog(LOG_NOTICE | LOG_USER, "Found friend. pid=%d", pid);
	}

	frequency.clear();
	lastFlush = getCurrentMillis();
}

void tryToFlush() {
	long long curTime = getCurrentMillis();
	if (curTime - lastFlush > FLUSH_TIME) {
		printf("Total pids=%d\n", (int)frequency.size());fflush(stdout);
		//FILE * fl = fopen("~/Desktop/log.txt", "w");
		//char str[] = "go\n";
		//fwrite(str , 1 , sizeof(str) , fl );
		//fclose(fl);
		frequency.clear();
		lastFlush = curTime;
	}
}
//========================================


// global functions
//========================================
void reportAction(int pid, int fd) {
	if (pid != getpid()) {
		addPid(pid);
	}
	//checkValid(pid);
}
void allowKills() {
	mayKill = 1;
}
void stopKills() {
	mayKill = 0;
}
//========================================
