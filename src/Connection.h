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

#ifndef CONNECTION_H
#define CONNECTION_H

#include "Thread.h"

class Connection : public Thread {

	private:
	SocketStream *mySocketStream;
	// real private functions
	bool ClientLink;
	bool ChatClient;
	QNodeDataRec myNodeData;	
	int Finished;
	
	void SetDefaults( void *arg);
	void SetDefaults(); // ONLY USED FOR RELOG
	void Begin(void);
	void End(int flag);
	
	// misccmd.c 
	void writeLocation(int location);
	int isOwner(void);
	int isAdmin(void);
	int isModerator(void);
	int isSysop(void);
	int isCosysop(void);
    
	// utl.c
	int MenuKey(string haystack);
	string FindUser(string s);
	void SendMail(void);
	int OneKey(string haystack);
	void pause(void);
	void whosonline(char flag=1);
	void login(void);
	void ChatRoom(void);
        void SecurityEditor(void);
	void BanEditor(void);
	void UserEditor(void);
	bool doOwnerCommand(string s, string olds);
	bool doAdminCommand(string s, string olds);
	bool doSysopCommand(string s, string olds);
	bool doCosysopCommand(string s, string olds);
	bool doModeratorCommand(string s, string olds);
	void doCommand(string s, string olds);
	void line(int flag);
	string Center(string s, unsigned int i=78);

	// fileio.c 
	int printfile(string filename);
	void CheckForPause(int &stop);
	int printfilepause(string filename, int &stop);
	void SaveMe(void);
	
	// socket.c 
	int ny(string s);
	int yn(string s);
	
	void npr(string message);
	void snpr(char *fmt, ...);
    	void cap(char *s);    
	void cls(void);
	char getkeyc(void);
	string input(int maxlen);
        string fxinput(int maxlen);
	string inputm(int maxlen, char mask);
	string inputfunc(int maxlen, int flag, char mask, bool wrap=false, string add="");
	int getkey(void);
	
	// main.c ??
	void PrepClientLink(string s);

	// message.c 
	void checkForMessages(void);
	void DoNetworkCommand(string s);
	void LinkEditor(void);
	string ReformatMessage(QInstantMessageRec *Msg);

	// commands.c
	void ListUsers(void);

	// menus.c
	void MainMenu(void);
	void Mail(void);
	void CommandDebug(int n);
	void UserConfig(void);
	void SystemEditor(void);
	void MessageEditor(void);
	int TextEditor(string fn, int flag = TE_READIN, QStringList *ReadIn = NULL);

public:	
	SocketStream *getSocketStream() { return mySocketStream; }
	Connection(Socket *mySock) {
		mySocketStream = new SocketStream(mySock);
		ClientLink=false;
		ChatClient=false;
		myNodeData.LastCommand=now();
		myNodeData.Logintime=now();
		myNodeData.Nodenum=0;
		myNodeData.Idle=now();
		myNodeData.FloodRate=0;
		myNodeData.Delete=false;
		myNodeData.Ip=mySock->getRemoteIP();
		myNodeData.Hostname=mySock->getRemoteHost();
		Finished=0;
	}
	
    	QLinkRec myLink;
        int  allowMsgs;
	int  enterflag;

	void clearmessages(void);
	void Snpr(char *fmt, ...);
	void PutChar(char ch) {
		mySocketStream->PutChar(ch);
	}
	void logout(int flag);

	QNodeDataRec getNodeData(void) { return myNodeData; }
	time_t *getLogintime() { return &myNodeData.Logintime; }
	time_t *getTimeon() { return &myNodeData.Timeon; }
	time_t *getIdle() { return &myNodeData.Idle; }
	QUserRec *getUser() { return &myNodeData.User; }
	QSecurityRec *getSec() { return &myNodeData.Sec; }
	bool getGuest() { return myNodeData.Guest; }
	char getFlag(int bit) { return testbit(bit, myNodeData.Flags); }	
	int getLocation() { return myNodeData.Location; }
	bool getLoggedin(void) { return(myNodeData.Loggedin); }
	bool getAway(void) { return myNodeData.Away; }
	string getAwaymsg(void) {return myNodeData.Awaymsg; }
	int getNodenum(void) { return myNodeData.Nodenum; }
	string getIp(void) { return string(myNodeData.Ip); }
	bool getIsbeingedited(void) { return myNodeData.IsBeingEdited; }
	string getPrompt(void) { return myNodeData.Prompt; }
	bool getMonitor(int i) { return testbit(i,myNodeData.User.monitor); }
	bool getDelete(void) { return myNodeData.Delete; };
	
	
	void setPrompt(string s) { myNodeData.Prompt=s; }
	void setIsbeingedited(bool b) {
		myNodeData.IsBeingEdited=b;
		if (myNodeData.IsBeingEdited) {
			time(&myNodeData.StartedEditing);
		}
	}
	void setMonitor(int i, bool b) {
          if (b) setbit(i,myNodeData.User.monitor);
	  else clearbit(i, myNodeData.User.monitor);
	}
	void setAway(bool b, string msg="") { 
		myNodeData.Away=b; 
		myNodeData.Awaymsg=msg; 
		if (myNodeData.Away) time(&myNodeData.Awaytime); 
	}
	void setNodenum(int n) { myNodeData.Nodenum=n; }
	void setFlag(int i) { setbit(i, myNodeData.Flags); }	
	void clearFlag(int i) { clearbit(i, myNodeData.Flags); }
	void setGuest(bool g) { myNodeData.Guest=g; }
	void setUser(QUserRec user) { myNodeData.User = user; }
	void setDelete(bool b) { myNodeData.Delete=b; };
	void setSec(QSecurityRec s) { myNodeData.Sec=s; }
	void setLocation(int l) { myNodeData.Location = l; }
	void setLoggedin(bool l) { myNodeData.Loggedin = l; }
	void setIdle(time_t i) { myNodeData.Idle = i; }
	void setTimeon(time_t t) { myNodeData.Timeon=t; } 
	void setIp(string s) { myNodeData.Ip = s; }	
	void setStayAlive(int secs) {
		myNodeData.StayAlive=secs;
	}
	string chatheader(int networkNode = -1);	
	Vector<QNodeDataRec *> Network;
	Vector<QInstantMessageRec> msg;   // Seperate Messaging system for each node
	
	void Execute(void *arg);

	Connection(Socket *mySock, QLinkRec lr) {
		myNodeData.Nodenum=0;
		myLink = lr;
		myNodeData.Logintime=now();
		myNodeData.Idle=now();
		//debug("Connection() - New Connection Created!");
		ClientLink=true;
		myNodeData.Ip=mySock->getRemoteIP();
		myNodeData.Hostname=mySock->getRemoteHost();
		mySocketStream = new SocketStream(mySock);
		ChatClient=false;
		myNodeData.FloodRate=0;
		myNodeData.LastCommand=now();
		myNodeData.Delete=false;
		Finished=0;
	}

	~Connection() {
	//	debug("~Connection()");
	}
};

#endif
