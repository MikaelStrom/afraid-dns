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
#include <errno.h>
#include <syslog.h>
#include <sstream>

#include "Util.h"
#include "Config.h"

//-----------------------------------------------------------------------------

FILE* Util::m_log_fp = NULL;
bool Util::m_log_stdout = false;
int Util::log_level = LogError;

//-----------------------------------------------------------------------------

void Util::Log(LogLevel level, string msg, string indent)
{
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

			while(getline(ss, line))
			{
				if(m_log_fp != NULL)
				{
					char buf[128];

					time_t now = time(NULL);
					struct tm *ts = localtime(&now);

					strftime(buf, sizeof(buf), "%x %X", ts);	// ISO style date and time

					fprintf(m_log_fp, "%s %s: %s%s\n", buf, prefix, indent.c_str(), line.c_str());
					fflush(m_log_fp);

					if(m_log_stdout)
					{
						printf("%s %s: %s%s\n", buf, prefix, indent.c_str(), line.c_str());
					}
				}
				else
				{
					syslog(sys_level, "%s: %s%s", prefix, indent.c_str(), line.c_str());
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------

void Util::OpenLogFile(const string fname)
{
	m_log_fp = fopen(fname.c_str(), "a");

	if(m_log_fp == NULL)
	{
		Log(LogError, "Failed to open log file" + fname + ": " + strerror(errno));
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
