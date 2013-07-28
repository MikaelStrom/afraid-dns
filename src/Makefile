#
# Makefile for afraid-dns
#
# Mikael Strom, July 2013
#

CC = g++
CFLAGS = -Wall -O3
LFLAGS = -lssl -lcrypto -o afraid-dns

SOURCE = main.cpp AfraidDns.cpp Config.cpp Connection.cpp Util.cpp
HEADERS = AfraidDns.h Config.h Connection.h Util.h

all: afraid-dns

afraid-dns: $(SOURCE) $(HEADERS) 
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

clean:
	rm afraid-dns

install:
	sudo install -s -o root -g root -m 0755 afraid-dns /usr/sbin
	sudo install -o root -g root -m 0600 afraid-dns.conf /etc
	sudo install -o root -g root -m 0755 init.d /etc/init.d/afraid-dns
	sudo install -o root -g root -m 0644 afraid-dns.1 /usr/share/man/man1/
	sudo gzip /usr/share/man/man1/afraid-dns.1
	@echo  
	@echo ------------------------------------------------------- 
	@echo Install completed. 
	@echo Edit the config file: $$ sudo nano /etc/afraid-dns.conf
	@echo To start manually: $$ sudo service afraid-dns start
	@echo To get more info, read the man page: $$ man afraid-dns

uninstall:
	sudo service afraid-dns stop
	sudo update-rc.d -f afraid-dns remove
	sudo rm /usr/sbin/afraid-dns /etc/init.d/afraid-dns /etc/afraid-dns.conf /usr/local/man/man1/afraid-dns.1
