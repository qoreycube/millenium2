
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

#ifndef SOCKET_H
#define SOCKET_H

#include "Lockable.h"
#include <sys/socket.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;

class Exception {
	
	protected:
	
		string myFunction, myString;
		void *myFrom;
		int myType;
	public: 
		Exception(string fn,string s, void *from = NULL, int type=0) {
			myFunction=fn;
			myString=s;
			myFrom=from;
			myType=type;
		}
		int getType(void) { return myType; }
		void *getFrom(void) { return myFrom; }
		string toString(void) { return "(" + myFunction + ") " + myString; }
};

const int TYPE_SOCKET = 1;

class Socket : public Lockable {

	protected: 
		// private/protected variables

		int mySocketID;
		bool myClose;
		string myLocalIP, myLocalHost;
		string myRemoteIP, myRemoteHost;
		unsigned short myLocalPort, myRemotePort;

		fd_set myReadFDS, myWriteFDS, myExceptionFDS;
		
		//private/protected functions
		
		void SetupFDS(void);
		void SetupLocals(void);
		
	public:
		
		//public variables
		
		    // a four byte storage block for the user to freely use
		void *Data;
		
		//public functions
		Socket(int sID, bool closeme=true) {
			mySocketID=sID;
			SetupLocals();
			myClose=closeme;
			//cout << "Socket(int)" << endl;
		}
		
		Socket() {
			//Lockable::Lockable();
			mySocketID = socket(AF_INET, SOCK_STREAM, 0);
			if (mySocketID == -1) throw Exception("Socket","socket returned -1", (void *)this, TYPE_SOCKET);
			SetupLocals();
			//cout << "Socket()" << endl;
		}
		
		~Socket() {
			if (mySocketID != -1) 
				if (myClose)
					try {
						Close();
					} catch (Exception e) {
						throw e;
					}
//			cerr << "~Socket()" << endl;
		}
		
		void setRemoteIP(string s) { myRemoteIP=s; }
		void setRemoteHost(string s) { myRemoteHost=s; }
		string getRemoteHost(void) { return myRemoteHost; }
		string getRemoteIP(void) { return myRemoteIP; }
		
		
		void Close(void) {
			if (mySocketID == -1) throw Exception("Socket::Close","No socket opened",(void *)this, TYPE_SOCKET);
			if (close(mySocketID) == -1) throw Exception("Socket::Close","close() returned -1", (void *)this, TYPE_SOCKET);
			mySocketID = -1;
		}

		int getSocketID(void) { return mySocketID; }
		
		void setSocketID(int id) { 
			if (mySocketID != -1) throw Exception("setSocketID","Socket already has an ID", (void *)this, TYPE_SOCKET);
			mySocketID=id; 
			SetupFDS();
		 }
		
		void Send(string s);
		void PutChar(char ch);
		// Get a character from the socket, timeout in (mill =milliseconds, secs = seconds)
		int GetChar(int mill, int secs=0);

		// Close the socket, if it is connected to one
		
		//public accessors
		string getLocalIP(void) { return myLocalIP; }
		string getLocalHost(void) { return myLocalHost; }
		
};

#endif
