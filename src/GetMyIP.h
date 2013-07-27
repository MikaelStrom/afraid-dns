/*
 * GetMyIP.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#ifndef GETMYIP_H_
#define GETMYIP_H_

#include <string>
#include "Connection.h"

using namespace std;

class GetMyIP
{
public:
	GetMyIP(const string hostname, const string skipToString = "");

	bool Get(string& myIp);

private:
//	bool ExtractIp(const string src, string& dest);
	string m_hostname;
	Connection m_host;
	string m_skip;
};

#endif /* GETMYIP_H_ */
