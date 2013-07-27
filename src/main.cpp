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

#include "Util.h"
#include "AfraidDns.h"
#include "Config.h"

using namespace std;

//-----------------------------------------------------------------------------

const char username[] = "mikestrom";
const char password[] = "svullo";
const char dns_host[] = "mikestrom.linuxd.net";

int main(int argc, char *argv[])
{
	openlog("afraid-dns", LOG_PID | LOG_CONS | LOG_PERROR, LOG_USER);

	string username;
	string password;
	string hostname;
	string ip_host;
	string ip_host_skip;

	if(! Config::ReadParam("username", username))
	{
		Util::Log(LogError, "Can't get username from config", "", true);
	}

	if(! Config::ReadParam("password", password))
	{
		Util::Log(LogError, "Can't get password from config", "", true);
	}

	if(! Config::ReadParam("hostname", hostname))
	{
		Util::Log(LogError, "Can't get hostname from config", "", true);
	}

	if(! Config::ReadParam("ip_host", ip_host))
	{
		ip_host = "myip.dnsomatic.com";
	}
	else
	{
		Config::ReadParam("ip_host_skip", ip_host_skip);
	}

	AfraidDns afraidDns(username, password, dns_host, ip_host, ip_host_skip);

	while(! afraidDns.GetApiKeys())
	{
		sleep(60);
	}

	while(1)
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

	closelog();

	return 0;
}

//-----------------------------------------------------------------------------
