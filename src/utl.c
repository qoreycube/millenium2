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

#include "vars.h"

void SaveUsersToFile(QUserRec *item, int &currNumber, void *obj) {
	UserFile.Write(item, currNumber);
}

bool operator<(const QUserRec &leftHand, QUserRec &rightHand) {
	string l(leftHand.loginname), r(rightHand.loginname);
	return ( l < r );
}

bool operator>(const QUserRec &leftHand, QUserRec &rightHand) {
	string l(leftHand.loginname), r(rightHand.loginname);
	return ( l > r );
}

bool operator==(const QUserRec &leftHand, QUserRec &rightHand) {
	string l(leftHand.loginname), r(rightHand.loginname);
	return ( l == r );
}


int GetLargestOnlineNode(void) {
int largest=0,i=0;
	while (i < connection.size()) {
		if (i==0) largest=connection[i]->getNodenum();
		else if (connection[i]->getNodenum() > largest) largest=connection[i]->getNodenum();
		if (connection[i]->getSec()->network == 1) {
			for (int j=0; j< connection[i]->Network.size(); j++) 
				if (connection[i]->Network[j]->Nodenum > largest) largest=connection[i]->Network[j]->Nodenum;
		}
		i++;
	}
	return largest;
}

int UserIsOnline(int usernum) {
int i=0;
	while (i < connection.size()) {
		if (connection[i]->getUser()->number == usernum) return i;
		i++;
	}
	return -1;
}

string GetLocation(int loc) {
	switch (loc) {
		case LOC_CHATROOM: 		return "|03C";
		case LOC_USER_EDITOR: 		return "|14UE";
		case LOC_USER_CONFIG: 		return "|15UC";
		case LOC_SECURITY_EDITOR: 	return "|14SE";
		case LOC_SYSTEM_EDITOR: 	return "|10SE";
		case LOC_LINK_EDITOR: 		return "|14LE";
		case LOC_MESSAGE_EDITOR: 	return "|14ME";
		case LOC_LOGIN: 		return "|10LG";
		case LOC_BBS:			return "|15BB";
		case LOC_GAMES: 		return "|15GM";
		case LOC_BAN_EDITOR: 		return "|14BE";
		default: return "|12OO";
	}
}

void Connection::whosonline(char flag) {
char s[81];
char ca[256];
QNodeDataRec nd;
int i=1, largestnode;
	LockMutex(&MUTEX_CONNECTION);
	largestnode=GetLargestOnlineNode();
	if (!ChatClient) {
		snpr("|15%s\r\n",Center("Who's Online?").c_str());
		line(LINE_TOP);
		if (flag == 1) {
	  		if (testbit(USERBIT_SHOWUID, getUser()->flags))
				npr("|07NNN |03C|11# |07Chat Handle      |12UID  |14Login@ |12A |15Idle|07   Doing\r\n");
			else 
				npr("|07NNN |03C|11# |07Chat Handle       |14Login@ |12A |15Idle|07   Doing\r\n");
				
		} else if (flag==2)
	  		npr(" |07NN |03C|11# |07Chat Handle      |15IP address       |12Host Name\r\n");
			
	}
	while (i <= largestnode) {
		if (GetDataByNode(i, &nd)) {
			if (ChatClient) {
				QInstantMessageRec Msg;
				Msg.msgType=MSG_LOGIN;
				Msg.chathandle=string(nd.User.chathandle);
				Msg.lc = nd.Sec.lc;
				Msg.rc = nd.Sec.rc;
				Msg.message="";
				Msg.nodeFrom=nd.Nodenum;
				Msg.channelFrom=nd.User.channel;
				npr(FormatClientMessage(&Msg));
				Msg.lc=' ';
				Msg.rc=' ';
				Msg.msgType=MSG_SETATTR;
				Msg.chathandle=IntToStr(nd.User.number,3);
				Msg.message=string(nd.User.loginname);
				npr(FormatClientMessage(&Msg));
				if (nd.Away) {
					Msg.lc = nd.Sec.lc;
					Msg.rc = nd.Sec.rc;
					Msg.chathandle=string(nd.User.chathandle);
					Msg.msgType=MSG_AWAY;
					Msg.message=nd.Awaymsg;
					npr(FormatClientMessage(&Msg));
				}
			} else {
				if (flag==1) {
					string outp;
					if (timeDifference(nd.Idle,now()) > 1200) {
						
						sprintf(s, "|07%03d:%s",i,stripcolors(GetLocation(nd.Location)).c_str());
						if (nd.Location == LOC_CHATROOM) sprintf(s,"%s%d",s,nd.User.channel);
						sprintf(s,"%s%c%s%c",s, nd.Sec.lc, nd.User.chathandle, nd.Sec.rc);
						if (testbit(USERBIT_SHOWUID, getUser()->flags))
							sprintf(ca, "|08%s %03d", spacecolors(stripcolors(string(s)),23, ALIGN_LEFT).c_str(), nd.User.number);
						else 
							sprintf(ca, "|08%s", spacecolors(stripcolors(string(s)),23, ALIGN_LEFT).c_str());
							
						sprintf(ca, "%s %s |12%c|08%s|07 %s\r\n", ca,spacecolors(getShortTime(nd.Logintime).c_str(), 7, ALIGN_RIGHT).c_str(),(nd.Away)?'*':' ',spacecolors(getTotalTimeShort(timeDifference(nd.Idle,now())), 7, ALIGN_LEFT).c_str(), nd.User.doing);
						outp=ca;
					
					} else {
						
						sprintf(s, "|07%03d:|03%s",i,GetLocation(nd.Location).c_str());
						if (nd.Location == LOC_CHATROOM) sprintf(s,"%s|11%d",s,nd.User.channel);
						sprintf(s,"%s|10%c|07%s|10%c",s, nd.Sec.lc, nd.User.chathandle, nd.Sec.rc);
						if (testbit(USERBIT_SHOWUID, getUser()->flags))
							sprintf(ca, "%s |12%03d", spacecolors(string(s),23, ALIGN_LEFT).c_str(), nd.User.number);
						else 
							sprintf(ca, "%s", spacecolors(string(s),23, ALIGN_LEFT).c_str());
						sprintf(ca, "%s |14%s |12%c|15%s|07 %s\r\n", ca,spacecolors(getShortTime(nd.Logintime).c_str(), 7, ALIGN_RIGHT).c_str(),(nd.Away)?'*':' ',spacecolors(getTotalTimeShort(timeDifference(nd.Idle,now())), 7, ALIGN_LEFT).c_str(), nd.User.doing);
						outp=ca;
					}
					npr(outp);
				} else if (flag==2) {
					string newstr= "|07" + IntToStr(i,3) + ":" + GetLocation(nd.Location);
					if (nd.Location == LOC_CHATROOM) newstr+="|11"+IntToStr(nd.User.channel);
					newstr += string("|10")+nd.Sec.lc+string("|07")+nd.User.chathandle+string("|10")+nd.Sec.rc;
					string res=spacecolors(newstr,23, ALIGN_LEFT);
					res += " |15" + spacecolors(nd.Ip,17, ALIGN_LEFT) +"|12"+ nd.Hostname + "\r\n";
					npr(res);
				}
			}
  		}
		i++;
	}
	UnlockMutex(&MUTEX_CONNECTION);
	
	if (!ChatClient) line(LINE_BOTTOM);
}

