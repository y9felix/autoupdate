#define _GNU_SOURCE

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

static volatile sig_atomic_t running = 1;

static int
aconline(void)
{
	char c;
	int fd;
	ssize_t n;

	if ((fd = open(ACPATH, O_RDONLY | O_CLOEXEC)) < 0)
		return 0;
	while ((n = read(fd, &c, 1)) < 0 && errno == EINTR)
		continue;
	close(fd);

	return n == 1 && c == '1';
}

static int
lidclosed(void)
{
	char buf[32];
	int fd;
	ssize_t n;

	if ((fd = open(LIDPATH, O_RDONLY | O_CLOEXEC)) < 0)
		return 0;
	while ((n = read(fd, buf, sizeof(buf) - 1)) < 0 && errno == EINTR)
		continue;
	close(fd);
	if (n <= 0)
		return 0;
	buf[n] = '\0';

	return strstr(buf, "closed") ? 1 : 0;
}

static int
markupdated(void)
{
	int fd;

	if ((fd = open(STAMPPATH, O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0644)) < 0) {
		warn("%s", STAMPPATH);
		return -1;
	}
	close(fd);

	return 0;
}

static int
run(char *const argv[])
{
	int fd, status;
	pid_t pid;

	if ((pid = fork()) < 0) {
		warn("fork");
		return -1;
	}
	if (!pid) {
		if ((fd = open("/dev/null", O_RDONLY)) < 0)
			_exit(127);
		if (fd != STDIN_FILENO) {
			if (dup2(fd, STDIN_FILENO) < 0)
				_exit(127);
			close(fd);
		}
		execv(argv[0], argv);
		warn("%s", argv[0]);
		_exit(127);
	}
	while (waitpid(pid, &status, 0) < 0) {
		if (errno != EINTR) {
			warn("waitpid");
			return -1;
		}
	}
	if (!WIFEXITED(status) || WEXITSTATUS(status))
		return -1;

	return 0;
}

static void
sigterm(int sig)
{
	(void)sig;
	running = 0;
}

static int
updatedtoday(void)
{
	struct stat st;
	time_t d;

	if (stat(STAMPPATH, &st) < 0)
		return 0;
	d = time(NULL) - st.st_mtime;

	return d >= 0 && d < COOLDOWNSECS;
}

int
main(void)
{
	static char *const cmdreboot[] = CMDREBOOT;
	static char *const cmdupdate[] = CMDUPDATE;
	struct sigaction sa;
	struct timespec ts;
	time_t closedsince, retryafter;
	int needsreboot;

	memset(&sa, 0, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = sigterm;
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) < 0)
		err(1, "sigaction");
	if (sigaction(SIGTERM, &sa, NULL) < 0)
		err(1, "sigaction");

	closedsince = -1;
	needsreboot = 0;
	retryafter = 0;
	for (; running; sleep(SLEEPSECS)) {
		if (!aconline() || !lidclosed()) {
			closedsince = -1;
			continue;
		}
		if (clock_gettime(CLOCK_MONOTONIC, &ts) < 0)
			continue;
		if (closedsince < 0)
			closedsince = ts.tv_sec;
		if (ts.tv_sec - closedsince < IDLESECS)
			continue;
		if (needsreboot) {
			needsreboot = 0;
			run(cmdreboot);
			continue;
		}
		if (ts.tv_sec < retryafter || updatedtoday())
			continue;
		if (run(cmdupdate) < 0 || markupdated() < 0) {
			if (!running)
				break;
			if (clock_gettime(CLOCK_MONOTONIC, &ts) >= 0)
				retryafter = ts.tv_sec + RETRYSECS;
			continue;
		}
		if (!running)
			break;
		if (aconline() && lidclosed())
			run(cmdreboot);
		else
			needsreboot = 1;
		if (clock_gettime(CLOCK_MONOTONIC, &ts) >= 0)
			retryafter = ts.tv_sec + RETRYSECS;
	}

	return 0;
}
