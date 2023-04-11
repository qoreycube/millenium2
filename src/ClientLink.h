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

#ifndef CLIENTLINK_H
#define CLIENTLINK_H

#ifndef THREAD_H
#include "thread.h"
#endif
#ifndef CONNECTION_H
#include "Connection.h"
#endif

class ClientLinkThread : public Thread {
		public:
			bool myFinished;
	virtual void Execute(void *arg) {
		myFinished=false;
		ClientSocket myClient(false);
		string temp;
		QLinkRec *LinkInfo = (QLinkRec *)arg;
		
		try {
			if (myClient.Connect(LinkInfo->host, LinkInfo->port) == -1) {
				temp="Link ("+string(LinkInfo->host)+") -> Could not connect to remote server "; 	
				SystemMessage(temp);
				myFinished=true; 
				return;
			} 
			temp="Link ("+string(LinkInfo->host)+") -> Connection Accepted";
			SystemMessage(temp);
		
		} catch (Exception &e) {
			SystemMessage(e.toString());
			myFinished=true;
			return;
		}
		
		debug("Connected...\r\n");	
		
		string line="";
		bool loggedinflag=false, ansi=false;
		char newbuf[50];
		if (LinkInfo->type==LINK_TC) {
			while (1) {
				char ch;
				try { 
					while ((ch = myClient.GetChar(0,5) )== 0);
				} catch (Exception &e) {
					temp="Link ("+string(LinkInfo->host)+") -> Connection Lost";
					SystemMessage(temp);
					SystemMessage(e.toString());
					myFinished=true;
					return;
				}
				if (loggedinflag==true) {
					if (line.find("Welcome to") != string::npos) {
						if (line[line.length()-1] == 13) {
							line="";
							Connection *newConnection=new Connection(new Socket(myClient.getSocketID()), *LinkInfo);
							int size=connection.size();
							debug("Size="+IntToStr(size));
							connection.push_back(newConnection);
							debug("Created new connection, pushed it back");
							newConnection->Start(NULL);
							debug("Started");
							myFinished=true;
							return;
						}
					}
				}
				line += ch;
				if ((line.find("login") != string::npos) && (ansi==false)) {
					sleep(1);
					ansi=true;
					sprintf(newbuf,"ANSI%c",13);
					try {
						myClient.Send(string(newbuf));
					} catch (Exception &e) {
						SystemMessage(e.toString());
						temp="Connection (" +string(LinkInfo->host) +") Lost";
						SystemMessage(temp);
						myFinished=true;
					}
					line="";
					temp="Link (" +string(LinkInfo->host) +") -> Sent disable ansi";
					SystemMessage(temp);
					debug(temp);
				} else if ((line.find("login") != string::npos) && (ansi==true)) {
					sleep(1);
					sprintf(newbuf,"%s%c",LinkInfo->loginname,13);
					try {
						myClient.Send(string(newbuf));
					} catch (Exception &e) {
						SystemMessage(e.toString());
						temp="Connection (" +string(LinkInfo->host) +") Lost";
						SystemMessage(temp);
						myFinished=true;
					}
					line="";
					temp="Link (" +string(LinkInfo->host)+ ") -> Sent login";
					SystemMessage(temp);
					debug(temp);
				} else if ((line.find("assword") != string::npos) && (loggedinflag==false)) {
					sleep(1);
					sprintf(newbuf,"%s%c",LinkInfo->password,13);
					try {
						myClient.Send(string(newbuf));
					} catch (Exception &e) {
						SystemMessage(e.toString());
						temp="Connection (" +string(LinkInfo->host) +") Lost";
						SystemMessage(temp);
						myFinished=true;
					}
					loggedinflag=true;
					line="";
					temp="Link ("+string(LinkInfo->host)+") -> Sent Password";
					SystemMessage(temp);
					debug(temp);
				}
			}
		} else if (LinkInfo->type == LINK_M2) {
			while (1) {
				char ch;
				try { 
					while ((ch = myClient.GetChar(0,5) )== 0);
				} catch (Exception &e) {
					temp="Link ("+string(LinkInfo->host)+") -> Connection Lost";
					SystemMessage(temp);
					SystemMessage(e.toString());
					myFinished=true;
					return;
				}
				
				if (loggedinflag==true) {
					if (ch==13) {
						Connection *newConnection=new Connection(new Socket(myClient.getSocketID()), *LinkInfo);
						int size=connection.size();
						debug("Size="+IntToStr(size));
						connection.push_back(newConnection);
						debug("Created new connection, pushed it back");
						newConnection->Start(NULL);
						debug("Started");
						myFinished=true;
						return;
					}
				}
				line += ch;
				if ((line.find("login") != string::npos) && (ansi==false)) {
					sleep(1);
					ansi=true;
					sprintf(newbuf,"%s%c",LinkInfo->loginname,13);
					try {
						myClient.Send(string(newbuf));
					} catch (Exception &e) {
						SystemMessage(e.toString());
						temp="Connection (" +string(LinkInfo->host) +") Lost";
						SystemMessage(temp);
						myFinished=true;
					}

					//status=write(TCSocket,newbuf, strlen(newbuf));
					line="";
					temp="Link ("+string(LinkInfo->host)+") -> Sent Login";
					SystemMessage(temp);
					debug(temp);
				} else if ((line.find("assword") != string::npos) && (loggedinflag==false)) {
					sleep(1);
					sprintf(newbuf,"%s%c",LinkInfo->password,13);
					try {
						myClient.Send(string(newbuf));
					} catch (Exception &e) {
						SystemMessage(e.toString());
						temp="Connection (" +string(LinkInfo->host) +") Lost";
						SystemMessage(temp);
						myFinished=true;
					}
				//	status=write(TCSocket,newbuf, strlen(newbuf));
					loggedinflag=true;
					line="";
					temp="Link ("+string(LinkInfo->host)+") -> Sent Password";
					SystemMessage(temp);
					debug(temp);
				}
			}
		}
	}
};

#endif
