/*
 * Connection.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
 */

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include <sstream>

#include "Util.h"
#include "Connection.h"

#define INVALID	-1

//-----------------------------------------------------------------------------

Connection::Connection(const string hostname)
:	m_hostname(hostname)
{
	m_fd = INVALID;
}

//-----------------------------------------------------------------------------

Connection::~Connection()
{
	if(m_fd != INVALID)
	{
		Close();
	}
}

//-----------------------------------------------------------------------------

bool Connection::Open()
{
	m_fd = socket(PF_INET, SOCK_STREAM, 0);

	struct hostent* hostinfo;
	hostinfo = gethostbyname(m_hostname.c_str());
	if (hostinfo == NULL)
	{
		Util::Log(LOG_CRIT, "Can't resolve hostname " + m_hostname + ": " + string(strerror(errno)));
		return false;
	}

	struct sockaddr_in name;
	name.sin_addr = *((struct in_addr *) hostinfo->h_addr);
	name.sin_family = AF_INET;
	name.sin_port = htons(80);

	// Connect to the web server
	if(connect(m_fd, (const sockaddr*) &name, sizeof(struct sockaddr_in)) < 0)
	{
		Util::Log(LOG_CRIT, string("connect() failed: ") + strerror(errno));
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------

void Connection::Close()
{
	close(m_fd);

	m_fd = INVALID;
}

//-----------------------------------------------------------------------------

bool Connection::Request(const string& request, vector<string>& result)
{
	result.clear();
	string temp;
	string req = request + " HTTP/1.1\n";
	req += "Host: ";
	req += m_hostname;
	req += "\n\n";

	if(! Open())
	{
		return false;
	}

	if(Write(req.c_str(), req.length()) < 0)
    {
    	Util::Log(LOG_CRIT, "write(socket_fd) failed");
    	return false;
    }

	int count = 5;

	while(--count)
	{
		const int READ_SIZE = 10000;
		char buf[READ_SIZE + 1];
		ssize_t n_read;

		n_read = Read(buf, READ_SIZE);

		if(n_read < 0)
	    {
	    	Util::Log(LOG_CRIT, "read(socket_fd) failed");
	    	return false;
	    }
		else if(n_read == 0)
		{
			sleep(1);
//			break;
		}
		else
		{
			buf[n_read] = 0;
			temp += buf;
		}
	}

	Close();

	stringstream ss(temp);
	std::string line;
	while(getline(ss, line))
	{
		if(line.length() > 1 && (line[line.length() - 1] == '\n' || line[line.length() - 1] == '\r'))
		{
			line.erase(line.length() - 1);
		}
		result.push_back(line);
	}

	return result.size() > 0;
}

//-----------------------------------------------------------------------------

ssize_t Connection::Write(const char* buf, size_t size)
{
	return write(m_fd, (void*) buf, size);
}

//-----------------------------------------------------------------------------

ssize_t Connection::Read(char* buf, size_t size)
{
	return read(m_fd, buf, size);
}

//-----------------------------------------------------------------------------
