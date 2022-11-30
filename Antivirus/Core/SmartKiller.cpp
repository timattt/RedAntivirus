#include "Core.hpp"

int get_process_parent_id(const pid_t pid) {
	char buffer[BUFSIZ];
	sprintf(buffer, "/proc/%d/stat", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
		if (size > 0) {
			// See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
			strtok(buffer, " "); // (1) pid  %d
			strtok(NULL, " "); // (2) comm  %s
			strtok(NULL, " "); // (3) state  %c
			char * s_ppid = strtok(NULL, " "); // (4) ppid  %d
			return atoi(s_ppid);
		}
		fclose(fp);
	}

	return -1;
}

void orderKill(int target, bool child) {
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
		if (get_process_parent_id(pid) == target) {
			orderKill(pid);
		}
	}

	closedir(dir);
	kill(target, SIGKILL);
	if (child) {
		syslog(LOG_CRIT | LOG_USER, "Дети врагов ОС подлежат ликвидации в обязательном порядке. pid=%d", target);
	} else {
		syslog(LOG_CRIT | LOG_USER, "Пустили в расход врага ОС. pid=%d", target);
	}
}