string Connection::Center(string s, unsigned int i) {
string Result = s;
while ( (i/2) > (stripcolors(Result).length() - (stripcolors(s).length()/2))) {
   Result = " " + Result; 
  }
return Result;
}

void Connection::line(int flag) {
if (flag == LINE_BOTTOM)
	snpr("|08-=-=-=-=-=-=-=-=-=-=-=-=-=-=|07-=-=|15-=-=-[ |10%s |15]=-=-=|07-=-=-|08=-=-\r\n|07", spacecolors(getDateTime(now()),6,ALIGN_RIGHT).c_str());
else
	npr("|08-=-=-=-=|07-=-=|15-=-=-=-=-=-=-=-=-=-=-=-=-|07=-=-|08=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n");
}


void securityname(int nodenum, char *s) {
QNodeDataRec nd;

  if (GetDataByNode(nodenum, &nd)) {

    strcpy(s,"|06(|15");
    if (nd.Guest) strcat(s,"Guest");
    else strcat(s,nd.Sec.name);
    strcat(s,"|06)");
  }
}

void Connection::login(void) {
  char s[181];
  string j;
  sysinfo.totallogins++;
  
  if (Finished) return;
  npr("\r\n");
  
  if (getSec()->network != 1) {
  	npr("\r\n");
  	printfile("./etc/motd");
  	if ((!getGuest()) && (getUser()->loginerrors)) {
  	  snpr("|04-|12=|15-|07 You have had |10%d|07 login errors since your last login\r\n",getUser()->loginerrors);
  	  getUser()->loginerrors=0;
  	  save_user(getUser());
  	}
  	sprintf(s,"./tmp/%s",getUser()->loginname);
  	if (exist(s)) {
  	  npr("You have some unread messages:\r\n");
  	  printfile(s);
  	  unlink(s);
  	}
  } else {
  	getUser()->ansi=0;
  	getUser()->echo=0;
	if (ClientLink==false) {
		string st;
		st=input(160);
		debug("-=- St = ("+ st + ")");
		debug("-=- st[0] == ("+st[0]+string(") ")+IntToStr(st[0]));
		if (st[0]==1) {
			debug("-=- Is an M2 Link, receiving VERINFO, HOST, PORT, HANDLE, NAME");
			myLink.type=LINK_M2;
			int mode, channelFrom, channelTo, nodeFrom, nodeTo;
			char lc, rc;
			string parameter, text, strMode;
			while (mode != MSG_END) {
				string hm;
				ParseM2Network(st,mode,strMode,nodeTo,nodeFrom, channelTo,channelFrom,lc,rc,parameter,text);
				switch (mode) {
					case M2MODE_INFO:
						   debug("-=- Got INFO, "+parameter+" = "+text);
						if (parameter == "HOST") strcpy(myLink.host,text.c_str());
						else if (parameter == "PORT") { myLink.port=atoi(text.c_str()); sprintf(getUser()->doing, "%s (%d)",myLink.host, myLink.port); }
						else if (parameter == "HANDLE") { strcpy(myLink.handle,text.c_str()); strcpy(getUser()->chathandle, myLink.handle); }
						else if (parameter == "NAME") { strcpy(myLink.name,text.c_str()); }
						else {
							hm="Received Param="+parameter+" text="+text;
							syslog(hm);
							debug(hm);
						}
						break;
					case MSG_END: 
						break;
					case M2MODE_VERINFO:
						hm="|10LINK: |15Millenium2 |03v|11" + parameter + " |07running on |15"+text+"|07.";
						SystemMessageAllBut(getNodenum(),hm);
						npr(FormatM2Message(M2MODE_VERINFO,1,-1,-1,-1,' ',' ',VERSION,OS));
									
						break;

					default: syslog("M2Mode link first message != VERINFO || INFO");
						 debug("M2Mode link first message != VERINFO || INFO");
						 break;
				}
				if (mode!=MSG_END) st=input(160);
			}
			npr(FormatM2Message(M2MODE_WHOSONLINE,-1,-1,-1,-1,' ',' ',"",""));
		} else if (st.find("\x03") != string::npos) {
			debug("-=- Is LINK_TC, getting VERINFO");
			myLink.type=LINK_TC;
			int mode, minormode;
			string parameter, text;
			ParseTCNetwork(st,mode,minormode, parameter, text);
			switch (mode) {
				case TCMODE_VERINFO: npr(FormatTCMessage(TCMODE_WHOSONLINE,0,"",""));
						     break;
				default: syslog("TCMode link first message != VERINFO");
					 debug("TCMode link first message != VERINFO");
					 break;
			}
			
		}
	}

  }
  getUser()->totallogins++;
  time(getLogintime());
  time(&getUser()->lastlogin);
  if (stripcolors(getUser()->chathandle).length() == 0) {
	npr("|15Well, well, well. Aren't you a clever little |10doof|15. Guess you'll have to\r\n");
	npr("change your handle to something READABLE\r\n");
	npr("New handle: ");
	j=input(50);
	npr("\r\n\n");
	if (stripcolors(j).length()==0) {
		npr("|12No? Alright. B'bye!$!@ B'bye now!#! B'bye!@!\r\n\n");
		logout(FLAG_LOGOUT);
	}
	if (trailingpipe(j)) j+='|';
	strcpy(getUser()->chathandle,j.c_str());
  }
  if (ChatClient) { 
  	  setLoggedin(true); 
  	  showmessage(getNodenum(), FLAG_LOGIN);
	  ListUsers();
	  whosonline();
	  ChatRoom();
  	  logout(FLAG_LOGOUT);
  } else {
 	if (testbit(USERBIT_LOGINCHAT, getUser()->flags) && (getSec()->network!=1)) {
		writeLocation(LOC_CHATROOM);
  		setLoggedin(true);
		showmessage(getNodenum(), FLAG_LOGIN);
 	}  
  	setLoggedin(true);
  	if ((ChannelList[getUser()->channel].getLocked()) && (!ChannelList[getUser()->channel].getPublic())) {
		npr(SYSS+"Attempted to login to channel |03C|11"+IntToStr(getUser()->channel)+"|07, but it was locked/private"+SYSF);
		getUser()->channel=1;
	}
	for (int z=0; z<=9; z++) {
  		if ((ChannelList[z].getLocked()) && (!ChannelList[z].getPublic())) {
			clearbit(z, getUser()->monitor);
		}
	}
	ChatRoom();
  }
}

