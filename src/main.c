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
#include <signal.h>

QUserRec guest;
QSysInfoRec sysinfo;
Vector<Connection *> connection;
Vector<QChannel> ChannelList;
BST<QUserRec> UserList;
pthread_mutex_t MUTEX_CONNECTION;
pthread_mutex_t MUTEX_ACCESSUSERLIST;
QFileDatabase LinkFile("./data/links.dat", sizeof(QLinkRec));
QFileDatabase UserFile("./data/system.dat", sizeof(QUserRec));
QFileDatabase SecurityFile("./data/security.dat", sizeof(QSecurityRec));
QFileDatabase SysinfoFile("./data/sysinfo.dat", sizeof(QSysInfoRec));
QFileDatabase ChannelFile("./data/channels.dat", sizeof(QChannelInfo));
QStringList BanList;

void LockMutex(pthread_mutex_t *mtx) {
	pthread_mutex_lock(mtx);
}

void UnlockMutex(pthread_mutex_t *mtx) {
	pthread_mutex_unlock(mtx);
}

void InitializeGlobalMutex(void) {

  pthread_mutex_init(&MUTEX_CONNECTION, NULL);
  pthread_mutex_init(&MUTEX_ACCESSUSERLIST, NULL);

}

void LoadUserList(void) {
int i=1;
QUserRec tempUser;
	LockMutex(&MUTEX_ACCESSUSERLIST);
	while (UserFile.Read(&tempUser,i)) { 
		UserList.Insert(tempUser);
		i++;
	}
	UnlockMutex(&MUTEX_ACCESSUSERLIST);
	//cerr << UserList.numLeaves() << " users added to BST.\r\n";
}

void setsignals(void) 
{     
  signal(SIGIO,broken_pipe);
  signal(SIGHUP,broken_pipe);
  signal(SIGINT,destroy);
  signal(SIGTERM,destroy); 
 
} 

