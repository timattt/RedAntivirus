#include "../Core/Core.hpp"
#include <bits/stdc++.h>
// local variables
//========================================
std::map<int, int> frequency;
std::set<int> friends;
std::set<int> killed;
long long lastFlush = 0;
int maxAllowedAction = DEFAULT_MAX_ACTIONS;
char mayKill = 0;
//========================================


// local functions
//========================================
void setToLog(const char * mes, std::set<int> & st) {
	char buf[BUFSIZ] = {0};
	char * cur = buf;
	int i = 0;
	int sz = st.size();
	cur += sprintf(cur, "%s = [", mes);
	for (int pid : st) {
		cur += sprintf(cur, "%d", pid);
		if (i + 1 < sz) {
			cur += sprintf(cur, ", ");
		}
		i++;
	}
	cur += sprintf(cur, "]\n");

	syslog(LOG_CRIT, "%s", buf);
}

void addPid(int pid) {
	if (frequency.count(pid) == 0) {
		frequency[pid] = 1;
	} else {
		frequency[pid]++;
	}
}

void resetMaxActions() {
	maxAllowedAction = DEFAULT_MAX_ACTIONS;
}

void checkValid(int pid) {
	if (mayKill && frequency[pid] > maxAllowedAction && friends.count(pid) == 0 && pid != getpid()) {
		orderKill(pid, killed);
	}
}

void parseFriends() {
	friends.clear();
	maxAllowedAction = DEFAULT_MAX_ACTIONS;
	for (std::pair<int, int> pr : frequency) {
		int pid = pr.first;
		maxAllowedAction = std::max(maxAllowedAction, pr.second);
		friends.insert(pid);
	}

	syslog(LOG_CRIT, "New maximum operation count: %d", maxAllowedAction);
	setToLog("Friends", friends);

	frequency.clear();
	lastFlush = getCurrentMillis();
}

void clearFriends() {
	friends.clear();
}

void tryToFlush() {
	long long curTime = getCurrentMillis();
	if (curTime - lastFlush > FLUSH_TIME) {
		frequency.clear();
		lastFlush = curTime;

		int sz = killed.size();
		if (sz > 0) {
			setToLog("Killed", killed);
			killed.clear();
		}
	}
}
//========================================


// global functions
//========================================
void reportAction(int pid, int fd) {
	if (pid != getpid()) {
		addPid(pid);
	}
	checkValid(pid);
}
void allowKills() {
	mayKill = 1;
}
void stopKills() {
	mayKill = 0;
}
//========================================
