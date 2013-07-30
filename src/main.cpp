/*
 * Copyright 2013 Mikael Ström
 * This file is part of afraid-dns.
 *
 * afraid-dns is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * afraid-dns is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with afraid-dns (COPYING).  If not, see <http://www.gnu.org/licenses/>.
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
#include <time.h>

#include "Util.h"
#include "AfraidDns.h"
#include "Config.h"

using namespace std;

//-----------------------------------------------------------------------------

#define PID_FILE		"/var/run/afraid-dns.pid"
#define RETRY_SECONDS	60

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
		Util::Log(LogInfo, "Received sginal SIGINT/SIGTERM, terminating.");
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
	if(chdir("/") != 0)
	{
		Util::Log(LogError, "chdir('/'): " + string(strerror(errno)));
	}

	// drop root privs (to user daemon)

// TODO: To lazy to fix this; requires some extra work as the pid file can't be
//		 deleted if created in /var/run/ as root. One way is to create /var/run/afraid/
//		 with permissions that allow user daemon to delete it.
//
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
		Util::Log(LogError, "open(PID_FILE) failed: " + string(strerror(errno)) + ". Probably another instance is already running, terminating.");
		exit(1); 		// open failed, we're a duplicate
	}

	if(lockf(lock_fd, F_TLOCK, 0) < 0)
	{
		Util::Log(LogError, "lockf(lock_fd) failed: " + string(strerror(errno)) + ", terminating.");
		exit(1); 		// lock failed - no idea what this means...
	}

	// write pid to file, and leave file open (blocking duplicates)

	char str[32];
	sprintf(str, "%d\n", getpid());
	if(write(lock_fd, str, strlen(str)) < 1)
	{
		Util::Log(LogError, "write(lock_fd) failed: " + string(strerror(errno)));
	}
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
	bool daemon = true;
	string username;
	string password;
	string domain;
	int update_interval;
	time_t next_update = 0;

	for(int i = 1; i < argc; ++i)
	{
		if(strcmp(argv[i], "-f") == 0)
		{
			daemon = false;
			printf("Running in foreground, log to stdout.\n");
			Util::EnableStdoutLog();
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

	// read config file

	string logfile;
	if(Config::ReadParam("logfile", logfile))
	{
		Util::OpenLogFile(logfile);
	}

	string syslog;
	if(Config::ReadParam("syslog", syslog))
	{
		LogLevel n = (LogLevel) atoi(syslog.c_str());

		if(n > LogDebug)
		{
			Util::SetLogLevel(LogDebug);
		}
		else if(n < LogNothing)
		{
			Util::SetLogLevel(LogNothing);
		}
		else
		{
			Util::SetLogLevel(n);
		}
	}

	if(! Config::ReadParam("username", username))
	{
		Util::Log(LogError, "Can't find 'username' in config, terminating.");
		exit(1);
	}

	if(! Config::ReadParam("password", password))
	{
		Util::Log(LogError, "Can't find 'password' in config, terminating.");
		exit(1);
	}

	if(! Config::ReadParam("domain", domain))
	{
		Util::Log(LogError, "Can't find 'domain' in config, terminating.");
		exit(1);
	}

	string iv_str;
	if(! Config::ReadParam("interval", iv_str))
	{
		Util::Log(LogInfo, "Can't find 'interval' in config, defaulting to 5 minutes.");
		update_interval = 5;
	}
	else
	{
		update_interval = atoi(iv_str.c_str());
		if(update_interval < 5)
		{
			Util::Log(LogError, "Invalid 'interval' value in config, defaulting to 5 minutes.");
			update_interval = 5;
		}
	}
	update_interval *= 60;	// minutes -> seconds

	// Set up

	AfraidDns afraidDns(domain);

	if(! afraidDns.CalcSHA1(username + "|" + password))
	{
		Util::Log(LogError, "Internal error, can't calc SHA1, terminating");
		exit(1);
	}

	// fork off if not in foreground mode

	if(daemon)
	{
		daemonize();
		Util::Log(LogInfo, "Daemon started.");
	}

	// first get api keys

	while(running && ! afraidDns.GetApiKeys())
	{
		sleep(RETRY_SECONDS);
	}

	// then update until IP terminated

	while(running)
	{
		time_t now = time(NULL);
		if(now > next_update)
		{
			next_update = now + update_interval + ((rand() * 1.0 / RAND_MAX) * 60) - 30;	// spread updates

			if(! afraidDns.Update())
			{
				next_update = now + RETRY_SECONDS;	// retry after RETRY_SECONDS
			}
		}
		sleep(1);
	}


	// close pid file and delete it

	if(lock_fd != -1)
	{
		close(lock_fd);
		if(unlink(PID_FILE) != 0)
		{
			Util::Log(LogError, "unlink(PID_FILE) failed: " + string(strerror(errno)));
		}
	}

	Util::Log(LogInfo, "Terminated.");

	closelog();

	return 0;
}

//-----------------------------------------------------------------------------