void checkExistFiles() {
  QUserRec temp;
  QSecurityRec sec;
  QLinkRec lr;

  if (!exist("./etc/banlist")) {
  	  cerr << "--> Creating banlist..." << endl;
	  
	  QStringList aList;
  	  aList.Add("# Any hostnames or IP's you want to be banned, please add them here, one per line.");
  	  aList.Add("# Regular expressions can be used here to describe a collection of hosts");
	  aList.Add("# type 'man regex' for more details, or 'man re_syntax'");
	  aList.Add("# For example:");
	  aList.Add("#  qorey*");
	  aList.Add("#  ipwnzjoo*");
	  aList.Add("#  199\\.17\\.*");
	  aList.SaveToFile("./etc/banlist");
  }
  if (!exist("./data/channels.dat")) {
  	  cerr << "--> Creating Channel Database..." << endl;
	  QChannelInfo myC;
	  memset(&myC,0,sizeof(myC));
	  strcpy(myC.name, "Administrator");
	  strcpy(myC.topic, "No Topic");
	  myC.isPublic=1;
	  myC.locked=0;
	  ChannelFile.Write(&myC,0);
	  strcpy(myC.name, "Main");
	  ChannelFile.Write(&myC,1);
	  strcpy(myC.name, "-unnamed-");
	  for (int i=2; i<=9; i++) 
		  ChannelFile.Write(&myC,i);
  }
  if (!exist("./data/links.dat")) {
    memset(&lr, 0, sizeof(QLinkRec));
    cerr << "--> Creating Link datafile\r\n";
    strcpy(lr.loginname, "link");
    cerr << "--> Creating Link datafile1\r\n";
    strcpy(lr.password, "linkme");
    cerr << "--> Creating Link datafile2\r\n";
    strcpy(lr.host, "digital-euphoria.org");
    cerr << "--> Creating Link datafile3\r\n";
    strcpy(lr.comment, "Digital Euphoria");
    cerr << "--> Creating Link datafile4\r\n";
    lr.port=5281;
    cerr << "--> Creating Link datafile5\r\n";
    lr.type=LINK_M2;
    cerr << "--> Creating Link datafile6\r\n";
    strcpy(lr.name, "|12D|15E");
    cerr << "--> Creating Link datafile7\r\n";
    strcpy(lr.handle, "|12D|15E");
    cerr << "--> Creating Link datafile8\r\n";

    LinkFile.Write(&lr, 0);
    LinkFile.Write(&lr, 1);
    SaveToFile((void *) &lr, sizeof(QLinkRec), 0, "./data/links.dat");
    SaveToFile((void *) &lr, sizeof(QLinkRec), 1, "./data/links.dat");
  }
  
  if (!exist("./data/sysinfo.dat")) {
    string news;
    cerr << "System information file not found, assuming fresh install.\r\n\n";
    cerr << "You MUST answer these questions to setup Millenium2 correctly\r\n";
    
    cerr << "System Name: ";
    news=localinput(30);
    if (news.length() == 0) exit(1);
    strcpy(sysinfo.name,news.c_str());
    
    cerr << "Link (Shortened, usually abbreviation) Name: ";
    news=localinput(30);
    if (news.length() == 0) exit(1);
    strcpy(sysinfo.linkName,news.c_str());
    
    cerr << "Link Chathandle name\n(Usually a shorthand of the system name): ";
    news=localinput(15);
    if (news.length() == 0) exit(1);
    strcpy(sysinfo.linkHandle,news.c_str());
    
    cerr << "Linking Hostname (Must exist, does not have to be correct):\n    ";
    news=localinput(40);
    if (news.length() == 0) exit(1);
    strcpy(sysinfo.linkHost,news.c_str());
    
    cerr << "System Port:";
    news=localinput(5);
    if (news.length() == 0) exit(1);
    sysinfo.port = StrToInt(news);
    
    if (sysinfo.port == -1) exit(1);
    sysinfo.locked=0;	// default to unlocked
    sysinfo.uptime=now();
    sysinfo.firststarted = now();
    sysinfo.totallogins=0;
    SysinfoFile.Write(&sysinfo,0);
  } else SysinfoFile.Read(&sysinfo,0);
  
  if (!exist("./data/security.dat")) {
    cerr << "Security Level database not found, creating a new one...\r\n\n";
    sec.admin=0;
    sec.cosysop=0;
    sec.sysop=0;
    sec.moderator=0;
    sec.network=0;
    sec.security=0;
    strcpy(sec.name, "Guest");

    sec.lc = '(';
    sec.rc = ')';
    SecurityFile.Write(&sec,0);
    SecurityFile.Write(&sec,1);
    
    sec.network=0;
    sec.admin=1;
    sec.cosysop=1;
    sec.sysop=1;
    sec.moderator=1;
    sec.security=254;
    strcpy(sec.name, "Admin");
    sec.lc = '{';
    sec.rc = '}';
    SecurityFile.Write(&sec,2);

    strcpy(sec.name, "Link");
    sec.lc='-';
    sec.rc='-';
    sec.network=0;
    sec.admin=0;
    sec.cosysop=0;
    sec.sysop=0;
    sec.moderator=0;
    sec.network=1;
    sec.security=25;
    SecurityFile.Write(&sec,3);
    
    strcpy(sec.name, "User");
    sec.lc='[';
    sec.rc=']';
    sec.network=0;
    sec.admin=0;
    sec.cosysop=0;
    sec.sysop=0;
    sec.moderator=0;
    sec.network=0;
    sec.security=25;
    SecurityFile.Write(&sec,4);
  }

  if (!exist("./data/system.dat")) {
    string news;
    
    cerr << "\r\n\nUser database not found, creating a new one\r\n\n";
    temp.security=1;
    temp.flags=0;
    strcpy(temp.loginname,"-");
    strcpy(temp.chathandle,"-");
    strcpy(temp.password,"-");
    temp.points=0;
    temp.loginerrors=0;
    temp.totallogins=0;
    temp.created=now();
    SaveToFile(&temp, sizeof(QUserRec), 0, "./data/system.dat");

    cerr << "SysOp Login Name: ";
    news=localinput(40);
    if (news.length() == 0) exit(1);
    strcpy(temp.loginname, news.c_str());

    cerr << "SysOp Chat Handle: ";
    news=localinput(15);
    if (news.length() == 0) exit(1);
    strcpy(temp.chathandle, news.c_str());

    cerr << "   Your password: ";
    news=localinput(30);
    if (news.length() == 0) exit(1);
    strcpy(temp.password, news.c_str());

    temp.channel=1;
    temp.echo=true;
    temp.sound=true;
    temp.ansi=true;
    temp.number=1;
    strcpy(temp.doing,"");
    
    setbit(USERBIT_OWNER, temp.flags);
    setbit(USERBIT_LOGINCHAT, temp.flags);
    setbit(USERBIT_SYSLOG, temp.flags);
    clearbit(USERBIT_HOTKEYS, temp.flags);

    cerr << "defaulting your security level to Admin.\r\n";
    temp.security=2;
    cerr << "defaulting your points to 0\r\n";
    temp.points=0;
    cerr << "defaulting the your flags to blank.\r\n";
    temp.flags=0;
    temp.loginerrors=0;
    SaveToFile(&temp, sizeof(QUserRec), 1, "./data/system.dat");
    temp.security=3;
    strcpy(temp.loginname, "link");
    strcpy(temp.chathandle, "link");
    temp.number=2;
    sprintf(temp.password, "link%d%d%d%d", randomnumber(9), randomnumber(9),randomnumber(9),randomnumber(9));
    SaveToFile(&temp, sizeof(QUserRec), 2, "./data/system.dat");
    strcpy(temp.loginname, "linkuser");
    temp.number=3;
    temp.security=1;
    strcpy(temp.chathandle, "linkuser");
    sprintf(temp.password, "link%d%d%d%d", randomnumber(9), randomnumber(9),randomnumber(9),randomnumber(9));
    SaveToFile(&temp, sizeof(QUserRec), 3, "./data/system.dat");
    cerr << "Finished creating datafiles, preparing to start server...\r\n\n";
  }
}

