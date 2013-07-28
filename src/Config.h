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

#define CFG_FILE	"/etc/afraid-dns.conf"

class Config
{
public:
	static bool ReadParam(const string& param, string& result, const string fname = CFG_FILE);

private:
	static char *SkipWs(char *s);
};

#endif /* CONFIG_H_ */