void ForceMessage(string s) {
	QInstantMessageRec newMsg;
	if (s == "killed") {
		newMsg.priority=PRIORITY_FORCE;
		newMsg.message=s;
		newMsg.chathandle="";
		newMsg.msgType=MSG_DIE;
		newMsg.nodeTo=-1;
		newMsg.nodeFrom=-1;
		newMsg.lc=0;
		newMsg.rc=0;

	} else {
		newMsg.priority=PRIORITY_FORCE;
		newMsg.message=s;
		newMsg.chathandle="";
		newMsg.msgType=MSG_CHAT;
		newMsg.nodeTo=-1;
		newMsg.nodeFrom=-1;
		newMsg.lc=0;
		newMsg.rc=0;
	}
	Broadcast(newMsg); 
}

void RelogNode(int n) {
char s[10];
	int c=GetFirstConnectionByNode(n);
	if (c != -1) {
        	sprintf(s, "%c%c%c",1,2,3);
		QInstantMessageRec Msg;
		Msg.priority=PRIORITY_FORCE;
		Msg.message=s;
		Msg.chathandle="";
		Msg.msgType=MSG_RECEIVERELOG;
		Msg.lc=0;
		Msg.rc=0;
		Msg.nodeTo=n;
		Broadcast(Msg); 
	} else {
		debug("RelogNode: GetConnectionByNode() returned -1? :(");
	}
}

