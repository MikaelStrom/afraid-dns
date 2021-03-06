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


#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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
		Util::Log(LogError, "Can't resolve hostname " + m_hostname + ": " + string(strerror(errno)));
		return false;
	}

	Util::Log(LogDebug, "Found host " + m_hostname + " at " + inet_ntoa(*((struct in_addr *)hostinfo->h_addr)));

	struct sockaddr_in name;
	name.sin_addr = *((struct in_addr *) hostinfo->h_addr);
	name.sin_family = AF_INET;
	name.sin_port = htons(80);

	// Connect to the web server
	if(connect(m_fd, (const sockaddr*) &name, sizeof(struct sockaddr_in)) < 0)
	{
		Util::Log(LogError, string("connect() failed: ") + strerror(errno));
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

bool Connection::Request(const string& request, vector<string>& result_head, vector<string>& result_body)
{
	result_head.clear();
	result_body.clear();

	string temp;
	string req = request + " HTTP/1.1\n";
	req += "Host: ";
	req += m_hostname;
	req += "\n\n";


	Util::Log(LogDebug, "Request:" );
	Util::Log(LogDebug, req, " ");

	if(! Open())
	{
		return false;
	}

	if(Write(req.c_str(), req.length()) < 0)
    {
    	Util::Log(LogError, "write(socket_fd) failed");
    	return false;
    }

	while(1)
	{
		const int READ_SIZE = 10000;
		char buf[READ_SIZE + 1];
		ssize_t n_read;

		n_read = Read(buf, READ_SIZE);

		if(n_read < 0)
	    {
	    	return false;
	    }
		else if(n_read == 0)
		{
			break;
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

	// get head

	Util::Log(LogDebug, "Response head:");

	while(getline(ss, line))
	{
		if(line[line.length() - 1] == '\n' || line[line.length() - 1] == '\r')
		{
			line.erase(line.length() - 1);	// strip CR / LF
		}

		if(line.length() == 0)
		{
			break;	// empty line, body follows
		}

		result_head.push_back(line);

		Util::Log(LogDebug, line, " ");
	}

	// get body

	Util::Log(LogDebug, "Response body:");

	while(getline(ss, line))
	{
		if(line[line.length() - 1] == '\n' || line[line.length() - 1] == '\r')
		{
			line.erase(line.length() - 1);	// strip CR / LF
		}

		result_body.push_back(line);

		Util::Log(LogDebug, line, " ");
	}

	return result_head.size() > 0;
}

//-----------------------------------------------------------------------------

ssize_t Connection::Write(const char* buf, size_t size)
{
	return write(m_fd, (void*) buf, size);
}

//-----------------------------------------------------------------------------

ssize_t Connection::Read(char* buf, size_t size)
{
	ssize_t result =  0;

	// set receive time out
	struct timeval tv;
	tv.tv_sec = 5;		// time out after 30 seconds
	tv.tv_usec = 0;

	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_fd, &fds);

	// Wait until timeout or data received.
	int n = select(m_fd + 1, &fds, NULL, NULL, &tv);
	if(n == 0)
	{
		result = 0;
	}
	else if(n < 0)
	{
		Util::Log(LogError, "Connection::Read: select() failed: " + string(strerror(errno)));
		result = -1;
	}
	else
	{
		// read from the socket
		result = recvfrom(m_fd, buf, size, 0, NULL, NULL);
	}

	return result;
}

//-----------------------------------------------------------------------------
