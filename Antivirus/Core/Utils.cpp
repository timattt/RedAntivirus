#include "Core.hpp"

long long getCurrentMillis() {
    struct timespec spec;

    clock_gettime(CLOCK_REALTIME, &spec);
    return spec.tv_sec * 1000L + round(spec.tv_nsec / 1.0e6);
}

char* get_program_name_from_pid(int pid, char *buffer, size_t buffer_size) {
  int fd;
  ssize_t len;
  char *aux;

  /* Try to get program name by PID */
  sprintf (buffer, "/proc/%d/cmdline", pid);
  if ((fd = open (buffer, O_RDONLY)) < 0)
    return NULL;

  /* Read file contents into buffer */
  if ((len = read (fd, buffer, buffer_size - 1)) <= 0)
    {
      close (fd);
      return NULL;
    }
  close (fd);

  buffer[len] = '\0';
  aux = strstr (buffer, "^@");
  if (aux)
    *aux = '\0';

  return buffer;
}

void fansyPrint(const char * mes, int count) {
	for (int u = 0; u < count+1; u++) {
		printf("\033[H\033[J");
		printf("%s", mes);
		for (int k = 0; k < u; k++) {
			printf(".");
		}
		printf("\n");
		sleep(1);
	}
}

