// includes
//===========================================================
//#define _GNU_SOURCE     /* Needed to get O_LARGEFILE definition */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fanotify.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../Core/Core.hpp"
//===========================================================

// local variables
//===========================================================
int fd = 0;
struct pollfd fanPoll = {0};
const char * workingDir = 0;
//===========================================================


// local methods
//===========================================================
void handle_events(int fd, const char * dir) {
    const struct fanotify_event_metadata *metadata;
    struct fanotify_event_metadata buf[200];
    ssize_t len;
    struct fanotify_response response;

    /* Loop while events can be read from fanotify file descriptor. */
    for (;;)
    {
        /* Read some events. */
        len = read(fd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        /* Check if end of available data reached. */
        if (len <= 0)
            break;
        /* Point to the first event in the buffer. */
        metadata = buf;

        /* Loop over all events in the buffer. */
        /* FAN_EVENT_OK macro checks the remaining length len of the buffer
           meta against the length of the metadata structure and the
           event_len field of the first metadata structure in the
           buffer.*/
        while (FAN_EVENT_OK(metadata, len))
        {
            /* Check that run-time and compile-time structures match. */
            if (metadata->vers != FANOTIFY_METADATA_VERSION)
            {
                fprintf(stderr, "Mismatch of fanotify metadata version.\n");
                exit(EXIT_FAILURE);
            }
            /* metadata->fd contains either FAN_NOFD, indicating a
               queue overflow, or a file descriptor (a nonnegative
               integer). Here, we simply ignore queue overflow. */
            if (metadata->fd >= 0)
            {
                /* Handle open permission event. */
                if (metadata->mask & FAN_OPEN_PERM)
                {
                    //printf("FAN_OPEN_PERM: ");
                    reportAction(metadata->pid, metadata->fd);
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write(fd, &response, sizeof(response));
                }
		        if (metadata->mask & FAN_OPEN_EXEC_PERM)
                {
		        	reportAction(metadata->pid, metadata->fd);
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write(fd, &response, sizeof(response));
                }
                /* Handle closing of writable file event. */
                if (metadata->mask & FAN_CLOSE_WRITE)
                	reportAction(metadata->pid, metadata->fd);

                /* Handle closing of nowritable file event. */
                if (metadata->mask & FAN_CLOSE_NOWRITE)
                	reportAction(metadata->pid, metadata->fd);

                /* Retrieve and print pathname of the accessed file. */
                /*
                snprintf(procfd_path, sizeof(procfd_path),
                         "/proc/self/fd/%d", metadata->fd);
                path_len = readlink(procfd_path, path,
                                    sizeof(path) - 1);
                if (path_len == -1)
                {
                    perror("readlink");
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                printf("File %s", path);
		        printf(" PID %d", metadata->pid);
		        printf("\n");*/

                /* Close the file descriptor of the event. */
                close(metadata->fd);
            }
            /* Advance to next event. */
            /* FAN_EVENT_NEXT macro uses the length indicated in the event_len
               field of the fanotify_event_metadata structure pointed to by metadata to
               calculate the address of the next fanotify_event_metadata structure that
               follows meta. len is the number of bytes of fanotify_event_metadata that
               currently remain in the buffer. The macro returns a pointer to the next
               fanotify_event_metadata structure that follows metadata,
               and reduces len by the number of bytes in the fanotify_event_metadata
               structure that has been skipped over(i.e., it subtracts
               metadata->event_len from len). */
            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }
}
//===========================================================


// global methods
//===========================================================
void initObserver(const char * dir) {
	workingDir = dir;

	ENSURE((fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT | FAN_NONBLOCK, O_RDONLY | O_LARGEFILE)) != -1, "fanotify init problems");
	ENSURE(fanotify_mark(fd, FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_OPEN_PERM | FAN_OPEN_EXEC_PERM | FAN_CLOSE_WRITE | FAN_CLOSE_NOWRITE, AT_FDCWD, dir) != -1, "marking root dir");

	fanPoll.fd = fd;
	fanPoll.events = POLLIN;
}

void updateObserver() {
	int poll_num = 0;

	poll_num = poll(&fanPoll, 1, 1);
	if (poll_num == -1) {
		if (errno == EINTR) {
			return;
		}

		ENSURE(0, "poll error");
	}

	if (poll_num > 0) {
		if (fanPoll.revents & POLLIN) {
			handle_events(fd, workingDir);
		}
	}
}
//===========================================================
