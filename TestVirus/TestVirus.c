#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <unistd.h>
#include <utime.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ENSURE_NONNULL(X) if ((X) == NULL) {printf("Something is null in %s at %d line.\n", __FILE__, __LINE__); exit(-1);}
#define ENSURE(X, ERRMES) if (!(X)) {printf("Something went wrong: [%s] in %s at %d line.\n", ERRMES, __FILE__, __LINE__); exit(-1);}

void parseDirectory(char * mainDirPath) {
	DIR * mainDir = NULL;
	struct dirent *entry = NULL;
	char absoluteEntryPath[PATH_MAX] = { 0 };
	DIR *subDir = NULL;
	FILE *file = NULL;
	int fileSize = 0;
	char *fileData = NULL;

	ENSURE_NONNULL(mainDir = opendir(mainDirPath));

	if (mainDir) {
		while ((entry = readdir(mainDir))) {
			subDir = NULL;
			memset(absoluteEntryPath, 0, PATH_MAX);
			if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
				continue;
			}

			snprintf(absoluteEntryPath, PATH_MAX, "%s/%s", mainDirPath, entry->d_name);

			// if it is directory
			if ((subDir = opendir(absoluteEntryPath))) {
				closedir(subDir);
				parseDirectory(absoluteEntryPath);
			} else if ((file = fopen(absoluteEntryPath, "r"))) {
				// find file size
				fseek(file, 0L, SEEK_END);
				fileSize = ftell(file);
				fseek(file, 0L, SEEK_SET);

				// read from file
				ENSURE_NONNULL(fileData = (char*) calloc(fileSize, sizeof(char)));
				ENSURE(fread(fileData, 1, fileSize, file) == fileSize, "Problem while reading from file");
				fclose(file);

				// write to file
				if ((file = fopen(absoluteEntryPath, "w"))) {
					for (int i = 0; i < fileSize; ++i) {
						fileData[i] = ~fileData[i];
					}
					ENSURE(fwrite(fileData, 1, fileSize, file) == fileSize, "Problem while writing to file");
					fclose(file);
				}

				free(fileData);
			}
		}
		closedir(mainDir);
	}
}

void fracSleep(float sec) {
    struct timespec ts;
    ts.tv_sec = (int) sec;
    ts.tv_nsec = (sec - ((int) sec)) * 1000000000;
    nanosleep(&ts,NULL);
}

int main(int argc, char *argv[]) {
	ENSURE(argc == 2, "Bad args count");

	while (1) {
		printf("Parsing [%s], My pid=[%d]\n", argv[1], getpid());
		parseDirectory(argv[1]);
		//fracSleep(0.02f);
	}
}
