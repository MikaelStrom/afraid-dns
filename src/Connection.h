/*
 * Connection.h
 *
 *  Created on: Jul 27, 2013
 *      Author: mike
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
