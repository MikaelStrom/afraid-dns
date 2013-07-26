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

#include "main.h"
#include "GetMyIP.h"
#include "AfraidDns.h"

//-----------------------------------------------------------------------------

#define HOSTNAME	"freedns.afraid.org"

//-----------------------------------------------------------------------------

AfraidDns::AfraidDns(const string username, const string password, const string hostname)
:	m_afraid_host(HOSTNAME),
	m_dns_host(hostname),
	myIp("get-site-ip.com", "Your ip is")
{
	// create SHA

	CalcSHA1(username + "|" + password);
}

//-----------------------------------------------------------------------------

AfraidDns::~AfraidDns()
{
}

//-----------------------------------------------------------------------------

void AfraidDns::CalcSHA1(const string s)
{
    SHA_CTX context;
	unsigned char out[SHA_DIGEST_LENGTH + 1];

    if(! SHA1_Init(&context))
    {
    	Log(LOG_CRIT, "SHA1_Init() failed", true);
    }

    if(! SHA1_Update(&context, (unsigned char*)s.c_str(), s.length()))
    {
    	Log(LOG_CRIT, "SHA1_Update() failed", true);
    }

    if(! SHA1_Final(out, &context))
    {
    	Log(LOG_CRIT, "SHA1_Final() failed", true);
    }

    for(int i = 0; i < SHA_DIGEST_LENGTH; i++)
	{
		char buf[8];
		sprintf(buf, "%02x", out[i]);
		m_sha_digest += buf;
	}
}

//-----------------------------------------------------------------------------

vector<string> AfraidDns::Tokenize(const string& text, const char delim)
{
	string s;
	stringstream ss(text);
	vector<string> result;

	while(getline(ss, s, delim))
	{
		result.push_back(s);
	}

	return result;
}

//-----------------------------------------------------------------------------

bool AfraidDns::CreateApiKey(const string hostname, const vector<string> text, string& api_key)
{
	api_key.clear();
	string key;

	// find right entry

	for(unsigned i = 0; i < text.size(); ++i)
	{
		vector<std::string> tokens = Tokenize(text[i], '|');

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
    	Log(LOG_CRIT, "No api key found matching '" + hostname + "'", true);
	}

	// String has format: http://freedns.afraid.org/dynamic/update.php?UnlDelkwbkRBd2RyUHhiS2R1TFY6OTY4NTI3Mg==
	// Discard leading "http://freedns.afraid.org"

	if(key.length() > 0)
	{
		size_t start = key.find("/dynamic/update.php?");
		if (start == string::npos)
		{
	    	Log(LOG_CRIT, "Failed to parse api key. Key = '" + key + "'", true);
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

	vector<string> result;
	string request = "GET /api/?action=getdyndns&sha=";
	request += m_sha_digest;

	if(! m_afraid_host.Request(request, result))
	{
		return false;
	}

	// find our key

	return CreateApiKey(m_dns_host, result, m_api_key);
}

//-----------------------------------------------------------------------------

bool AfraidDns::UpdateIp()
{
	string my_ip;
	bool changed;

	myIp.Get(my_ip, changed);

	if(changed)
	{

		// send

		vector<string> update_result;
		string update_request = "GET ";
		update_request += m_api_key;

		if(! m_afraid_host.Request(update_request, update_result))
		{
			return false;
		}

		// parse result

		for(unsigned i = 0; i < update_result.size(); ++i)
		{
			printf("%s\n", update_result[i].c_str());
		}
	}

	return true;
}

//-----------------------------------------------------------------------------

