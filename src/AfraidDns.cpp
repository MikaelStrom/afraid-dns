/*
 * AfraidDns.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <unistd.h>
#include <stdio.h>
#include <openssl/sha.h>
#include <sstream>
#include <algorithm>

#include "Util.h"
#include "AfraidDns.h"

//-----------------------------------------------------------------------------

#define HOSTNAME	"freedns.afraid.org"

//-----------------------------------------------------------------------------

AfraidDns::AfraidDns(const string hostname, const string ip_host, const string ip_skip)
:	m_afraid_host(HOSTNAME),
	m_dns_host(hostname)
{
	m_last_ip = "<unknown>";
}

//-----------------------------------------------------------------------------

bool AfraidDns::CalcSHA1(const string s)
{
    SHA_CTX context;
	unsigned char out[SHA_DIGEST_LENGTH + 1];

    if(! SHA1_Init(&context))
    {
    	Util::Log(LogError, "SHA1_Init() failed");
    	return false;
    }

    if(! SHA1_Update(&context, (unsigned char*)s.c_str(), s.length()))
    {
    	Util::Log(LogError, "SHA1_Update() failed");
    	return false;
    }

    if(! SHA1_Final(out, &context))
    {
    	Util::Log(LogError, "SHA1_Final() failed");
    	return false;
    }

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		char buf[8];
		sprintf(buf, "%02x", out[i]);
		m_sha_digest += buf;
	}

    return true;
}

//-----------------------------------------------------------------------------

bool AfraidDns::CreateApiKey(const string hostname, const vector<string> text, string& api_key)
{
	string key;

	api_key.clear();

	// find right entry

	for(unsigned i = 0; i < text.size(); ++i)
	{
		vector<std::string> tokens = Util::Tokenize(text[i], '|');

		if(tokens.size() == 3)
		{
			if(tokens[0] == hostname)
			{
				key = tokens[2];
				break;
			}
		}
	}

	if(key.length() == 0)
	{
    	Util::Log(LogError, "No api key found matching '" + hostname + "'");
    	return false;
	}

	// String has format: "http://freedns.afraid.org/dynamic/update.php?UnlDelkwbkRBd2RyUHhiS2R1TFY6OTY4NTI3Mg=="
	// discard leading "http://freedns.afraid.org"

	if(key.length() > 0)
	{
		size_t start = key.find("/dynamic/update.php?");
		if (start == string::npos)
		{
	    	Util::Log(LogError, "Failed to parse api key. Key = '" + key + "'");
	    	return false;
		}

		api_key = key.substr(start);
	}

	api_key = key;

	return true;
}

//-----------------------------------------------------------------------------

bool AfraidDns::GetApiKeys()
{
	// Retrieve api keys

	vector<string> head;
	vector<string> body;
	string request = "GET /api/?action=getdyndns&sha=";
	request += m_sha_digest;

	if(! m_afraid_host.Request(request, head, body))
	{
		return false;
	}

	// create our key

	return CreateApiKey(m_dns_host, body, m_api_key);
}

//-----------------------------------------------------------------------------

bool AfraidDns::Update()
{
	bool success;
	string response_ip;
	bool changed = false;

	success = UpdateIp(response_ip, changed);

	if(! success)
	{
		Util::Log(LogError, "Failed to update IP on " + string(HOSTNAME));
	}
	else if(changed)
	{
		Util::Log(LogInfo, "Successfully updated IP from " + m_last_ip + " to " + response_ip);
		m_last_ip = response_ip;
	}
	else
	{
		Util::Log(LogInfo, "IP not changed (still " + response_ip + ")");
	}

	return success;
}

//-----------------------------------------------------------------------------

bool AfraidDns::UpdateIp(string& response_ip, bool& changed)
{
	vector<string> head;
	vector<string> body;
	string update_request = "GET ";
	update_request += m_api_key;

	response_ip.clear();
	changed = false;

	if(! m_afraid_host.Request(update_request, head, body))
	{
		return false;
	}

	// On success the result, can be either
	//   Updated mikestrom.linuxd.net to 112.209.21.79 in 0.576 seconds
	// or
	//   ERROR: Address 112.209.21.79 has not changed.

	string result_ip;

	for(unsigned i = 0; i < body.size(); ++i)
	{
		const string search_1 = "to";
		const string search_2 = "ERROR: Address";

		const string& line = body[i];

		// try Updated... to ...

		size_t s1 = line.find("Updated");
		size_t s2 = line.find(search_1);
		if(s1 != string::npos && s2 != string::npos)
		{
			Util::ExtractIp(line.substr(s2 + search_1.length()), response_ip);
			changed = true;
			break;
		}

		// try Updated... to ...

		size_t s3 = line.find(search_2);
		if(s3 != string::npos)
		{
			Util::ExtractIp(line.substr(s3 + search_2.length()), response_ip);
			break;
		}
	}

	return response_ip.length() > 0;
}

//-----------------------------------------------------------------------------

