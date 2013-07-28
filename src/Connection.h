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


#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <string>
#include <vector>

using namespace std;

//-----------------------------------------------------------------------------

class Connection
{
public:
	Connection(const string hostname);
	virtual ~Connection();

	bool Request(const string& request, vector<string>& result_head, vector<string>& result_body);

private:
	bool Open();
	void Close();

	ssize_t Write(const char* buf, size_t size);
	ssize_t Read(char* buf, size_t size);

	string m_hostname;
	int m_fd;
};

//-----------------------------------------------------------------------------

#endif /* CONNECTION_H_ */
