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
