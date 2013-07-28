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


#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

using namespace std;

#define CFG_FILE	"/etc/afraid-dns.conf"

class Config
{
public:
	static bool ReadParam(const string& param, string& result, const string fname = CFG_FILE);

private:
	static char *SkipWs(char *s);
};

#endif /* CONFIG_H_ */
