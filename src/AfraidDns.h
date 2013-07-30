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


#ifndef AFRAIDDNS_H_
#define AFRAIDDNS_H_

#include <string>
#include <vector>
#include "Connection.h"

using namespace std;

class AfraidDns
{
public:
	AfraidDns(const string domain);

	bool CalcSHA1(const string s);	// do sthis first, format "username|password"

	bool GetApiKeys();				// then this,

	bool Update();					// and repeat this until death.

private:
	bool UpdateIp(string& response_ip, bool& changed);
	bool CreateApiKey(const string hostname, const vector<string> text, string& api_key);

	Connection m_afraid_host;
	string m_sha_digest;
	string m_domain;
	string m_api_key;
	string m_last_ip;
};

#endif /* AFRAIDDNS_H_ */