void setupGuest(void) {
	guest.points=0;
	guest.echo=true;
	guest.ansi=true;
	guest.security=1;
	guest.sound=true;
	guest.channel=1;
	guest.number=0;
	guest.flags=0;
	setbit(USERBIT_LOGINCHAT, guest.flags);
	strcpy(guest.loginname, "Guest");
	guest.totallogins=0;
}

void CheckChannels() {
	for (int i=0; i<=9; i++) {
		if (ChannelCount(i) == 0) {
			ChannelList[i].setPublic(true);
			ChannelList[i].setLocked(false);
		}
	}
}
void LoadBanList(void) {
	BanList.LoadFromFile("./etc/banlist");
	int i=0;
	while (i < BanList.Size()) {
		if (BanList[i][0]=='#') {
			BanList.Erase(i);
			i--;
		}
		i++;
	}
}

int main(int argc, char *argv[]) {
  
#ifndef DEBUG
  if (argc == 1) {
	cerr << "(*)(*)(*) Error: Do not run this program directly.\n                    Run the script supplied with this." << endl << endl;
	return 1;
  }
#endif
  int x,retval=0, timetosave=0;
  argument myArgs;
  
  try {
  	ServerSocket myServer;
  } catch (Exception e) {
	cerr << e.toString();
	return 0;
  }
  
  ServerSocket myServer;
  
  srand((unsigned) time(NULL));
  setsignals();
  InitializeGlobalMutex();
  
  setupGuest(); 
  checkExistFiles();
  LoadUserList();
  LoadBanList();
 
  cerr << UserFile.TotalRecords() << endl;
  for (int i=0; i <= 9; i++) {
	ChannelList.push_back(QChannel(&ChannelFile, i));
	ChannelList[i].Load();
  }

  // if not debugging, fork it.
#ifndef DEBUG
  int forkdeal;
  forkdeal=fork();
  if (forkdeal==-1) { printf("Could not fork?\r\n"); return 1; }
  else if (!forkdeal) {
    fflush(stdin);
    close(STDIN_FILENO);
    close(STDERR_FILENO);
    if (setsid()==-1) return(4);
  } else return(0);
#endif  

  sysinfo.uptime=now();
  sysinfo.firststarted=now();
  sysinfo.shutdown=0;

  /* create socket */
	try {
		myServer.setMaxClients(100);
		myServer.Listen(sysinfo.port);
	} catch (Exception e) {
		cerr << e.toString() << endl;
		return 1;
	}
	while (1==1) {  //never ending loop, as if you couldn't tell;
  		CheckChannels();
		timetosave++;
		if (sysinfo.shutdown==1) { // did someone run /shutdown?
			ForceMessage("\r\n\n|04-|12=|15- Millenium2 is shutting down -NOW-\r\n");
			saveOnlineUsers();
			while (connection.size() > 0) {
				ForceMessage("killed");	
				sleep(5);
				PackConnectionVector();
			}
			sleep(5);
			debug("* Saving userlist");
			SaveUserListToFile();
			debug("* Saving system information");
			SysinfoFile.Write(&sysinfo,0);
			return 0;
		}
		if (timetosave >= 1800) {  // save system information about every 1/2 hour
	        	timetosave=0;
			SysinfoFile.Write(&sysinfo,0);
		} else if (timetosave % 300 == 299) {
			saveOnlineUsers();
			LockMutex(&MUTEX_ACCESSUSERLIST);
			UserList.PreOrder(SaveUsersToFile, NULL);
			UnlockMutex(&MUTEX_ACCESSUSERLIST);
		}
		try {
			if ((timetosave % 30) == 0)
				CheckTimeStamps();	// check time stamps, it's been 30 seconds
			CheckKeepAlive();
			if (myServer.CheckForConnection(900,0)) {
				Socket *newSocket;
				try {
					newSocket = &myServer.Accept(); // accept a connection
				} catch (Exception e) {
					if (e.toString()=="(Accept) Maximum Clients Exceeded") {
						debug("* Maximum Clients Reached");
						goto cont;
					} else {
						cerr << e.toString() << endl;
						return 1;
					}
				}
				debug("* Accepted socket\r\n");
	    			bool noprob=true;
				for (int i=0; i<BanList.Size(); i++) {
					try {
						RegExp newRE(BanList[i]);
						if (newRE.Match(newSocket->getRemoteHost())) {
                                			newSocket->Send("-=- Sorry, this hostname has been banned.\r\n    If you feel your received this message incorrecly,\r\n     contact an administrator. ");
							syslog("Login attempt from banned Host: " + newSocket->getRemoteHost() + " (" + newSocket->getRemoteIP()+")");
							newSocket->Close();
							myServer.Erase(newSocket);
							noprob=false;
							break;
						}
						if (newRE.Match(newSocket->getRemoteIP())) {
                                			newSocket->Send("-=- Sorry, this hostname has been banned. If you feel your received\r\n     this message incorrecly, please contact an administrator.\r\n\n");
							syslog("Login attempt from banned IP: " + newSocket->getRemoteIP() + " (" + newSocket->getRemoteHost()+")");
							newSocket->Close();
							myServer.Erase(newSocket);
							noprob=false;
							break;
						}
					} catch (REException e) {
						debug("* BanList Error: '"+BanList[i]+"' - "+e.toString());
						syslog("*** BanList Error: '"+BanList[i]+"' - "+e.toString());
					}
				}
				if (noprob) {
					Connection *newC = new Connection(newSocket);
					connection.push_back(newC);
		    			if (newC->Start(&myArgs) != 0) {
						debug("* pthread_create() returned nonzero!"); // uhoh
		    			}
				}
			}
		} catch (Exception e) {
			if (e.getType() == TYPE_SERVERSOCKET) {
				cerr << e.toString() << endl;
				return 1;
			}
		}
cont:
		int x=0;
		while (x < connection.size()) {
			if (connection[x]->getCompleted() == true) {
				Connection *cptr;
				cptr=connection[x];
				Socket *sock=cptr->getSocketStream()->getSocket();
				myServer.Erase(sock);
				delete sock;
				connection.erase(x);
				int r=pthread_join(cptr->myThread, NULL); // we don't want any zombies laying about
				delete cptr;
				x=-1;
			}
			x++;
		}
	  } /* while (1) */

}

