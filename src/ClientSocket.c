
/*
 * Millenium2 Chat Server Software for Unix Operating Systems
 * Copyright (c) 2002  Cory Kratz
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */


#include "ClientSocket.h"
#include<strings.h>
int ClientSocket::Connect(string hostname, unsigned short port) {
	
	struct sockaddr_in adr_srvr;
	struct hostent *hp;
	int len_inet;
	int s;

	if (connected==true) throw Exception("Connect","Already connected", (void *)this);
	bzero(&adr_srvr, sizeof(sockaddr_in));

	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_port = htons(port);
	myPort=port;

	hp=gethostbyname(hostname.c_str());
	if  (hp == NULL) throw Exception("Connect", "Hostname lookup failure", (void *)this);
	
	adr_srvr.sin_addr = *((struct in_addr *)hp->h_addr);
        if (adr_srvr.sin_addr.s_addr == INADDR_NONE) {
		throw Exception("Connect","bad internet address", (void *)this);
                return -1;
        }
        len_inet = sizeof adr_srvr;

	if(connect(getSocketID(),(sockaddr*)&adr_srvr, len_inet) == -1)
		throw Exception("Connect","connect returned -1", (void *)this);
	connected=true;
	return 1;
}
