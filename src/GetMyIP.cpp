/*
 * GetMyIP.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <stdio.h>
#include <string.h>
#include "Util.h"
#include "GetMyIP.h"

//------------------------------------------------------------------------------

GetMyIP::GetMyIP(const string hostname, const string skipToString)
:	m_hostname(hostname),
 	m_host(hostname),
 	m_skip(skipToString)
{
}

//------------------------------------------------------------------------------

bool GetMyIP::Get(string& myIp)
{
	string req = "GET /";
	vector<string> response;

	if(m_host.Request(req, response))
	{
		for(unsigned i = 0; i <response.size(); ++i)
		{
			string& line = response[i];

			size_t start = line.find(m_skip);
			if(start != string::npos)
			{
				if(Util::ExtractIp(line.substr(start + m_skip.length()), myIp))
				{
					return true;
				}
			}
		}
	}

	Util::Log(LOG_CRIT, "failed to get IP from " + m_hostname);

	return false;
}

//------------------------------------------------------------------------------
