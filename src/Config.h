/*
 * Config.h
 *
 *  Created on: Mar 12, 2009
 *      Author: mike
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

using namespace std;

class Config
{
public:
	static bool ReadParam(const string& param, string& result, const string fname = "/etc/afraid-dns.conf");

private:
	static char *SkipWs(char *s);
};

#endif /* CONFIG_H_ */