void KillNode(int n) {
char s[10];
	int c=GetFirstConnectionByNode(n);
	if (c != -1) {
        	sprintf(s, "%c%c%c",1,2,3);
		QInstantMessageRec Msg;
		Msg.priority=PRIORITY_FORCE;
		Msg.message=s;
		Msg.chathandle="";
		Msg.msgType=MSG_DIE;
		Msg.lc=0;
		Msg.rc=0;
		Msg.nodeTo=n;
		Broadcast(Msg); 
	} else {
		debug("KillNode: GetConnectionByNode() returned -1? :(");
	}
}

string Connection::chatheader(int networkNode) {
string s;
char s2[10];
QNodeDataRec nd;
int j;
  if (networkNode != -1) {
	int c=GetFirstConnectionByNode(networkNode);
	if (connection[c]->getNodenum() == networkNode) {
		nd=connection[c]->getNodeData();	
	} else {
		for (j=0; j<connection[c]->Network.size(); j++) 
			if (connection[c]->Network[j]->Nodenum == networkNode) {
				nd=*connection[c]->Network[j];
				break;
			}
		if (j==connection[c]->Network.size()) return "Uhoh?";
	}
	sprintf(s2,"%02d",nd.Nodenum);
	s="|08#" + string(s2) + "|07:|03C|11";
	sprintf(s2, "%d", nd.User.channel);
	s+=string(s2) + "|10";
	s+= nd.Sec.lc;
	s+="|07";
	s+=string(nd.User.chathandle);
	s+= "|10";
	s+=nd.Sec.rc;
	s+="|07";
	return s;
	
  } else {
	  sprintf(s2,"%02d",getNodenum());
	  s="|08#" + string(s2) + "|07:|03C|11";
	  sprintf(s2, "%d", getUser()->channel);
	  s+=string(s2) + "|10";
	  s+= getSec()->lc;
	  s+="|07";
	  s+=string(getUser()->chathandle);
	  s+= "|10";
	  s+=getSec()->rc;
	  s+="|07";
	  return s;
  }
}

int randomnumber(int n) {
	int d= (int)(1+(rand() % n));
return d;
}
void ReverseSelectionSort(vector<QSort> *myVec) {
	SelectionSort(myVec, false);

}
void SelectionSort(vector<QSort> *myVec, bool which) {
	for (unsigned int i=0; i<myVec->size(); i++) {
		unsigned int min=i;
		for (unsigned int j=i+1; j<myVec->size(); j++) {
			if (which) {
				if ((*myVec)[j].sortedItem < (*myVec)[min].sortedItem) {
					min=j;
				}
			} else {
				if ((*myVec)[j].sortedItem > (*myVec)[min].sortedItem) {
					min=j;
				}
			}
		}
		if (i != min) { 
			QSort T=(*myVec)[min];
			(*myVec)[min]=(*myVec)[i];
			(*myVec)[i]=T;
		}
	}
}

void ParseTCNetwork(string s, int &mode, int &minormode, string &parameter, string &text) {
	unsigned int sep;
	string temp;
	
	sep=s.find(":");
	if (sep==string::npos) return;
	temp=s.substr(0,sep);
	mode=atoi(temp.c_str());
	s.erase(0,sep+1);
	
	sep=s.find(" ");
	if (sep==string::npos) return;
	temp=s.substr(0,sep);
	minormode=atoi(temp.c_str());
	s.erase(0,sep+1);
	
	sep=s.find("\x03");
	if (sep==string::npos) return;
	temp=s.substr(0,sep);
	parameter=temp;
	s.erase(0,sep+1);
	text=s;
}

string FormatTCMessage(int mode, int minormode, string param, string text) {
char s[10];
string Result;

	sprintf(s,"%02d:",mode);
	Result = string(s);
	sprintf(s,"%02d ",minormode);
	Result += string(s) + param;
	Result += "\x03";
	Result += text + "\n";
	return Result;

}

string FormatM2Message(int mode, int nodeTo, int nodeFrom, int channelTo, int channelFrom, char lc, char rc, string parameter, string text) {
	string Result;
	char delim=4;
	char start=1;
	
	Result  = start + IntToStr(mode, 2) + delim + MsgTypeString(mode) + delim;
	Result += IntToStr(nodeTo);
	Result += delim;
	Result += IntToStr(nodeFrom);
	Result += delim;
	Result += IntToStr(channelTo);
	Result += delim;
	Result += IntToStr(channelFrom);
	Result += delim;
	if (lc == -1) Result += "  ";
	else {
		Result += lc;
		Result += rc;
	}
	Result += delim;
	Result += parameter;
	Result += delim;
	Result += text;
	Result += "\n";
	return Result;
}

