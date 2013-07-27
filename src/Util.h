/*
 * Util.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
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

	static void Log(LogLevel level, string msg, string indent = "", bool terminate = false);

	static bool ExtractIp(const string src, string& dest);

	static vector<string> Tokenize(const string& text, const char delim);
};

//-----------------------------------------------------------------------------

#endif /* UTIL_H_ */
