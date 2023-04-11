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
#include <stdarg.h>

// This one is public 
void Connection::Snpr(char *fmt, ...) {
va_list ap; 
char s[512]; 

  va_start(ap, fmt);
  vsprintf(s, fmt, ap);
  va_end(ap);
 npr(string(s));


}

void Connection::snpr(char *fmt, ...) {
  va_list ap; 
  char s[512];

  va_start(ap, fmt);
  vsprintf(s, fmt, ap);
  va_end(ap);

 npr(string(s));
 
}

void Connection::cls(void) {
  if (getUser()->ansi)  
	  npr("\r\n\n");
	  //snpr("%c[2J%c[0;0H",27,27);
  else {
    npr("\r\n\n");
    npr("\0xC");
    
  }
}

int Connection::yn(string s) {
string s2;
char ch;

do {
  snpr("|07%s |08[|15Yn|08] |07",s.c_str());

  ch=OneKey("YN\r");
  switch (toupper(ch)) {
    case '\r':
    case 'Y' : npr("es\r\n"); return(1);
    case 'N' : npr("o\r\n"); return(0);
  }
} while (!Finished);
return 0;
}

int Connection::ny(string s) {
string s2;
char ch;

do {
  snpr("|07%s |08[|15yN|08] |07",s.c_str());
  
  ch=OneKey("YN\r"); 
  switch (toupper(ch)) {
    case 'Y': npr("es\r\n"); return(1);
    case '\r':
    case 'N' : npr("o\r\n"); return(0);
  }
} while (!Finished);

return 0;
}


void Connection::npr(string message) {
string s;
char num[10]; 
int pipeflag=0, slen,i,wrote, totwrote;
slen=message.length();

s="";
if ((getSec()->network == 1) || (ChatClient))
	s=message;
else if (getUser()->ansi == false) {
	if (message.length() > 2) 
		s=stripcolors(message);
	else s=message;
} else {
	if (message.find("|") != string::npos) {
		for (i=0; i<slen; i++) {
cont:
			if (pipeflag==2) { 
				if (!isdigit(message[i])) {
					s+= "|" + string(num);
					pipeflag=0;
					goto cont;
				} else {	
	        			pipeflag=0; 
	        			sprintf(num,"%s%c",num,message[i]); 
	        			if (!strcmp(num,"01")) s += "\x1B[0;34m";
	        			else if (!strcmp(num,"02")) s += "\x1B[0;32m";
	        			else if (!strcmp(num,"03")) s += "\x1B[0;36m";
	        			else if (!strcmp(num,"04")) s += "\x1B[0;31m";
	        			else if (!strcmp(num,"05")) s += "\x1B[0;35m";
				        else if (!strcmp(num,"06")) s += "\x1B[0;33m";
				        else if (!strcmp(num,"07")) s += "\x1B[0;37m";
				        else if (!strcmp(num,"08")) s += "\x1B[1;30m";
				        else if (!strcmp(num,"09")) s += "\x1B[1;34m";
				        else if (!strcmp(num,"10")) s += "\x1B[1;32m";
				        else if (!strcmp(num,"11")) s += "\x1B[1;36m";
				        else if (!strcmp(num,"12")) s += "\x1B[1;31m";
				        else if (!strcmp(num,"13")) s += "\x1B[1;35m";
				        else if (!strcmp(num,"14")) s += "\x1B[1;33m";
				        else if (!strcmp(num,"15")) s += "\x1B[1;37m";
				        else if ((num[0]=='b') || (num[0]=='B')) {
						 // background colors go here :)
				        } else {
						s += string("|")+num;
					}
				}
			} else if (pipeflag==1) { 
				pipeflag++; 
				if (message[i]=='|') {
					pipeflag=0;
					s+='|';
				} else if (!isdigit(message[i])) {
					s +="|";
					pipeflag=0;
					goto cont;
				} else {
					sprintf(num,"%c",message[i]); 
				}
			} else if (message[i]=='|') { 
					pipeflag++;
			} else { 
					s += message[i];
			}
		}

		if (pipeflag>0) {
			s += '|';
			if (pipeflag==2) s += num;
		}
	} else s=message;
}

slen=s.length();
if (s.length() == 0) {
	debug("("+IntToStr(getNodenum())+") Nothing to write...");
	return;
}
if (slen==0){ 
	debug("slen==0");
	return;
}
	try {
		if (!Finished) (*mySocketStream) << s;
	} catch (Exception e) {
		debug(e.toString());
		logout(FLAG_LOGOUT);
	}
}

