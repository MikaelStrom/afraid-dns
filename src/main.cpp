/*
 * main.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>

#include "Util.h"
#include "AfraidDns.h"
#include "Config.h"

using namespace std;

//-----------------------------------------------------------------------------

#define PID_FILE	"/var/run/afraid-dns.pid"

int running = 1;
int lock_fd = -1;

//------------------------------------------------------------------------------

void signal_handler(int sig)
{
	switch (sig)
	{
	case SIGHUP:
//		reload = 1;
		break;
	case SIGINT:
	case SIGTERM:
		running = 0;
		break;
	}
}

//-----------------------------------------------------------------------------

void daemonize()
{
	if(getppid() == 1)
	{
		return; // already a daemon
	}

	// fork of new process
	pid_t pid = fork();

	if(pid < 0)
	{
		Util::Log(LogError, "fork() failed: " + string(strerror(errno)));
		exit(1); 		// fork error
	}

	if(pid > 0)
	{
		exit(0);		// parent exits
	}

	// child (daemon) continues

	setsid(); 			// create a new session

	umask(022); 		// set createfile permissions

	// close I/O
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	// reopen I/O to /dev/null
	if(open("/dev/null", O_RDONLY) == -1)
	{
		Util::Log(LogError, "open(stdin) failed: " + string(strerror(errno)));
	}
	if(open("/dev/null", O_WRONLY) == -1)
	{
		Util::Log(LogError, "open(stdout) failed: " + string(strerror(errno)));
	}
	if(open("/dev/null", O_RDWR) == -1)
	{
		Util::Log(LogError, "open(stderr) failed: " + string(strerror(errno)));
	}

	// set sensible working directory
	chdir("/");

	// drop root privs (to user daemon), if runnning as root
//	if(getuid() == 0)
//	{
//	    if(setgid(2) != 0)
//	    {
//	    	Util::Log(LogError, "setgid(2) failed: " + string(strerror(errno)));
//	    }
//	    if(setuid(2) != 0)
//	    {
//	    	Util::Log(LogError, "setuid(2) failed: " + string(strerror(errno)));
//	    }
//	}

	// create lockfile
	int lock_fd = open(PID_FILE, O_RDWR | O_CREAT, 0640);

	if(lock_fd < 0)
	{
		Util::Log(LogError, "open(PID_FILE) failed: " + string(strerror(errno)) + ". Probably another instance is already running. Exiting.");
		exit(1); 		// open failed, we're a duplicate
	}

	if(lockf(lock_fd, F_TLOCK, 0) < 0)
	{
		Util::Log(LogError, "lockf(lock_fd) failed: " + string(strerror(errno)) + ". Exiting.");
		exit(1); 		// lock failed - no idea what this means...
	}

	// write pid to file, and leave file open (blocking duplicates)

	char str[32];
	sprintf(str, "%d\n", getpid());
	write(lock_fd, str, strlen(str));
}

//-----------------------------------------------------------------------------

void usage()
{
	printf("usage: afraid-dns [-f]\n");
	printf("  -f	run in foregound, log to stdout\n");
	exit(-1);
}

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	string username;
	string password;
	string hostname;
	string ip_host;
	string ip_host_skip;
	int daemon = 1;

	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-f") == 0)
		{
			daemon = 0;
			printf("Running in foreground, log to stdout.\n");
		}
		else
		{
			usage();
		}
	}

	openlog("afraid-dns", LOG_PID | LOG_CONS | (daemon ? 0 : LOG_PERROR), LOG_DAEMON);

	signal(SIGCHLD, SIG_IGN); 			// ignore child
	signal(SIGTSTP, SIG_IGN); 			// ignore tty signals
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGINT, signal_handler); 	// catch Ctrl-C signal (terminating in foreground mode)
	signal(SIGHUP, signal_handler); 	// catch hangup signal (reload config - not implemented yet)
	signal(SIGTERM, signal_handler); 	// catch kill signal

	if(! Config::ReadParam("username", username))
	{
		Util::Log(LogError, "Can't find username in config, exiting");
		exit(1);
	}
	if(! Config::ReadParam("password", password))
	{
		Util::Log(LogError, "Can't find password in config, exiting");
		exit(1);
	}
	if(! Config::ReadParam("hostname", hostname))
	{
		Util::Log(LogError, "Can't find hostname from in, exiting");
		exit(1);
	}
	if(! Config::ReadParam("ip_host", ip_host))
	{
		ip_host = "myip.dnsomatic.com";
	}
	else
	{
		Config::ReadParam("ip_host_skip", ip_host_skip);
	}

	if(daemon)
	{
		daemonize();
	}

	AfraidDns afraidDns(hostname, ip_host, ip_host_skip);

	if(! afraidDns.CalcSHA1(username + "|" + password))
	{
		Util::Log(LogError, "Internal error, can't calc SHA1, terminating");
	}
	else
	{
		running = 1;

		while(running && ! afraidDns.GetApiKeys())
		{
			sleep(60);
		}

		while(running)
		{
			if(afraidDns.Update())
			{
				sleep(60 * 5);	// success, wait 5 minutes
			}
			else
			{
				sleep(60);		// fail, wait 1 minute
			}
		}
	}

	closelog();

	// close pid file and delete it

	if(lock_fd != -1)
	{
		close(lock_fd);
		if(unlink(PID_FILE) != 0)
		{
			Util::Log(LogError, "unlink(PID_FILE) failed: " + string(strerror(errno)));
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------