bool ParseM2Network(string s, int &mode, string &strMode, int &nodeTo, int &nodeFrom, int &channelTo, int &channelFrom, char &lc, char &rc, string &parameter, string &text) {

	unsigned int sep;
	string temp;
	string delim("\x04");
	
	if (s[0]==1) s.erase(0,1);
	else return false;
	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	mode=atoi(temp.c_str());
	s.erase(0,sep+1);
	
	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	strMode=temp;
	s.erase(0,sep+1);
	
	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	nodeTo=atoi(temp.c_str());
	s.erase(0,sep+1);

	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	nodeFrom=atoi(temp.c_str());
	s.erase(0,sep+1);

	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	channelTo=atoi(temp.c_str());
	s.erase(0,sep+1);

	sep=s.find(delim);
	if (sep == string::npos) return false;
	temp=s.substr(0,sep);
	channelFrom=atoi(temp.c_str());
	s.erase(0,sep+1);

	sep=s.find(delim);
	if (sep == string::npos) return false;
	lc=s[0];
	rc=s[1];
	s.erase(0,sep+1);

	sep=s.find(delim);
	if (sep == string::npos) return false;
	parameter=s.substr(0,sep);
	s.erase(0,sep+1);

	text=s;
	return true;
}

void Connection::DoNetworkCommand(string s) {
QNodeDataRec *newNode;
QUserRec newUser;

	debug("NETWORK: "+ s);
	if (myLink.type == LINK_M2) {
		int mode, nodeTo, nodeRemote, channelTo, channelFrom;
		string strMode, parameter, text, Result;
		char lc, rc;
		int temp;		
		if (ParseM2Network(s, mode, strMode, nodeTo, nodeRemote, channelTo, channelFrom, lc, rc, parameter, text)) {
			if (mode < MSG_END) {
				for (int i=0; i<Network.size(); i++) {
					if (nodeRemote == -1) {
						QInstantMessageRec Msg;
						Msg.msgType=mode;
						Msg.message=text + " (From "+myLink.name+"|07).";
						Msg.chathandle=parameter;
						BroadcastAllBut(getNodenum(), Msg);

					} else if (Network[i]->RemoteNodenum == nodeRemote) {
						Network[i]->Idle=now();
						QInstantMessageRec Msg;
						Msg.chathandle=parameter;
						Msg.message=text;
						Msg.msgType=mode;
						switch(Msg.msgType) {
							case MSG_SYSTEM: 
							case MSG_SYSLOG: 
							case MSG_FORCE:
							case MSG_RETURNED:
							case MSG_LEFT: Msg.message += " |07(From |10" + string(myLink.name) + "|07)";
						}
						Msg.nodeTo=nodeTo;
						Msg.nodeFrom=Network[i]->Nodenum;
						Msg.channelTo=channelTo;
						Msg.channelFrom=channelFrom;
						Msg.lc = Network[i]->Sec.lc;
						Msg.rc = Network[i]->Sec.rc;
						switch (mode) {
							case MSG_PRIVATE: Msg.priority=PRIORITY_PRIVATE; break;
							case MSG_CHAT: Msg.priority=PRIORITY_CHAT; break;
							case MSG_SYSTEM: Msg.priority=PRIORITY_SYSTEM; break;
							case MSG_FORCE: Msg.priority=PRIORITY_FORCE; break;
							default: Msg.priority=PRIORITY_SYSTEM;
						}					
						BroadcastAllBut(getNodenum(), Msg);
						return;
					}
				}
			} else 
				switch(mode) {
				case M2MODE_PING:
					if (nodeTo == 0) {
						Result = FormatM2Message(M2MODE_PING,1,-1,-1,-1,-1,-1,"","");
						npr(Result);
					} else {
						// calculate ping time.
					}
					break;
				case M2MODE_VERINFO:
					if (nodeTo==1) {
						Result = "|10LINK: |15Millenium2 |03v|11" + parameter + "|07 running on |15"+text;
						SystemMessageAllBut(getNodenum(), Result);
					} else {
						Result = "|10LINK: |15Millenium2 |03v|11" + parameter + "|07 running on |15"+text;
						SystemMessageAllBut(getNodenum(), Result);
						Result = FormatM2Message(M2MODE_VERINFO, 1,-1,-1,-1,-1,-1,VERSION,OS);
						npr(Result);
						Result = FormatM2Message(M2MODE_WHOSONLINE, -1,-1,-1,-1,-1,-1,"","");
						npr(Result);
					}
					break;
				case M2MODE_WHOSONLINE: 
					for (int i=0; i<connection.size(); i++) {
						if (connection[i]->getNodenum() != getNodenum()) {
							if (connection[i]->getSec()->network==1) {
								for (int j=0; j<connection[i]->Network.size(); j++) {
									Result = FormatM2Message(M2MODE_LOGIN, -1, connection[i]->Network[j]->Nodenum, -1, connection[i]->Network[j]->User.channel, connection[i]->Network[j]->Sec.lc, connection[i]->Network[j]->Sec.rc, string(connection[i]->Network[j]->User.chathandle), "");
									
									npr(Result);
									if (connection[i]->Network[j]->Away) {
										Result = FormatM2Message(M2MODE_AWAY, -1, connection[i]->Network[j]->Nodenum, -1, connection[i]->Network[j]->User.channel, connection[i]->Network[j]->Sec.lc, connection[i]->Network[j]->Sec.rc, string(connection[i]->Network[j]->User.chathandle), string(connection[i]->Network[j]->Awaymsg));
										npr(Result);
									}
								}
							} else {
								Result = FormatM2Message(M2MODE_LOGIN, -1, connection[i]->getNodenum(), -1, connection[i]->getUser()->channel, connection[i]->getSec()->lc, connection[i]->getSec()->rc, string(connection[i]->getUser()->chathandle), "");
								npr(Result);
								if (connection[i]->getAway()) {
									Result = FormatM2Message(M2MODE_AWAY, -1, connection[i]->getNodenum(), -1, connection[i]->getUser()->channel, connection[i]->getSec()->lc, connection[i]->getSec()->rc, string(connection[i]->getUser()->chathandle), string(connection[i]->getAwaymsg()));
									npr(Result);
								}
							}
						}
					}
					break;
				case M2MODE_LOGIN:	
					int n;
					for (int i=0; i<Network.size(); i++) {
						if (nodeRemote == Network[i]->RemoteNodenum) { // Node already being used
							return;
						}
					}
					strcpy(newUser.loginname, "linkuser");
					LockMutex(&MUTEX_ACCESSUSERLIST);
					if (!UserList.Search(newUser)) { 
						SystemMessage("|04-|12=|15-|08 LINK Error, loginname 'linkuser' must exist in the user database");
						logout(FLAG_DISCONNECT);
					}	
					UnlockMutex(&MUTEX_ACCESSUSERLIST);
					newNode = new QNodeDataRec;
					newNode->User = newUser;
					newNode->User.channel=channelFrom;
					newNode->Logintime = now();
					newNode->Idle = now();
					newNode->Starttime=now();
					newNode->Nodenum = GetAvailableNode();
					newNode->User.number=0;
					newNode->Ip = myNodeData.Ip;
					newNode->Hostname = myNodeData.Hostname;
					sprintf(newNode->User.loginname,"%s%d",newNode->User.loginname, newNode->Nodenum);
					n=newNode->Nodenum;
					newNode->RemoteNodenum = nodeRemote;
					newNode->Away=false;
					newNode->Loggedin=true;
					newNode->Flags = 0;
					newNode->Awaymsg="";
					newNode->Away=false;
					newNode->IsBeingEdited=false;
					sprintf(newNode->User.doing, "|03-|11=|15- |07Linked - Hub %02d |15-|11=|03-", getNodenum());
					SecurityFile.Read(&newNode->Sec, newUser.security);
					newNode->Sec.lc = lc;
					newNode->Sec.rc = rc;
					strcpy(newNode->User.chathandle, parameter.c_str());
					Network.push_back(newNode);
					ShowMessageAllBut(n, FLAG_LOGIN, getNodenum());
					break;
				case M2MODE_RELOG:
				case M2MODE_KILLED:
				case M2MODE_DISCONNECT:
				case M2MODE_LOGOUT:
					for (int i=0; i < Network.size(); i++) {
						if (Network[i]->RemoteNodenum == nodeRemote) {
							switch (mode) {
								case M2MODE_RELOG: temp=FLAG_RELOG; break;
								case M2MODE_KILLED: temp=FLAG_KILLED; break;
								case M2MODE_LOGOUT: temp=FLAG_LOGOUT; break;
								case M2MODE_DISCONNECT: temp=FLAG_DISCONNECT; break;
							}
							ShowMessageAllBut(Network[i]->Nodenum, temp, getNodenum());
							Network.erase(i);
							break;
						}
					}
					break;
				default:
					string hm="M2 !DEFINED: Mode "+IntToStr(mode,2) + "(" + strMode + ")";
					debug(hm);
					syslog(hm);
				}
		} else {
				syslog("Bad LINK packet; "+s);
				string hm="BAD PACKET: "+s;
				syslog(hm);
				debug(hm);
		}
	} else if (myLink.type == LINK_TC) {
		int mode, minormode;
		string parameter, text;
		string Result;
		
		ParseTCNetwork(s, mode, minormode, parameter, text);	
	
		switch(mode) {
		case TCMODE_PING:
			Result = FormatTCMessage(TCMODE_PING,1,"","");
			npr(Result);
			break;
		case TCMODE_HANDLECHANGE:
			for (int i=0; i<Network.size(); i++) {
				if (Network[i]->RemoteNodenum == minormode) {
					Network[i]->Idle=now();
					strcpy(Network[i]->User.chathandle,parameter.c_str());
					return;
				}
			}
			break;
		case TCMODE_PRIVATE:
			for (int i=0; i<Network.size(); i++) {
				if (Network[i]->RemoteNodenum == minormode) {
					int LocalNode = atoi(text.substr(0, text.find(":")).c_str());
					Network[i]->Idle=now();
					text.erase(0, text.find(":")+1);
					QInstantMessageRec Msg;
					Msg.chathandle=Network[i]->User.chathandle;
					Msg.nodeFrom = Network[i]->Nodenum;
					Msg.priority= PRIORITY_PRIVATE;
					Msg.msgType= MSG_PRIVATE;
					Msg.lc = Network[i]->Sec.lc;
					Msg.channelFrom=Network[i]->User.channel;
					Msg.rc = Network[i]->Sec.rc; 
				        Msg.nodeTo=LocalNode;
					Msg.message = text;
					Broadcast(Msg);
					return;
				}
			}
			break;
		case TCMODE_VERINFO:
			Result = FormatTCMessage(TCMODE_WHOSONLINE, 0, "", "");
			npr(Result);
		//	npr(Result);
			break;
		case TCMODE_CHAT:
			for (int i=0; i<Network.size(); i++) {
				if (Network[i]->RemoteNodenum == minormode) {
					string hm;
					QInstantMessageRec newMsg;
					
					if (string(Network[i]->User.chathandle) != parameter) strcpy(Network[i]->User.chathandle,parameter.c_str());
					
					newMsg.chathandle = string(Network[i]->User.chathandle);
					newMsg.message=text;
					newMsg.lc = Network[i]->Sec.lc;
					newMsg.rc = Network[i]->Sec.rc;
					newMsg.msgType = MSG_CHAT;
					newMsg.nodeFrom = Network[i]->Nodenum;
					newMsg.priority=PRIORITY_CHAT;
					newMsg.channelFrom=Network[i]->User.channel;
					newMsg.channelTo=Network[i]->User.channel;
					
					time(&Network[i]->Idle);
					
					BroadcastAllBut(getNodenum(), newMsg);
					return;
				}
			} 
			break;
		case TCMODE_WHOSONLINE1: 
		case TCMODE_WHOSONLINE: 
			for (int i=0; i<connection.size(); i++) {
				if (connection[i]->getNodenum() != getNodenum()) {
					if (connection[i]->getSec()->network==1) {
						for (int j=0; j<connection[i]->Network.size(); j++) {
							Result = FormatTCMessage(TCMODE_LOGIN, connection[i]->Network[j]->Nodenum, string(connection[i]->Network[j]->User.chathandle), "");
							npr(Result);
						}
					} else {
						Result = FormatTCMessage(TCMODE_LOGIN, connection[i]->getNodenum(), string(connection[i]->getUser()->chathandle), "");
						npr(Result);

					}
				}
			}
			break;

		case TCMODE_LOGIN:	
			int n;
			for (int i=0; i<Network.size(); i++) {
				if (minormode == Network[i]->RemoteNodenum) { // Node already being used
					return;
				}
			}
			strcpy(newUser.loginname, "linkuser");
			LockMutex(&MUTEX_ACCESSUSERLIST);
			if (!UserList.Search(newUser)) { 
				SystemMessage("|04-|12=|15-|08 LINK Error, loginname 'linkuser' must exist in the user database");
				logout(FLAG_DISCONNECT);
			}	
			UnlockMutex(&MUTEX_ACCESSUSERLIST);
			newNode = new QNodeDataRec;
			newNode->User = newUser;
			newNode->Logintime = now();
			newNode->Idle = now();
			newNode->Starttime=now();
			newNode->Nodenum = GetAvailableNode();
			n=newNode->Nodenum;
			newNode->RemoteNodenum = minormode;
			newNode->Away=false;
			newNode->Ip = myNodeData.Ip;
			newNode->Hostname= myNodeData.Hostname;
			newNode->Loggedin=true;
			newNode->Flags = 0;
			newNode->Awaymsg="";
			newNode->Away=false;
			newNode->IsBeingEdited=false;
			newNode->User.monitor=0;
			sprintf(newNode->User.doing, "|03-|11=|15- |07Linked - Hub %02d |15-|11=|03-", getNodenum());
			SecurityFile.Read(&newNode->Sec, newUser.security);
			strcpy(newNode->User.chathandle, parameter.c_str());
			Network.push_back(newNode);
			ShowMessageAllBut(n, FLAG_LOGIN, getNodenum());
			break;
		case TCMODE_LOGOUT:
			for (int i=0; i < Network.size(); i++) {
				if (Network[i]->RemoteNodenum == minormode) {
					ShowMessageAllBut(Network[i]->Nodenum, FLAG_LOGOUT, getNodenum());
					//QNodeDataRec *ndr=Network[i];   // memory leak
					Network.erase(i);
					//delete ndr;
					break;
				}
			}
			break;
		default:
			string hm;
			hm="TC !DEFINED: Mode "+IntToStr(mode,2);
			debug(hm);
			syslog(hm);
		}
	}
}