char Connection::getkeyc(void) {
int buf;
int retval=0;

	while (!Finished) {
		if (allowMsgs) {
			if (!msg.empty()) {
				checkForMessages();
			}
		}
		try {
			if ((buf=mySocketStream->GetChar(500, 0)) != 0) {	
				if (!getAway()) myNodeData.Idle=now();

				if ((buf=='\n') && (enterflag == '\r')) {
					enterflag=0;
				} else if ((buf=='\r') && (enterflag=='\n')) {
					enterflag=0;
				} else if (buf=='\r') { 
					enterflag='\r';
					return '\r';
				} else if (buf=='\n') { 
					enterflag='\n'; 
					return '\n'; 
				} else { 
					enterflag=0; 
					return buf; 
				}
		    	}
		} catch (Exception e) {
			debug(IntToStr(getNodenum()) + ") " +e.toString());
			//perror("What?! ");
			logout(FLAG_DISCONNECT);
		}
		if ((timeDifference(myNodeData.Starttime, now()) > 120) && ((!getLoggedin())) && (Finished==0)) {
			npr("\r\n\n|04-|12=|15- |07You must log in before two minutes\r\n\n");
			logout(FLAG_DISCONNECT);
			return 0;
		}
	}
	return 0;
}

int Connection::getkey(void) {
  char ch;
  ch=getkeyc();
  return (int)ch;
}

string Connection::input(int maxlen) {
  return (inputfunc(maxlen, FLAG_INPUT_NORMAL, ' '));
}

string Connection::inputm(int maxlen, char mask) {
  return(inputfunc(maxlen, FLAG_INPUT_MASKED, mask));
}
string Connection::fxinput(int maxlen) {
 return (inputfunc(maxlen, FLAG_INPUT_FX, ' '));

}

string Connection::inputfunc(int maxlen, int flag, char mask, bool wrap, string add) {
int ch,curpos=0,done=0, len;
char chs[21];
string result="";

if (flag == FLAG_INPUT_FX) {
  npr("|08[|10");
	for (len=0; len < maxlen; len++)
		npr(".");
  npr("|15]");
	for (len=0; len < (maxlen)+1; len++)
                npr("\b");
  npr("|07");
}
if (wrap) {
	int i;
	for (i=0; i<add.size(); i++) {
		string newstr=""+add[i];
		npr(newstr);
		result+=add[i];
		if (result.length() > maxlen) {
			result.erase(result.size()-1,1);
			add.erase(0,i-1);
			return result;
		}
	}
	curpos=i;
	setFlag(NODEBIT_TYPING);
}

while ((!done) && (!Finished)) {
  if (curpos == 0) clearFlag(NODEBIT_TYPING);
  else if (getUser()->echo) setFlag(NODEBIT_TYPING);
  ch=getkey();
  switch((unsigned char) ch) {
    case 127:
    case   8: 
	     if (curpos > 0) {
		if ((result.length()-1) == 0) result = "";
		else result= result.substr(0,result.length()-1);
                sprintf(chs,"%c%s%c",8,(flag==FLAG_INPUT_FX)?"|10.|07":" ",8);
                npr(chs);
                curpos--;
             }
             break;
    case 0:  break;
    case 10:
    case 13: 
             done=1; 
             clearFlag(NODEBIT_TYPING);
	     if (flag==FLAG_INPUT_FX) npr("|07");
             break;
    default: sprintf(chs,"%c",ch);
	     if (getSec()->network==1) {
		 if ((strchr(ALLOWED_CHARACTERS,ch)) || ((unsigned char) ch < 5)) {
	         if ((curpos < maxlen) || (maxlen == -1)) {
		   result+=ch;
		   if (flag==FLAG_INPUT_MASKED)
		     sprintf(chs,"%c",mask);
           	   if (getUser()->echo) npr(chs);
                   curpos++;
                 }
               }
	       
	     } else {
	       if (strchr(ALLOWED_CHARACTERS,ch)) {
	         if ((curpos < maxlen) || (maxlen == -1)) {
		   result+=ch;
		   if (flag==FLAG_INPUT_MASKED)
		     sprintf(chs,"%c",mask);
           	   if (getUser()->echo) npr(chs);
                   curpos++;
                 }
               }
	     }
  }
}
return result;
}

