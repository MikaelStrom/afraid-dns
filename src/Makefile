#
# Makefile for afraid-dns
#
# Mikael Strom, July 2013
#
# Note: depends libssl, installed on Debian/Ubuntu via
#   $ sudo apt-get install libssl-dev
#

CC = g++
CFLAGS = -Wall -O3 `pkg-config --cflags libssl`
LFLAGS = `pkg-config --libs libssl` -o afraid-dns 

SOURCE = main.cpp AfraidDns.cpp Config.cpp Connection.cpp Util.cpp
HEADERS = AfraidDns.h Config.h Connection.h Util.h

all: afraid-dns

afraid-dns: $(SOURCE) $(HEADERS) 
	$(CC) $(CFLAGS) $(SOURCE) $(LFLAGS)

clean:
	rm afraid-dns

install:
	sudo install -s -o root -g root -m 0755 afraid-dns /usr/sbin
	if ! test -f /etc/afraid-dns.conf; then sudo install -o root -g root -m 0600 afraid-dns.conf /etc; fi
	sudo install -o root -g root -m 0755 init.d /etc/init.d/afraid-dns
	sudo install -o root -g root -m 0644 afraid-dns.1 /usr/share/man/man1/
	sudo gzip -q /usr/share/man/man1/afraid-dns.1
	@echo  
	@echo ------------------------------------------------------- 
	@echo  
	@echo Install completed. 
	@echo  
	@echo Edit the config file: $$ sudo nano /etc/afraid-dns.conf
	@echo To start manually: $$ sudo service afraid-dns start
	@echo To get more info, read the man page: $$ man afraid-dns
	@echo  

uninstall:
	sudo service afraid-dns stop
	sudo update-rc.d -f afraid-dns remove
	sudo rm /usr/sbin/afraid-dns /etc/init.d/afraid-dns /etc/afraid-dns.conf /usr/share/man/man1/afraid-dns.1.gz