int Connection::MenuKey(string haystack) {
bool done=false;
char ch;

	while ((!done) && (!Finished)) {
		string needle;

		if (testbit(USERBIT_HOTKEYS, getUser()->flags)) 
			needle += OneKey(haystack);
		else
			needle=input(1);
		
		ch = needle[0];
		if (toUpper(haystack).find(toUpper(needle)) != string::npos) {
			if (testbit(USERBIT_HOTKEYS, getUser()->flags))
				snpr("%c",ch); 
			npr("\r\n");
			done=true;
		} else if (ch==8) done=true;
		
		if (!testbit(USERBIT_HOTKEYS, getUser()->flags))
			npr("\b \b");
	}
//	pause();
	return toupper(ch);
}

int Connection::OneKey(string haystack) {
bool done=false;
char ch;
	while ((!done) && (!Finished)) {
		ch=getkey();
		string needle;
		needle += toupper(ch);
		if (ch==127) ch=8;
		if (toUpper(haystack).find(needle) != string::npos) {
			snpr("%c",ch); 
			done=true;
		} else if (ch==8) done=true;
	}
	return toupper(ch);
}

string StringLinkType(int type) {
	switch (type) {
		case LINK_M2: return "Millenium2";
		case LINK_TC: return "TinyChat";
		case LINK_DDIAL: return "DDial";
		case LINK_STS: return "STS";
		default: return "CHANGE";
	}
}

