
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

#include "Socket.h"
#include <strings.h>
#include <cstring>
void Socket::SetupLocals(void) {
	char buf[128];
	if (gethostname(buf, sizeof(buf)) == -1) {
		myLocalHost = "-ERR-";
		myLocalIP="-ERR-";
		//throw Exception("SetupLocals","gethostname", (void *)this, TYPE_SOCKET);
	}  else {
		myLocalHost = string(buf);
	
		struct hostent *hp;
		hp = gethostbyname(myLocalHost.c_str());

		if (!hp) {
			myLocalIP="-ERR-";
		
	//	throw Exception("SetupLocals","Host Lookup Failure", (void *)this, TYPE_SOCKET);
		} else if (hp->h_addr_list[0]) {
			strncpy(buf, inet_ntoa( *(struct in_addr *) hp->h_addr_list[0]), 127);
			buf[sizeof(buf)-1]='\0';
			myLocalIP=string(buf);
		}
	}
}

void Socket::SetupFDS(void) {
		
	FD_ZERO(&myReadFDS);
	FD_ZERO(&myWriteFDS);
	FD_ZERO(&myExceptionFDS);
	
	FD_SET(mySocketID, &myReadFDS);
	FD_SET(mySocketID, &myWriteFDS);
	FD_SET(mySocketID, &myExceptionFDS);

}

void Socket::PutChar(char ch) {
	int wrote;
	char buf[5];
	sprintf(buf,"%c",ch);
	int retval;
	int done=0;
	
	SetupFDS();	
	wrote=0;
	time_t thirty;
	time_t now;
	time(&thirty);
	struct timeval tv;
	while (!done) {
		tv.tv_sec = 5;
		tv.tv_usec = 0;
		retval = select(getSocketID()+1, NULL, &myWriteFDS, &myExceptionFDS, &tv);
		if (retval==-1) throw Exception("Send","select returned -1", (void *)this, TYPE_SOCKET);
		if (FD_ISSET(getSocketID(), &myExceptionFDS)) throw Exception("Send","select exception", (void *)this, TYPE_SOCKET);
		else if (FD_ISSET(getSocketID(), &myWriteFDS)) {		
			while (wrote <= 0) {
				wrote=write(getSocketID(), buf, 1);
				if (wrote <= 0) throw Exception("Send","Wrote returned <= 0", (void *)this, TYPE_SOCKET);
				if (wrote==1) done=1;
			}
		}
		time(&now);
		if (difftime(thirty, now) > 180) // Time out after 3 minutes of trying to send this one packet
			throw Exception("Send", "Write timeout", (void *)this, TYPE_SOCKET);

	}
}

void Socket::Send(string s) {
	int wrote;
	unsigned int totwrote;
	string buf;

	SetupFDS();	
	while (s.length() > 0) {
		if (s.length() > 255) {
			buf=s.substr(0,255);
			s.erase(0,255);
		} else {
			buf=s;
			s="";
		}
		if (buf.length() > 0) {
			totwrote=0;
			wrote=0;
			time_t thirty;
			time_t now;
			time(&thirty);
			while(totwrote < buf.length()) {
				struct timeval tv;
				int retval;
				tv.tv_sec = 5;
				tv.tv_usec = 0;
				retval = select(getSocketID()+1, NULL, &myWriteFDS, &myExceptionFDS, &tv);
				if (retval==-1) throw Exception("Send","select returned -1", (void *)this, TYPE_SOCKET);
				if (FD_ISSET(getSocketID(), &myExceptionFDS)) throw Exception("Send","select exception", (void *)this, TYPE_SOCKET);
				else if (FD_ISSET(getSocketID(), &myWriteFDS)) {		
					wrote=write(getSocketID(), buf.c_str()+totwrote, buf.length()-totwrote);
					if (wrote <= 0) throw Exception("Send","Wrote returned <= 0", (void *)this, TYPE_SOCKET);
					totwrote += wrote;
				
				}
				time(&now);
				if (difftime(thirty, now) > 180) { // Time out after 3 minutes of trying to send this one packet
					throw Exception("Send", "Write timeout", (void *)this, TYPE_SOCKET);

				}	
			}
			
		}
	}
}

int Socket::GetChar(int mill, int secs) {
char buf[1];
struct timeval tv;
int retval=0;

		SetupFDS();
		bzero(&tv,sizeof(tv));
		tv.tv_sec = secs;
		tv.tv_usec = mill*1000;
		retval = select(mySocketID+1, &myReadFDS, NULL, &myExceptionFDS, &tv);
		if (retval == -1) throw Exception("GetChar", "select returned -1", (void *)this, TYPE_SOCKET);
	    	if (FD_ISSET(getSocketID(), &myExceptionFDS)) {
			throw Exception("GetChar","select() returned an exception", (void *)this, TYPE_SOCKET);
		} else if (FD_ISSET(getSocketID(), &myReadFDS)) {
			if (read(getSocketID(),buf,1) <= 0) {
				throw Exception("GetChar","Received <= 0 from read", (void *)this, TYPE_SOCKET);
			}
			return (int) (unsigned char)buf[0];
		}	
	return 0;
}