void RoundToThirty(int &secs) {
	
	secs += 30-(secs % 30);
}

void CheckKeepAlive(void) {
	int x=0;
	while (x < connection.size()) {
		if (timeDifference(connection[x]->getNodeData().Idle, now()) > 119) {
			int secs=(int)timeDifference(connection[x]->getNodeData().Idle,now());
			RoundToThirty(secs);
			string s;
			if ((secs % 120 == 0) && (connection[x]->getNodeData().StayAlive != secs)) {
				connection[x]->setStayAlive(secs);
				connection[x]->PutChar(255); // IAC from RFC
				connection[x]->PutChar(241); // NoOp from RFC
			}
		}
		x++;
	}
}

void PackConnectionVector(void ) {
	int x=0;
	while (x < connection.size()) {
		if (connection[x]->getCompleted() == true) {
			//debug("Erasing connection "+IntToStr(x));
			
			connection.erase(x);
			x=0;
		}
		x++;
	}
}

string FlagName(int flag) {
	switch(flag) {
		case FLAG_LOGIN: return "Login";
		case FLAG_LOGOUT: return "Logout";
		case FLAG_DISCONNECT: return "Disconnect";
		case FLAG_CONNECT: return "Connect";
		case FLAG_RELOG: return "Relogged";
	}
	return "FLAG NOT HANDLED";
}

