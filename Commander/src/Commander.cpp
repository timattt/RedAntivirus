#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

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

int giveCommand(int type) {
	int fd = 0;

	char path[BUFSIZ] = {0};
	getCommandFilePath(path);

	if ((fd = open(path, O_RDWR|O_CREAT, 0777)) < 0 || write(fd, &type, sizeof(int)) < 0 || close(fd) < 0) {
		return -1;
	}

	return 0;
}

int main(int argc, char * argv[]) {
	if (argc != 2) {
		printf("Bad args count. Specify command type!\n");
		return 1;
	}

	int type = -1;

	if (!strcmp(argv[1], "friends")) {
		type = 123456;
	}
	if (!strcmp(argv[1], "clear")) {
		type = 23456;
	}
	if (!strcmp(argv[1], "reset")) {
		type = 345678;
	}

	if (type == -1) {
		printf("Bad command!\n");
		return 1;
	}

	if (giveCommand(type) < 0) {
		printf("Something went wrong with files!\n");
		perror("");
		return 1;
	}

	printf("Gived command with type=[%d]\n", type);

	return 0;
}
