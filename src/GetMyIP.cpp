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
	vector<string> head;
	vector<string> body;

	if(m_host.Request(req, head, body))
	{
		for(unsigned i = 0; i <body.size(); ++i)
		{
			string& line = body[i];

			size_t start = line.find(m_skip);
			if(start != string::npos)
			{
				if(Util::ExtractIp(line.substr(start + m_skip.length()), myIp))
				{
					Util::Log(LogInfo, "Successfully detected IP " + myIp + " from " + m_hostname);

					return true;
				}
			}
		}
	}

	Util::Log(LogError, "failed to get IP from " + m_hostname);

	return false;
}

//------------------------------------------------------------------------------