void broken_pipe(int hm) {
  pthread_t mythread;
  int x;
  char s[181];
  mythread=pthread_self();

  setsignals();
  debug("broken_pipe(); called");
/*  for (x=0; x<connection.size(); x++) {
    if (connection[x]->myThread==mythread) {
	    debug(s,"BrokenPipe: Node#%d,thread:%d,sock:%d",x,mythread,connection[x]->getSocketid());
      	    connection[x]->logout(FLAG_DISCONNECT);
    }
  } */

//  debug("no thread was found...");

  }
void SaveOnlineUsers(void) {
	LockMutex(&MUTEX_ACCESSUSERLIST);
		UserList.PreOrder(SaveUsersToFile, NULL);
	UnlockMutex(&MUTEX_ACCESSUSERLIST);
}


void destroy(int hm) {

  SaveOnlineUsers();
  debug("Signal Thrown to destroy m2\r\n");
  sysinfo.shutdown=1;
  setsignals();
 //exit(0);

}

int GetAvailableNode(void) {
int n=1,i,j;
	LockMutex(&MUTEX_CONNECTION);
	while (1) {
		for (i=0; i<connection.size(); i++) {
			if (connection[i]->getNodenum() == n) goto tryAgain;
			else if (connection[i]->getSec()->network == 1) {
				for (j=0; j< connection[i]->Network.size(); j++) {
					if (connection[i]->Network[j]->Nodenum == n) goto tryAgain;
				}
			}
		}
tryAgain:
		if (i==connection.size()) {
			UnlockMutex(&MUTEX_CONNECTION);
			return n;
		}
		n++;
	}
	UnlockMutex(&MUTEX_CONNECTION);
}

