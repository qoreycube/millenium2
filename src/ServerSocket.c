
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

#include "ServerSocket.h"
#include<cstring>
void ServerSocket::Listen(unsigned short  port) {
  struct linger mylinger = { 0 };
  struct sockaddr_in adr_srvr;
  int sopt=1, len_inet;
	if (myListening)
		throw Exception("Listen", "Already listening", (void *)this);

	
	if (setsockopt(mySocketID,SOL_SOCKET,SO_REUSEADDR,&sopt,sizeof(sopt))) throw Exception("Listen","setsockopt(REUSEADDR)", (void *)this, TYPE_SERVERSOCKET);
  

	mylinger.l_onoff=1;
	mylinger.l_linger=30;

	if (setsockopt(mySocketID,SOL_SOCKET,SO_LINGER,&mylinger,sizeof(struct linger))) throw Exception("Listen","setsockopt(LINGER)", (void *)this, TYPE_SERVERSOCKET);
  
	bzero(&adr_srvr, sizeof(sockaddr_in));
	adr_srvr.sin_family = AF_INET;
	adr_srvr.sin_addr.s_addr = htonl(INADDR_ANY);
	adr_srvr.sin_port = htons(port);
	len_inet=sizeof adr_srvr; 
	if (bind(mySocketID, (struct sockaddr *) &adr_srvr, sizeof(adr_srvr))<0) throw Exception("Listen","bind() returned < 0", (void *)this, TYPE_SERVERSOCKET);

	listen(mySocketID, 10);
	
	myListenPort=port;
	myListening=true;	
}

bool ServerSocket::CheckForConnection(int mili, int secs) {
	struct timeval tv;
	int retval;
	SetupFDS();
	tv.tv_sec = secs;
	tv.tv_usec = mili*1000;
	
	retval = select(getSocketID()+1, &myReadFDS, NULL, &myExceptionFDS, &tv);
	if (retval == -1) throw Exception("CheckForConnection", "select returned -1", (void *)this, TYPE_SERVERSOCKET);
    	if (FD_ISSET(getSocketID(), &myExceptionFDS)) {
		throw Exception("CheckForConnection","select() returned an exception", (void *)this, TYPE_SERVERSOCKET);
	} else if (FD_ISSET(getSocketID(), &myReadFDS)) {
		return true;
	}	
	return false;
}

Socket &ServerSocket::Accept(void) {
	int newSocket;
	Socket *sptr;
	struct sockaddr_in cliAddr;
	socklen_t len_inet = sizeof(sockaddr_in);
	struct hostent *hp;
	newSocket=accept(getSocketID(), (struct sockaddr *) &cliAddr, &len_inet);
	

	if (newSocket < 0) throw Exception("Accept","accept() returned -1", (void *)this, TYPE_SERVERSOCKET);
	
	_Lock();	
		AcceptedSocket *newSock = new AcceptedSocket(this, newSocket);
		char ip[61];
		
		strncpy(ip, inet_ntoa(cliAddr.sin_addr),60);
		newSock->setIP(string(ip));
		hp = gethostbyaddr((char *)&cliAddr.sin_addr, sizeof cliAddr.sin_addr, cliAddr.sin_family);
		if (!hp) {
			newSock->setHost(string(ip));
		} else {
			strncpy(ip,hp->h_name, 60);
			newSock->setHost(ip);
		}
		unsigned int i=mySockets.size();
		mySockets.push_back(newSock);
		Socket *intSocket = &mySockets[i]->getSocket();
	_Unlock();
	if (mySockets.size() > myMaxClients) {
		intSocket->Send("Maximum Clients Exceeded. Try back later...");
		intSocket->Close();
		Erase(intSocket);
		throw Exception("Accept","Maximum Clients Exceeded", (void *)this, TYPE_SERVERSOCKET);
	}
	return *intSocket;
}

