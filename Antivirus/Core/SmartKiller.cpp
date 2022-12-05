#include "Core.hpp"

int get_process_parent_id(const pid_t pid) {
	char buffer[BUFSIZ];
	sprintf(buffer, "/proc/%d/stat", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
		fclose(fp);
		if (size > 0) {
			// See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
			strtok(buffer, " "); // (1) pid  %d
			strtok(NULL, " "); // (2) comm  %s
			strtok(NULL, " "); // (3) state  %c
			char * s_ppid = strtok(NULL, " "); // (4) ppid  %d
			return atoi(s_ppid);
		}
	}

	return -1;
}

int get_process_group_id(const pid_t pid) {
	char buffer[BUFSIZ];
	sprintf(buffer, "/proc/%d/stat", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
		fclose(fp);
		if (size > 0) {
			// See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
			strtok(buffer, " "); // (1) pid  %d
			strtok(NULL, " "); // (2) comm  %s
			strtok(NULL, " "); // (3) state  %c
			strtok(NULL, " "); // (4) ppid  %d
			char * gr = strtok(NULL, " ");
			return atoi(gr);
		}
	}

	return -1;
}

void orderKill(int target, std::set<int> & killedList) {
	DIR * dir = opendir("/proc");

	if (dir == NULL) {
		return;
	}

	struct dirent * d = NULL;

	errno = 0;
	while ((d = readdir(dir)) != NULL) {
		pid_t pid = atoi(d->d_name);

		if (pid == 0) {
			continue;
		}

		// kill children
		if (pid != target && (get_process_parent_id(pid) == target || get_process_group_id(pid) == target)) {
			orderKill(pid, killedList);
		}
	}

	closedir(dir);
	kill(target, SIGKILL);
	killedList.insert(target);
}
