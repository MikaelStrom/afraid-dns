/*
 * GetMyIP.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "GetMyIP.h"

//------------------------------------------------------------------------------

GetMyIP::GetMyIP(const string hostname, const string skipToString)
:	m_hostname(hostname),
 	m_host(hostname),
 	m_skip(skipToString)
{
}

//------------------------------------------------------------------------------

bool GetMyIP::Get(string& myIp, bool& changed)
{
	string req = "GET /";
	vector<string> result;
	string new_ip;

	changed = false;

	if(m_host.Request(req, result))
	{
		for(unsigned i = 0; i <result.size(); ++i)
		{
			string& line = result[i];

			size_t start = line.find(m_skip);
			if(start != string::npos)
			{
				int a, b, c, d;
				string ip_str = line.substr(start);

				if(sscanf(ip_str.c_str(), "%d.%d.%d.%d", a, b, c, d) == 4)
				{
					char buf[32];
					sprintf(buf, "%d.%d.%d.%d", a, b, c, d);
					new_ip = buf;
					break;
				}
			}
		}
	}

	if(new_ip.length() > 0)
	{
		changed = new_ip != m_last_ip;
		m_last_ip = new_ip;
		myIp = new_ip;
		return true;
	}
	else
	{
	   	Log(LOG_CRIT, "failed to get IP from " + m_hostname);
	   	return false;
	}
}

//------------------------------------------------------------------------------