void Connection::Begin(void) {
	string loginname,password;
	int status=0;
	QUserRec temp;
	//QSecurityRec sec;
	bool done=false;
	//int field;

	time(&myNodeData.Starttime);
	//clearmessages();
	writeLocation(LOC_LOGIN);
	snpr("%c%c%c",255,251,1);
	snpr("%c%c%c",255,251,3);
	printfile("./etc/msg.login");
	while ((status < 3) && (!Finished)) {
		done=false;
		npr("|07If you're not a registered user, enter \"GUEST\" for guest access.\r\n");
		loginname = string(sysinfo.name) + " login: ";
		int field=loginname.length();
		npr(loginname);
		loginname=input(20);
		if (loginname == "^!^ChatClient^!^") {
			ChatClient=true;
			npr("\r\n-=- Chat client Enabled\r\n");	
			continue;
		}
		loginname=toLower(loginname);
		if (loginname == "") {
			npr("\r\n\n|04-|12=|15-|07 Invalid login name, or password\r\n\n");
			status++;
		} else if (loginname == "guest") {
			setGuest(true);
			setUser(guest);
			setNodenum(GetAvailableUserNumber());
			sprintf(getUser()->loginname,"Guest%d",myNodeData.Nodenum);
			strcpy(getUser()->chathandle, "Logging In...");
			getUser()->channel=1;
			getSec()->network=0;
			SecurityFile.Read(getSec(), (int)getUser()->security);
			
			while ((!done) && (!Finished)) {
				if (sysinfo.locked > 0) {
					if (getSec()->security < sysinfo.locked) {
						npr("\r\n\n|04-|12=|15- |07System is currently |12locked|07. Try logging in at a later time\r\n");
						logout(FLAG_LOGOUT);
					}
				}
				if (!Finished) {
					npr("\r\n");
					npr(spacecolors("Guest Handle: ",field, ALIGN_RIGHT));
 					password=input(50);
					if (trailingpipe(password)) password += '|';
					if ((stripcolors(password).length() == 0) || (stripcolors(password).length() > 15)) {
						npr("\r\n|04-|12=|15- |07Handle must be within |101|15 to |1015|07 characters!\r\n\n");
					} else {
						strcpy(getUser()->chathandle,password.c_str());
						done=1;
						if (ChatClient) {
							getUser()->echo=false;
							getUser()->ansi=false;
						}
						if (sysinfo.locked > 0) {
							if (getSec()->security < sysinfo.locked) {
								npr("\r\n\n|04-|12=|15- |07System is currently |12locked|07. Try logging in at a later time\r\n");
								logout(FLAG_LOGOUT);
							}
						}
						if (!Finished) {
							login();
						}
						if (Finished == FLAG_RELOG) {
							SetDefaults();
							myNodeData.Starttime = now();	
							Finished=0;
							done=true;
							status=0;
						}
						continue;
					}
				}
			}
		} else {
			npr("\r\n");
			npr(spacecolors("Password: ",field, ALIGN_RIGHT));
			password=inputm(30,'o');
			if (!load_user(&temp, loginname)) {
				npr("\r\n\n|04-|12=|15-|07 Invalid login name, or password\r\n\n");
			} else {	
				if (password == temp.password) {
					setUser(temp);
					SecurityFile.Read(getSec(), (int)getUser()->security);
					setGuest(false);
					if (ChatClient) {
						getUser()->echo=false;
						getUser()->ansi=false;
					}
					clearFlag(USERBIT_SYSLOG);
					clearFlag(USERBIT_HOTKEYS);
					setFlag(USERBIT_LOGINCHAT);
					if (sysinfo.locked > 0) {
						if (getSec()->security < sysinfo.locked) {
							npr("\r\n\n|04-|12=|15- |07System is currently locked. Try logging in at a later time\r\n");
							logout(FLAG_LOGOUT);
						}
					} else {
						myNodeData.Nodenum = getUser()->number;
						login();
					}
					if (Finished == FLAG_RELOG) {
						Finished=0;
						done=true;
						status=0;
						myNodeData.Starttime = now();	
						SetDefaults();
					} else if (Finished==FLAG_LEFTCHAT) {
						logout(FLAG_LOGOUT);
					}
					continue;
				} else {
					temp.loginerrors++;
					save_user(&temp);
					npr("\r\n\n"+SYSS+" Invalid login name, or password"+SYSF+"\r\n");
				}
			}
			status++;
		}
	}
	if (Finished==0) logout(FLAG_LOGOUT);
}

void Connection::SetDefaults() {
		myNodeData.TempModerator=false;
		SecurityFile.Read(getSec(),1);
		myNodeData.Away=false;
		myNodeData.Flags = 0;
		//myNodeData.Hostname=string(myArgs.hostname);
		setUser(guest);
		strcpy(getUser()->chathandle, "Logging in...");
		//setIp(string(myArgs.ip));
		setLocation(LOC_LOGIN);
		setLoggedin(false);
		setGuest(false);
		setAway(false);
	getUser()->ansi=true;
	getUser()->sound=true;
	getUser()->echo=true;
	allowMsgs=1;
//	clearmessages();
	setIsbeingedited(false);
	setPrompt("");
}

