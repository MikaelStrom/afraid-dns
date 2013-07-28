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
#include <syslog.h>
#include <sstream>

#include "Util.h"
#include "Config.h"

//-----------------------------------------------------------------------------

void Util::Log(LogLevel level, string msg, string indent)
{
	static int log_level = -1;

	// read config file only once

	if(log_level == -1)
	{
		string s;
		if(Config::ReadParam("syslog", s))
		{
			int n = atoi(s.c_str());

			if(n > LogDebug)
			{
				log_level = LogDebug;
			}
			else if(n < LogNothing)
			{
				log_level = LogNothing;
			}
			else
			{
				log_level = n;
			}
		}
	}

	if(level <= log_level)
	{
		int sys_level = -1;
		const char* prefix;

		switch(level)
		{
		case LogNothing:	sys_level = -1;			prefix = "";		break;
		case LogError:		sys_level = LOG_ERR;	prefix = "ERROR";	break;
		case LogInfo:		sys_level = LOG_INFO;	prefix = "INFO";	break;
		case LogDebug:		sys_level = LOG_DEBUG;	prefix = "DEBUG";	break;
		}

		if(sys_level != -1)
		{
			stringstream ss(msg);
			std::string line;

			// get head

			while(getline(ss, line))
			{
				syslog(sys_level, "%s: %s%s", prefix, indent.c_str(), line.c_str());
			}
		}
	}
}

//-----------------------------------------------------------------------------

bool Util::ExtractIp(const string src, string& dest)
{
	int a, b, c, d;

	if(sscanf(src.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
	{
		char buf[32];
		sprintf(buf, "%d.%d.%d.%d", a, b, c, d);
		dest = buf;
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

vector<string> Util::Tokenize(const string& text, const char delim)
{
	string s;
	stringstream ss(text);
	vector<string> result;

	while(getline(ss, s, delim))
	{
		result.push_back(s);
	}

	return result;
}

//-----------------------------------------------------------------------------
