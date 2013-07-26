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
#include "GetMyIP.h"
#include "Connection.h"

using namespace std;

class AfraidDns
{
public:
	AfraidDns(const string username, const string password, const string hostname);
	virtual ~AfraidDns();

	bool GetApiKeys();

	bool UpdateIp();

private:
	void CalcSHA1(const string s);
	bool CreateApiKey(const string hostname, const vector<string> text, string& api_key);
	vector<string> Tokenize(const string& text, const char delim);

	Connection m_afraid_host;
	string m_sha_digest;
	string m_dns_host;
	string m_api_key;

	GetMyIP myIp;
};

#endif /* AFRAIDDNS_H_ */
