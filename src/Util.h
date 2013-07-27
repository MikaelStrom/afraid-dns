/*
 * Util.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <syslog.h>
#include <string>
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------

class Util
{
public:
	Util()				{};
	virtual ~Util()		{};

	static void Log(int pri, string msg, bool terminate = false);

	static bool ExtractIp(const string src, string& dest);

	static vector<string> Tokenize(const string& text, const char delim);
};

//-----------------------------------------------------------------------------

#endif /* UTIL_H_ */