int parseString(string olds, int *n, string *msg, int mode) {
unsigned int i;
string snum;
string s=olds;
  s.erase(0,2); 
  if (mode == PARSE_STRING) {
    while ((s[0]==' ') && (s.length() > 0)) {
    	s.erase(0,1);
    }
    if (s.length() == 0) return PARSE_ERROR; 
    *msg = s;
    return PARSE_STRING;

  } else if (mode == PARSE_NODE) {
   while ((s.length() > 0) && (s[0]==' ')) { 
     s.erase(0,1);
   }
    if (s.length() == 0) return PARSE_ERROR;
    else {
      i=0;
      while (i < s.length()) {
	if (!isdigit(s[i])) return PARSE_ERROR;
        i++;
      }
      *n = (atoi(s.c_str()) < 0)?0:atoi(s.c_str());
      if (*n > 9999) *n=9999;
      return PARSE_NODE;
    }
  } else if (mode == PARSE_NODESTRING) {
    i=0;
    while ((s[0]==' ') && (s.length() > 0)) {
    	s.erase(0,1);
    } 
    if (!isdigit(s[0])) return PARSE_ERROR;
    snum = "";
    while ((s.length() > 0) && (isdigit(s[0]))) {
	    snum += s[i];
	    s.erase(0,1);
    }
      *n = (atoi(snum.c_str()) < 0)?0:atoi(snum.c_str());
      if (*n > 9999) *n=9999;
      
    if (s.length() == 0) return PARSE_NODE;
    while ((s[0]==' ') && (s.length() > 0)) {
	 s.erase(0,1);
    }
    if (s.length() == 0) return PARSE_NODE;
    *msg = s;
    return PARSE_NODESTRING;
  }
  return 0;
}


