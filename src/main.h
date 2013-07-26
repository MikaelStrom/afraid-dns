/*
 * afraid-dns.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#ifndef AFRAID_DNS_H_
#define AFRAID_DNS_H_

#include <string>
#include <syslog.h>

using namespace std;

void Log(int pri, string msg, bool terminate = false);

#endif /* AFRAID_DNS_H_ */
