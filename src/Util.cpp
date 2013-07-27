/*
 * Util.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include <sstream>

//-----------------------------------------------------------------------------

void Util::Log(int pri, string msg, bool terminate)
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
