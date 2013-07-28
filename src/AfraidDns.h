/*
 * AfraidDns.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#ifndef AFRAIDDNS_H_
#define AFRAIDDNS_H_

#include <string>
#include <vector>
#include "Connection.h"

using namespace std;

class AfraidDns
{
public:
	AfraidDns(const string hostname, const string ip_host, const string ip_skip);

	bool CalcSHA1(const string s);	// do sthis first, format "username|password"

	bool GetApiKeys();				// then this,

	bool Update();					// and repeat this until death.

private:
	bool UpdateIp(string& response_ip, bool& changed);
	bool CreateApiKey(const string hostname, const vector<string> text, string& api_key);

	Connection m_afraid_host;
	string m_sha_digest;
	string m_dns_host;
	string m_api_key;
	string m_last_ip;
};

#endif /* AFRAIDDNS_H_ */
