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

#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------

enum LogLevel { LogNothing = 0, LogError = 1, LogInfo = 2, LogDebug = 3 };

class Util
{
public:

	static void Log(LogLevel level, string msg, string indent = "");
	static void OpenLogFile(const string fname);
	static void EnableStdoutLog()					{ m_log_stdout = true; }
	static void SetLogLevel(LogLevel level)			{ log_level = level; }

	static bool ExtractIp(const string src, string& dest);

	static vector<string> Tokenize(const string& text, const char delim);

private:
	static FILE* m_log_fp;
	static bool m_log_stdout;
	static int log_level;
};

//-----------------------------------------------------------------------------

#endif /* UTIL_H_ */
