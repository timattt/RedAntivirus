#include "../Core/Core.hpp"

// local variables
//========================================
std::map<int, int> frequency;
std::set<int> olds;
std::map<int, int> killed;
long long lastFlush = 0;
int maxAllowedAction = 100;
char mayKill = 0;
//========================================


// local functions
//========================================
void drawHistogram() {
	int max = 0;
	for (std::pair<int, int> pr : frequency) {
		int count = pr.second;
		max = std::max(max, count);
	}

	// olds
	printf("OLDS:\n");

	// border
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n");

	unsigned j = 0;
	for (int pid : olds) {
		printf("%d", pid);
		if (j + 1 < olds.size()) {
			printf(", ");
		}
		j++;
	}
	printf("\n");

	// border
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n\n");

	// active
	printf("ACTIVE:\n");

	// borders
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n");

	for (std::pair<int, int> pr : frequency) {
		int pid = pr.first;
		int count = pr.second;

		// name
		char buf[200] = {0};
		get_program_name_from_pid(pid, buf, 200);
		if (olds.count(pid) == 0) {
			printf(RED "|%s", buf);
		} else {
			printf("|%s", buf);
		}

		// indentation
		for (int i = 0; i+strlen(buf) < 40;i++) {
			printf(" ");
		}

		// pid
		printf("|pid=%d", pid);

		// indentation
		for (int i = 0; i < 7 - std::log10(pid); i++) {
			printf(" ");
		}

		// count
		printf("|count=%d", count);

		// indentation
		for (int i = 0; i < 5 - std::log10(count); i++) {
			printf(" ");
		}
		printf("|");

		// percents
		for (int i = 0; i < count * 20 / max; i++) {
			printf("#");
		}

		// new line and color flush
		if (olds.count(pid) == 0) {
			printf("\n" RESET);
		} else {
			printf("\n");
		}
	}

	// bottom border
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n");

	// max allowed actions
	printf("[Max allowedActions: %d]\n", maxAllowedAction);

	// killed
	printf("\nKILLED:\n");

	// other border
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n");

	for (auto pr : killed) {
		int pid = pr.first;
		int count = pr.second;
		printf("pid=%d, actions=%d\n", pid, count);
	}

	// last border
	for (int i = 0; i < 88; i++) {
		printf("=");
	}
	printf("\n");

	fflush(stdout);
}

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
		kill(pid, SIGKILL);
	}
}

void parseOlds() {
	for (std::pair<int, int> pr : frequency) {
		int pid = pr.first;
		maxAllowedAction = std::max(maxAllowedAction, pr.second);
		olds.insert(pid);
	}
	frequency.clear();
	lastFlush = getCurrentMillis();
}

void tryToPrintInfo() {
	long long curTime = getCurrentMillis();
	if (curTime - lastFlush > FLUSH_TIME) {
		printf("\033[H\033[J");
		drawHistogram();
		frequency.clear();
		lastFlush = curTime;
	}
}

//========================================


// global functions
//========================================
void reportAction(int pid, int fd) {
	addPid(pid);
	checkValid(pid);
}
void allowKills() {
	mayKill = 1;
}
//========================================
