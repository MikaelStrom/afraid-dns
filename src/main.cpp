/*
 * main.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "AfraidDns.h"

using namespace std;

//-----------------------------------------------------------------------------

void Log(int pri, string msg, bool terminate)
{
	if(terminate)
	{
		msg += ", terminating.";
	}

	syslog(pri, "%s", msg.c_str());

	if(terminate)
	{
		exit(1);
	}
}

//-----------------------------------------------------------------------------

const char username[] = "mikestrom";
const char password[] = "svullo";
const char dns_host[] = "mikestrom.linuxd.net";

int main(int argc, char *argv[])
{
	openlog("afraid-dns", LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	AfraidDns afraidDns(username, password, dns_host);

	while(! afraidDns.GetApiKeys())
	{
		sleep(60);
	}

	while(1)
	{
		afraidDns.UpdateIp();
		sleep(60 * 5);
	}

	closelog();
	return 0;
}

//-----------------------------------------------------------------------------