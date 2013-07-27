/*
 * Config.cpp
 *
 *  Created on: Mar 12, 2009
 *      Author: mike
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <string>
#include "Config.h"
#include "Util.h"

using namespace std;

//-----------------------------------------------------------------------------

char *Config::SkipWs(char *s)
{
	while(isspace(*s))
		++s;
	return s;
}

//-----------------------------------------------------------------------------

bool Config::ReadParam(const string& param, string& result, const string fname)
{
	result.clear();

	FILE *fp = fopen(fname.c_str(), "r");

	if(fp == NULL)
	{
		Util::Log(LogError, "Can't open config file " + fname + ": " + strerror(errno));
	}
	else
	{
		while(! feof(fp))
		{
			char buf[256];
			char *line;
			char *nl;

			line = fgets(buf, sizeof(buf), fp);
			if(line != NULL)
			{
				line = SkipWs(line);

				if(line != NULL && line[0] != '#')
				{
					if(strncmp(line, param.c_str(), param.length()) == 0)
					{
						line = strchr(line, '=');
						if(line != NULL)
						{
							++line;
							line = SkipWs(line);
							if(line != NULL && *line != '\0')
							{
								nl = &line[strlen(line) - 1];
								if(*nl == '\n')
									*nl = '\0';
								result = line;
								line = NULL;
								break;
							}
						}
					}
				}
			}
		}

		fclose(fp);
	}

	return result.length() > 0;
}

//-----------------------------------------------------------------------------