void ConvertToModifiedVector(QUserRec *usr, int &usernum, void *ptr) {
	vector<QModifiedUserRec> *UserListEditor = (vector<QModifiedUserRec> *)ptr;
	
	QModifiedUserRec newUser;
	newUser.user=*usr;
	newUser.modified=false;
	newUser.loginname = string(usr->loginname);
	UserListEditor->push_back(newUser);
}

void ConvertToVector(QUserRec *usr, int &usernum, void *ptr) {
	vector<QUserRec> *UserListEditor = (vector<QUserRec> *)ptr;
	UserListEditor->push_back(*usr);	
}

void Connection::pause(void) {
string s;

  npr("\r\n|04-|12=|15- |07Pause |04-|12=|15-|07\r");
  s=input(1);
  npr("             \r");

}

int QStringList::SaveToFile(string fn) {
FILE *out;

	if ((out=fopen(fn.c_str(), "wt"))==NULL) return 0;
	for (int i=0; i< myStrings.size(); i++) {
		string s=myStrings[i];
		s+='\n';
		fputs(s.c_str(), out);
	}
	fflush(out);
	fclose(out);
	return 1;
}

int QStringList::LoadFromFile(string fn) {
FILE *in;
bool Zeof=false;
	if ((in=fopen(fn.c_str(), "rt")) == NULL) return 0;
	for (;;) {
		
		if (feof(in)) break;
		char s[20];
		memset(s,0,20);
		string res;
		do {
			if ((fgets(s, 20,in)) == 0) {
				if (!feof(in))  {
   					debug("fgets() returned 0 and ! feof");
					return 0;
				} else {
					if (s[0] != 0) res += s;
					Zeof=true;				
					break;
				}
			} 
			if (!feof(in)) res += s;
		//debug("res=%s", res.c_str());
		} while (s[strlen(s)-1] != 10);
		if (Zeof) { 
			if (s[0]==0) return 1;
			else Add(res);
			return 1;
		}
		res.erase(res.length()-1,1);
		Add(res);
		s[0]=0;
	}
	fclose(in);
	return 1;
}
