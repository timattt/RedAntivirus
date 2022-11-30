#include "../Core/Core.hpp"

// local functions
//====================================================================================
int lockfile(int fd) {
        struct flock fl = {0};
        fl.l_type = F_WRLCK;
        fl.l_start = 0;
        fl.l_whence = SEEK_SET;
        fl.l_len = 0;
        return(fcntl(fd, F_SETLK, &fl));
}

int create_pid_file() {
        char buf[16] = {0};
        int fd = 0;
        fd = open(PID_FILE_PATH, O_RDWR|O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd == -1) {
                syslog(LOG_ERR, "open failed: %s", strerror(errno));
                return errno;
        }
        if (lockfile(fd) < -1) {
                syslog(LOG_ERR, "can't lock file: %s", strerror(errno));
                return errno;
        }
        ftruncate(fd, 0);
        sprintf(buf, "%ld", (long)getpid());
        write(fd, buf, strlen(buf)+1);
        return 0;
}
//====================================================================================


// global functions
//====================================================================================
int daemonStart() {
        pid_t pid = 0;
        int fd = 0;
        const long long TIMEOUT_MS = 60000 ; // 1 minute
        const long long RETRY_TIME_MS = 20;
        const long long N_RETRY = TIMEOUT_MS / RETRY_TIME_MS;
        long long i = 0;
        struct timespec req = {0};
        int fd0, fd1, fd2;

        /* Fork off the parent process */
        pid = fork();

        /* An error occurred */
        if (pid < 0) {
                syslog(LOG_ERR, "fork failed: %s", strerror(errno));
                return errno;
        }

        /* Success: Let the parent terminate */
        if (pid > 0)
                exit(0);

        /* On success: The child process becomes session leader */
        if (setsid() < 0){
                syslog(LOG_ERR, "setsid failed: %s", strerror(errno));
                return errno;
        }

        /* Save parent pid */
        pid_t ppid = getpid();

        /* Fork off for the second time*/
        pid = fork();

        /* An error occurred */
        if (pid < 0) {
                syslog(LOG_ERR, "fork failed: %s", strerror(errno));
                return errno;
        }

        /* Success: Let the parent terminate */
        if (pid > 0)
                  exit(0);

        /* Set new file permissions */
        umask(0);

        /* Change the working directory to the root directory */
        /* or another appropriated directory */
        chdir("/");

        /* Close all open file descriptors */
        for (fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--) {
                close(fd);
        }

        /* Reopen stdin (fd = 0), stdout (fd = 1), stderr (fd = 2) */
        fd0 = open("/dev/null", O_RDWR);
        /* 'fd0' should be 0 */
        if (fd0 != STDIN_FILENO) {
            syslog(LOG_ERR, "fd0(%d) is not equal STDIN_FILENO(%d)", fd0, STDIN_FILENO);
            return -1;
        }

        fd1 = dup(0);
        /* 'fd1' should be 1 */
        if (fd1 != STDOUT_FILENO) {
            syslog(LOG_ERR, "fd1(%d) is not equal STDOUT_FILENO(%d)", fd1, STDOUT_FILENO);
            return -1;
        }

        fd2 = dup(0);
        /* 'fd2' should be 2 */
        if (fd2 != STDERR_FILENO) {
            syslog(LOG_ERR, "fd2(%d) is not equal STDERR_FILENO(%d)", fd2, STDERR_FILENO);
            return -1;
        }

        /* Waitting 1 minute while parent does not exit,
        * because systemd using pid_file check who our parent.
        * And in same cases it stop our daemon using SIGKILL. */
        for (i = 0; i < N_RETRY && getppid() == ppid; ++i) {
                req.tv_sec = 0;
                req.tv_nsec = RETRY_TIME_MS * 1000 * 1000;
                nanosleep(&req, NULL);
        }


        /* systemctl will read this file with pid to check that daemon start */
        /* and stop it when it will be necessary */
        if (create_pid_file()) {
            syslog(LOG_ERR, "my_daemon already started");
            return -1;
        }
        return 0;
}
//====================================================================================