void Connection::SetDefaults( void *arg ) {
	LockMutex(&MUTEX_CONNECTION);
		SecurityFile.Read(getSec(),1);
		time(getLogintime());
		myNodeData.Away=false;
		argument myArgs = *(argument *)arg;
		myNodeData.Flags = 0;
		//myNodeData.Hostname=string(myArgs.hostname);
		setUser(guest);
		strcpy(getUser()->chathandle, "Logging in...");
		//setIp(string(myArgs.ip));
		setNodenum(GetAvailableUserNumber());
		setLocation(LOC_LOGIN);
		setLoggedin(false);
		setGuest(false);
		setAway(false);
		myNodeData.TempModerator=0;
	UnlockMutex(&MUTEX_CONNECTION);
	syslog("|07["+getShortTime(now())+"] Connection from: " + getSocketStream()->getSocket()->getRemoteHost() + " ("+getSocketStream()->getSocket()->getRemoteIP()+")");
	string logme = "from " + getSocketStream()->getSocket()->getRemoteHost() + " (" + getSocketStream()->getSocket()->getRemoteIP() +  ")";
	connectionlog(logme, FLAG_CONNECT);
	getUser()->ansi=true;
	getUser()->sound=true;
	getUser()->echo=true;
	allowMsgs=1;
//	clearmessages();
	setIsbeingedited(false);
	setPrompt("");
}

void Connection::Execute(void *arg) {	
	if (ClientLink) {
		argument myArg;
		strcpy(myArg.ip,"127.0.0.1");
		strcpy(myArg.hostname,"127.0.0.1");
		myArg.nodenum=GetAvailableUserNumber();
		SetDefaults( (void *) &myArg);
		QUserRec tmpusr;
		strcpy(tmpusr.loginname, "link");
		if (UserList.Search(tmpusr)) {
			string str;
			setUser(tmpusr);
			SecurityFile.Read(getSec(), getUser()->security);
			strcpy(getUser()->chathandle, myLink.handle);
			sprintf(getUser()->doing, "|11%s |03%d",myLink.host, myLink.port);
			getUser()->monitor=0;
			if (myLink.type == LINK_TC) {
				debug("* TC Link Connected, sending VERINFO and WHOSONLINE");
				str=FormatTCMessage(TCMODE_VERINFO,TCMODE_VERINFO,"","");
				npr(str);
				str=FormatTCMessage(TCMODE_WHOSONLINE,0,"","");
				npr(str);
			} else if (myLink.type == LINK_M2) {
				debug("* M2 Link Connected, sending VERINFO, INFO, and WHOSONLINE");

				str=FormatM2Message(M2MODE_INFO,-1,-1,-1,-1,-1,-1,"HOST",sysinfo.linkHost);
				npr(str);
				str=FormatM2Message(M2MODE_INFO,-1,-1,-1,-1,-1,-1,"PORT",IntToStr(sysinfo.port));
				npr(str);
				str=FormatM2Message(M2MODE_INFO,-1,-1,-1,-1,-1,-1,"HANDLE",sysinfo.linkHandle);
				npr(str);
				str=FormatM2Message(M2MODE_INFO,-1,-1,-1,-1,-1,-1,"NAME",sysinfo.linkName);
				npr(str);
#ifdef OPENBSD
				str=FormatM2Message(M2MODE_VERINFO,-1,-1,-1,-1,-1,-1,"1.0.300r","OpenBSD");
				npr(str);

#else
				str=FormatM2Message(M2MODE_VERINFO,-1,-1,-1,-1,-1,-1,VERSION, OS);
				npr(str);

#endif
				str=FormatM2Message(MSG_END,-1,-1,-1,-1,-1,-1,"","");
				npr(str);
				str=FormatM2Message(M2MODE_WHOSONLINE,-1,-1,-1,-1,-1,-1,"","");
				npr(str);
			}
			login();
		} else {
			SystemMessage("Link user not found!");
		}
	} else {
		SetDefaults( arg );
		Begin();
	}
	if (Finished==0) logout(FLAG_LOGOUT);

	try {
		getSocketStream()->getSocket()->Close();
	} catch (Exception e) {
		debug(e.toString());
	}
}

bool FifteenMinuteInterval(time_t n) {
	char s[15];
	struct tm *l_time;
	l_time=localtime(&n);
	strftime(s, 15,"%M", l_time);
	int i=StrToInt(string(s));
	
	if ((i % 15) == 0) return true;
	else return false;
}

void CheckTimeStamps(void) {
	static bool sent;
	if (FifteenMinuteInterval(now())) {
		if (!sent) {
			sent=true;
			BroadcastTimeStamp();
		}
	} else sent=false;
}