void showmessage(int n, int flag) {
	ShowMessageAllBut(n,flag,-1);
}


bool IsIn(int i, vector<int> vec) {
	for (int j=0; j<vec.size(); j++) 
		if (vec[j]==i) return true;
	return false;
}

void ShowMessageAllBut(int n, int flag, int noShow) {
string s;
int c,stat;
QInstantMessageRec msg;
vector<int> Con;
c=GetFirstConnectionByNode(n);
QNodeDataRec nd;
if (c== -1) {
	debug("Ohshit, ShowMessageAllBut() c == -1");
	return;
}

nd=connection[c]->getNodeData();
if (nd.Sec.network==1) {
  switch(flag) {
    	  case FLAG_LOGINBBS:
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 to |15BBS|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
			msg.lc=connection[c]->Network[i]->Sec.lc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;

		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
			msg.rc=connection[c]->Network[i]->Sec.rc;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
		  	msg.msgType=MSG_LOGIN;
		  	BroadcastAllBut(noShow, msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllBut(noShow, msg);
			return;
		}
	    }
		  
		  break;
	  case FLAG_LOGIN:
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
			msg.lc=connection[c]->Network[i]->Sec.lc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;

		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
			msg.rc=connection[c]->Network[i]->Sec.rc;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
		  	msg.msgType=MSG_LOGIN;
		  	BroadcastAllBut(noShow, msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllBut(noShow, msg);
			return;
		}
	    }
        break;
    case FLAG_LOGOUT:
	if (connection[c]->getLoggedin()) {
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
		  	msg.lc=connection[c]->Network[i]->Sec.lc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;
		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	msg.rc=connection[c]->Network[i]->Sec.rc;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
		  	msg.msgType=MSG_LOGOUT;
		  	BroadcastAllBut(noShow, msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllButLinks(noShow, msg);
			return;
		}
	    }
	}
	break;
    case FLAG_DISCONNECT:
	if (connection[c]->getLoggedin()) {
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
		  	msg.lc=connection[c]->Network[i]->Sec.lc;
		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	msg.rc=connection[c]->Network[i]->Sec.rc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
			msg.msgType=MSG_DISCONNECT;
		  	BroadcastAllBut(noShow, msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllButLinks(noShow, msg);
			return;
		}
	    }
	}
	break;
    case FLAG_KILLED:
	if (connection[c]->getLoggedin()) {
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
		  	msg.lc=connection[c]->Network[i]->Sec.lc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;
		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	msg.rc=connection[c]->Network[i]->Sec.rc;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
		  	msg.msgType=MSG_KILLED;
		  	BroadcastAllBut(noShow,msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllButLinks(noShow, msg);
			return;
		}
	    }
	}
	break;
    case FLAG_RELOG:
	if (connection[c]->getLoggedin()) {
	    for (int i=0; i < connection[c]->Network.size(); i++) {
	  	if (connection[c]->Network[i]->Nodenum==n) {
			msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	s += "|07at |15";
		  	s += Now() + "|07 from |10" + connection[c]->myLink.name + "|07.";
		  	msg.message=s;
		  	msg.priority=PRIORITY_SYSTEM;
		  	msg.lc=connection[c]->Network[i]->Sec.lc;
			msg.channelFrom=connection[c]->Network[i]->User.channel;
		  	msg.nodeFrom=connection[c]->Network[i]->Nodenum;
		  	msg.rc=connection[c]->Network[i]->Sec.rc;
		  	msg.chathandle = connection[c]->Network[i]->User.chathandle;
		  	msg.msgType=MSG_RELOG;
		  	BroadcastAllBut(noShow,msg);
		  	msg.lc=' ';
			msg.rc=' ';
			msg.msgType=MSG_SETATTR;
			msg.message=string(connection[c]->Network[i]->User.loginname);
			msg.chathandle="LOGINNAME";
			BroadcastAllButLinks(noShow, msg);
			return;
		}
	    }
	}
	break;
    default: break;
  }
} else {
  switch(flag) {
    case FLAG_LOGINBBS:
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now() + " |07to |15BBS|07";
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
	  	msg.lc=nd.Sec.lc;
		msg.channelFrom=nd.User.channel;
		msg.nodeFrom=nd.Nodenum;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_LOGIN;
	  	BroadcastAllBut(noShow, msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
		break;
    case FLAG_LOGIN:
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now();
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
	  	msg.lc=nd.Sec.lc;
		msg.channelFrom=nd.User.channel;
		msg.nodeFrom=nd.Nodenum;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_LOGIN;
	  	BroadcastAllBut(noShow, msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
        break;
    case FLAG_RELOG:
	if (connection[c]->getLoggedin()) {
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now();
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
	  	msg.lc=nd.Sec.lc;
		msg.nodeFrom=nd.Nodenum;
		msg.channelFrom=nd.User.channel;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_RELOG;
	  	BroadcastAllBut(noShow, msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
	}
	break;
    case FLAG_LOGOUT:
	if (connection[c]->getLoggedin()) {
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now();
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
	  	msg.lc=nd.Sec.lc;
		msg.nodeFrom=nd.Nodenum;
		msg.channelFrom=nd.User.channel;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_LOGOUT;
	  	BroadcastAllBut(noShow, msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
	}
	break;
    case FLAG_DISCONNECT:
	if (connection[c]->getLoggedin()) {
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now();
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
	  	msg.lc=nd.Sec.lc;
		msg.nodeFrom=nd.Nodenum;
		msg.channelFrom=nd.User.channel;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_DISCONNECT;
	  	BroadcastAllBut(noShow, msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
	}
	break;
    case FLAG_KILLED:
	if (connection[c]->getLoggedin()) {
		msg.nodeFrom=nd.Nodenum;
	  	s += "|07at |15";
	  	s += Now();
	  	msg.message=s;
	  	msg.priority=PRIORITY_SYSTEM;
		msg.channelFrom=nd.User.channel;
	  	msg.lc=nd.Sec.lc;
		msg.nodeFrom=nd.Nodenum;
	  	msg.rc=nd.Sec.rc;
	  	msg.chathandle = nd.User.chathandle;
	  	msg.msgType=MSG_KILLED;
	  	BroadcastAllBut(noShow,msg);
		msg.lc=' ';
		msg.rc=' ';
		msg.msgType=MSG_SETATTR;
		msg.message=string(nd.User.loginname);
		msg.chathandle="LOGINNAME";
		BroadcastAllButLinks(noShow, msg);
		return;
	}
	break;
    default: break;
  }
}
}

void Connection::logout(int flag) {

  debug("Logout - Node: "+IntToStr(getNodenum())); 
  //clearmessages();
  debug("clearmessages()");
  if (getSec()->network==1) {
	while (Network.size() > 0) {
		showmessage(Network[0]->Nodenum, flag);
		Network.erase(0);
	}
  }
  showmessage(getNodenum(), flag);
  if ((!getGuest()) && (getLoggedin())) {
    save_user(getUser());
  }
  if (flag != FLAG_DISCONNECT) checkForMessages();
  //KillThis
  if (flag == FLAG_RELOG) {
	setLocation(LOC_LOGIN);
	printfile("./etc/msg.relog");
	pause();
  }

  debug("Ending...");
  End(flag);
}

void Connection::End(int flag) {
	setLoggedin(false);	
	Finished=flag;
}


int GetSecurityByName(string s, QSecurityRec *sec) {
int i=1;
QSecurityRec temp;

	while (SecurityFile.Read(&temp,i)) {
		if (toLower(string(temp.name)) == toLower(s)) {
			SecurityFile.Read(sec,i);
			return i;
		}
		i++;
	}
	return 0;
}


void SaveUserListToFile(void) {
	LockMutex(&MUTEX_ACCESSUSERLIST);
		UserList.PreOrder(SaveUsersToFile, NULL);
	UnlockMutex(&MUTEX_ACCESSUSERLIST);
}

int SaveToFile(void *item, size_t size, int which, string filename) {
	FILE *fp;
	size_t pos;

	pos=(which) * (size);
	if (!exist(filename)) {
		if ((fp=fopen(filename.c_str(), "a+b")) == NULL) {
			debug("Cannot write to "+filename);
			return 0;
	 	}
	} else {
		if ((fp=fopen(filename.c_str(), "r+b")) == NULL) {
			debug("Cannot write to "+filename);
			return 0;
		}
	}
	rewind(fp);
	if (pos > 0) fseek(fp, pos, SEEK_SET);
	fwrite(item, size, 1, fp);
	fflush(fp);
	fclose(fp);
	return 1;
}

int LoadFromFile(void *item, size_t size, int which, string filename) {
	FILE *fp;
	size_t pos;

	pos=(which) * (size);
	if (!exist(filename)) {
		debug("Cannot read from "+filename);
		syslog("Cannot read from "+filename);
		return 0;
	} else {
		if ((fp=fopen(filename.c_str(), "r+b")) == NULL) {
			debug("Cannot read from "+filename);
			syslog("Cannot read from "+filename);
			return 0;
		}
	}
	rewind(fp);
	if (pos > 0) fseek(fp, pos, SEEK_SET);
	fread(item, size, 1, fp);
	fclose(fp);
	return 1;
}

void getconfigfile(void) {
FILE *in;
int x;
char *s;

in=fopen("m2.conf","rt");

  while (!feof(in)) {
    x=lineinput(s,in);
  #ifdef DEBUG
    printf("%s\r\n",s);
  #endif
  }
fclose(in);
}

int lineinput(char *s, FILE *in) {
char ln[151],chs[81];
int ch;
strcpy(ln,"");
while (!feof(in)) {
ch=fgetc(in);
   if ((ch!='\r') && (ch!='\n')) {
     sprintf(chs,"%c",ch);
     strcat(ln,chs);
   }
   if ((ch == '\n') || (ch=='\r')) { strcpy(s,ln); return(-1); }
}
return (0);
}


int load_user(QUserRec *p, string loginname) {
  QUserRec aUser;
  if (loginname.length() > 20) loginname.erase(19,loginname.length()-20); //*** hm
  strcpy(aUser.loginname, loginname.c_str());
  LockMutex(&MUTEX_ACCESSUSERLIST);
    bool found=UserList.Search(aUser);
  UnlockMutex(&MUTEX_ACCESSUSERLIST);
  if (found) {
    *p=aUser;
    return 1;
  } else {
    return 0;
  }
}

void save_user2(QUserRec *p, int which) {
long pos;
FILE *fp;

     pos=(which) * (sizeof(QUserRec));
     if (!exist("system2.dat")) {
       if ((fp=fopen("system2.dat", "a+b")) == NULL) {
         debug("Cannot write to system2.dat");
         exit(1);
       }
     } else {
       if ((fp=fopen("system2.dat", "r+b")) == NULL) {
         debug("Cannot write to system2.dat");
         exit(1);
       }
     }
     rewind(fp);
     fseek(fp, pos, SEEK_SET);
     fwrite((QUserRec *)p, sizeof(QUserRec), 1, fp);
     fflush(fp);
     fclose(fp);
}

void Connection::SaveMe(void) {
	if (!getGuest()) {
		save_user(getUser());
	}
}

void save_user(QUserRec *p) {
	UserList.Insert(*p);
}

int totalusers(void) {

	return UserList.numLeaves();
}

int TotalLinks(void) {
int i=0;
QLinkRec temp;

  while (1) {
    i++;
    if (!LinkFile.Read(&temp, i)) return(i-1);
  }
}
    
int totalSecurityRecords(void) {
int i=0;
QSecurityRec temp;

  while (1) {
    i++;
    if (!SecurityFile.Read(&temp, i)) return(i-1);
  }

}

void Connection::CheckForPause(int & stop) {
	if (stop < 0) return;
	if (stop == 20) {	
		npr(SYSS+"Continue? (y/n/c): ");
		char ch=8;
		while (ch == 8) 
			ch=OneKey("YNC\r");
		switch (ch) {
			case 'Y': stop = -1; break;
			case 'N': stop = -2; break;
			case 'C': stop = -3; break;
			case '\r': stop = -1; break;
		}
		npr("\r\n");
	}
	stop++;
	return;			
}

int Connection::printfile(string filename) {
  FILE *filey;		// -2 is continous, -1 is STOP
  char line[2048];
  
  filey = fopen(filename.c_str(),"r");
  if (!filey) {
    syslog("printfile: " + filename + " not found.");
    return 0;
  }
  while (fgets(line, 2047, filey) != NULL) {
    npr(string(line) + "\r");
  }
  fclose(filey);
  return(1);
}

int Connection::printfilepause(string filename, int &stop) {
  FILE *filey;		// -2 is continous, -1 is STOP
  char line[2048];
  
  if (stop == -1) return 1;
     
  filey = fopen(filename.c_str(),"r");
  if (!filey) {
    syslog("printfilepause: " + filename + " not found.");
    return 0;
  }
  while (fgets(line, 2047, filey) != NULL) {
    npr(string(line) + "\r");
    CheckForPause(stop);
    if (stop == -1) return 1;
  }
  fclose(filey);
  return(1);
}

int exist(string filename) {
FILE *filey;

  filey = fopen(filename.c_str(),"r");
  if (!filey)
      return 0;
  fclose(filey);
  return 1;
}

string localinput(int maxlen) {
int ch,curpos=0,done=0, len;
char chs[21];
string result="";
Socket Input(STDIN_FILENO,false);
fflush(stdout);
cerr << '[';
	for (len=0; len < maxlen; len++)
		cerr << '.';
  cerr << ']';
	for (len=0; len < (maxlen)+1; len++)
                cerr << '\b';

  while (!done) {
	  ch=0;
	 while (ch==0)
	    ch=Input.GetChar(0,100);
    switch((unsigned char) ch) {
    case 127:
    case   8: 
	     if (curpos > 0) {
		if ((result.length()-1) == 0) result = "";
		else result= result.substr(0,result.length()-1);
                sprintf(chs,"%c.%c",8,8);
                curpos--;
             }
             break;
    case 0:  break;
    case 10:
    case 13: 
             done=1; 
             break;
    default: sprintf(chs,"%c",ch);
		 if ((strchr(ALLOWED_CHARACTERS,ch)) || ((unsigned char) ch < 5)) {
			 if ((curpos < maxlen) || (maxlen == -1)) {
		   		result+=ch;
                   		curpos++;
                 	 }
  		}
    }
  }
  
return result;
}


