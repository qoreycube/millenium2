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

int GetUserByNumber(int num, QUserRec &us) {
	vector<QUserRec> vecUserList;
 	UserList.PreOrder(ConvertToVector, (void *)&vecUserList);
	for (int i=0; i < vecUserList.size(); i++) {
		if (vecUserList[i].number == num) {
			us = vecUserList[i];
			return i;
		}
	}
	return -1;
}

void Connection::ChatRoom(void) {
  bool done=false;
  string s,olds, msg;
  double dif;

  if ((!testbit(USERBIT_LOGINCHAT, getUser()->flags)) && (getSec()->network != 1)) {
 	ShowMessageAllBut(getNodenum(), FLAG_LOGINBBS, getNodenum());
	writeLocation(LOC_BBS);
	MainMenu();
	writeLocation(LOC_CHATROOM);
  	if (Finished) return;
	QInstantMessageRec Msg;
  	Msg.msgType=MSG_SYSTEM;
  	Msg.priority=PRIORITY_SYSTEM;
  	Msg.message = chatheader() + " has joined channel |03C|11" + IntToStr(getUser()->channel); 
  	Msg.channelTo=getUser()->channel;
  	Broadcast(Msg);
  }
  
  while ((!done) && (!Finished)) {
    setPrompt("");
    writeLocation(LOC_CHATROOM);
    checkForMessages();
    s="";
    while ((s.length() == 0) && (!Finished)) {
    	s=input(400);
    }
    dif=difftime(now(), myNodeData.LastCommand);
    debug("[N "+IntToStr(getNodenum())+"] dif="+IntToStr((int)dif));
    if (getSec()->network != 1) {
	if (dif < 3.0) {
		myNodeData.FloodRate++;
	    } else {
		if (dif > 10.0) myNodeData.FloodRate=0;
		else if (dif > 6.0)  myNodeData.FloodRate-=2;
		else myNodeData.FloodRate--;
		if (myNodeData.FloodRate < 0) myNodeData.FloodRate=0;
    	    }
    	    if (myNodeData.FloodRate >= 7) {
		if (myNodeData.FloodRate==12) {
			QInstantMessageRec Msg;
			Msg.nodeFrom = getNodenum();
			Msg.chathandle = string(getUser()->chathandle);
			Msg.message = "is being killed for flooding the server.";
			Broadcast(Msg);
			logout(FLAG_KILLED);
		}
		int per = 85 + ((myNodeData.FloodRate-7)*3);
		
		npr("\r\n" +SYSS+"Please wait before sending another message |08(|07Flood Rate: |15"+IntToStr(per)+"%|08)|07" +SYSF);	
      		if ((ChatClient) || (!getUser()->echo)) npr("\r\n");
        }
    }
    myNodeData.LastCommand=now();
    if (getSec()->network == 1) {
		DoNetworkCommand(s);
    } else {
	    if (trailingpipe(s)) s+='|';
	    if (s.length() > 2) {
	    
	    if (testbit(USERBIT_COLONEMOTE, myNodeData.User.flags)) {
	       if (s[0]==';') s = "/me2" + s.substr(1,s.length()-1);
	       if (s[0]==':') s = "/me" + s.substr(1,s.length()-1);
	    }
	    if (s[0]=='~') s = "/me"+ s.substr(1,s.length()-1);
	      if (s[0]=='&') s = "/me"+ s.substr(1,s.length()-1);
	
	    }
	    if (getUser()->echo) npr("\r\n");
       if ((s.length() > 0) && ((s[0]=='/') || (s[0]=='.'))) {
	      s[0]='/';
	      olds=s;
	      s=toLower(s);
	//      if (s=="/q") {
	//	if (getFlag(USERBIT_LOGINCHAT)) logout(FLAG_LOGOUT);
        //		else done=true;
	//      } 
	      if (!getAway()) {
	        if (!doOwnerCommand(s, olds))
	          if (!doAdminCommand(s, olds))
	            if (!doSysopCommand(s, olds))
	  	      if (!doCosysopCommand(s, olds))
		        if (!doModeratorCommand(s, olds))
		          doCommand(s, olds);
	       } else doCommand(s,olds);    
       } else if (s.length() > 0) {
	        if (!getAway()) {
		  QInstantMessageRec newMsg;
		  newMsg.chathandle=getUser()->chathandle;
		  newMsg.lc = getSec()->lc;
		  newMsg.rc = getSec()->rc;
		  newMsg.priority=PRIORITY_CHAT;
		  newMsg.channelFrom=getUser()->channel;
		  newMsg.channelTo=getUser()->channel;
		  newMsg.nodeFrom=getNodenum();
		  newMsg.message=s;
		  newMsg.nodeTo=-1;
		  newMsg.msgType=MSG_CHAT;
		  Broadcast(newMsg);
      	 	} else {
		  npr(SYSS+"Command not available in away mode"+SYSF);
       	 	}
       }
    }
  } 
}

void Connection::LinkEditor(void) {
	int done=0;
	int currRec=1;
	int totalRecords = TotalLinks();
	char s2[81];
	QLinkRec currLink;
	
	LinkFile.Read(&currLink,currRec);
	string s;
	writeLocation(LOC_LINK_EDITOR);
	
	while((!done) && (!Finished)) {
		cls();
		cap("L i n k   E d i t o r");
		snpr("      |08.|07A|08. |15Name      : |10%s\r\n", currLink.name);
		snpr("      |08.|07D|08. |15Handle    : |10%s\r\n", currLink.handle);
		
		snpr("      |08.|07H|08. |15Host      : |10%s\r\n", currLink.host);
		snpr("      |08.|07P|08. |15Port      : |10%d\r\n", currLink.port);
		snpr("      |08.|07C|08. |15Comment   : |10%s\r\n\n", currLink.comment);
		snpr("      |08.|07T|08. |15Link Type : |10%s\r\n\n", StringLinkType(currLink.type).c_str());
		snpr("      |08.|07L|08. |15Login name: |10%s\r\n", currLink.loginname);
		snpr("      |08.|07W|08. Password  : %s\r\n\n", currLink.password);
		 npr("               |08.|07[|08.|15 Previous Record        |08.|07]|08.|15 Next record\r\n");
		 npr("               |08.|07N|08.|15 New record             |08.|07Q|08.|15 Quit\r\n\n");
		sprintf(s2,"      |04-|12=|15[ |10Link Editor, |15%d|10 of |15%d ]|12=|04- |07[ ]\b\b",currRec, totalRecords);
		npr(s2);
		setPrompt(string(s2));
		s=input(1);
		s[0]=toupper(s[0]);
		npr("\r\n\n");
		switch(s[0]) {
			case 'A':
				snpr("|04-|12=|15- Old Name: |10%s\r\n|04-|12=|15- New Name: |10",currLink.name);
				s=input(20);
				if (s.length() > 0) {
					strcpy(currLink.name, s.c_str());
				}
				break;
			case 'D':
				snpr("|04-|12=|15- Old Handle: |10%s\r\n|04-|12=|15- New Handle: |10",currLink.handle);
				s=input(30);
				if (s.length() > 0) {
					strcpy(currLink.handle, s.c_str());
				}
				break;
			case 'C':
				snpr("|04-|12=|15- Old Comment: |10%s\r\n|04-|12=|15- New Comment: |10",currLink.comment);
				s=input(50);
				if (s.length() > 0) {
					strcpy(currLink.comment, s.c_str());
				}
				break;
			case 'P':
				snpr("|04-|12=|15- Old Port: |10%d\r\n|04-|12=|15- New Port: |10",currLink.port);
				s=input(29);
				if ((s.length() > 0) && (atoi(s.c_str()) > 0)) {
					currLink.port=atoi(s.c_str());
				}
				break;
			case 'H':
				snpr("|04-|12=|15- Old Host: |10%s\r\n|04-|12=|15- New Host: |10", currLink.host);
				s=input(49);
				if (s.length() > 0) {
					strcpy(currLink.host, s.c_str());
				}
				break;
			case 'L':
				snpr("|04-|12=|15- Old Login name: |10%s\r\n|04-|12=|15- New Loginname: |10", currLink.loginname);
				s=input(29);
				if (s.length() > 0) {
					strcpy(currLink.loginname, s.c_str());
				}
				break;
				
			case 'W':
				snpr("|04-|12=|15- Old password: |10%s\r\n|04-|12=|15- New password: |10",currLink.password);
				s=input(29);
				if (s.length() > 0) {
					strcpy(currLink.password, s.c_str());
				}
				break;
			case 'T':
				snpr("\r\n\n|08.|07%d|08.|15 Millenium2\r\n", LINK_M2);
				snpr("|08.|07%d|08.|15 TinyChat\r\n", LINK_TC);
				snpr("|08.|07%d|08.|15 STS\r\n", LINK_STS);
				snpr("|08.|07%d|08.|15 DDial\r\n", LINK_DDIAL);
				snpr("|07Select: [ ]\b\b|10");
				s=input(1);
				if ((atoi(s.c_str()) < 5) && (atoi(s.c_str()) >= 0)) {
					currLink.type=atoi(s.c_str());
				} else {
					npr("|04-|12=|15-|07 Invalid Input\r\n\n");
					pause();
				}
				break;
			case 'N':	if (yn("\r\n\nCreate a new Link?")) {
				  		LinkFile.Write(&currLink, currRec);
						if (!yn("Create from this record?")) {
							strcpy(currLink.host,"NewHost");
							strcpy(currLink.loginname, "link");
							strcpy(currLink.password, "linkme");
							currLink.type=LINK_M2;
						}
				  		LinkFile.Write(&currLink, currRec);
						totalRecords++;
						currRec=totalRecords;
					}
				break;
			case '[':
				  LinkFile.Write(&currLink, currRec);
				  currRec--; if (currRec < 1) currRec = 1;
				  LinkFile.Read(&currLink, currRec);
				  break;
			case ']':
				  LinkFile.Write(&currLink, currRec);
				  currRec++; if (currRec > totalRecords) currRec = totalRecords;
				  LinkFile.Read(&currLink, currRec);
				  break;
			case 'Q': done=1;
				  LinkFile.Write(&currLink, currRec);
				  break;
		}
	}
}

void Connection::SystemEditor(void) {
	QSysInfoRec systemp=sysinfo;
	string s, in;
	bool done=false;
	while ((!done) && (!Finished)) {
		npr("\r\n\n");
		cap("S y s t e m ");
		npr("      |04.|12N|04. |07System Name: |10" + string(systemp.name)+"\r\n");
		npr("      |04.|12P|04. |07Port       : |10" + IntToStr(systemp.port)+"\r\n\n");
		npr("      |04.|12A|04. |07Link Handle: |10" + string(systemp.linkHandle)+"\r\n");
		npr("      |04.|12H|04. |07Link Host  : |10" + string(systemp.linkHost)+"\r\n");
		npr("      |04.|12I|04. |07Link Name  : |10" + string(systemp.linkName)+"\r\n\n");
		
		npr("      |04.|12Q|04. |07Quit\r\n\n");

		string prompt="      "+SYSS+"|07System Editor |15]|12=|04- |07: ";
		setPrompt(prompt);
		npr(prompt);
		
		in=input(1);
		switch(toupper(in[0])) {
			case 'Q': done=true;
				  setPrompt("");
				  sysinfo=systemp;
				  SysinfoFile.Write(&sysinfo,0);
				  
				  break;
			case 'X': done=true;
				  break;
			case 'N':
				  npr("\r\n\n|15Old System Name: |10" + string(systemp.name) + "|15\r\nNew System Name: |10");
				  s=input(30);
				  if (s.length() > 0) strcpy(systemp.name, s.c_str());
				  break;
			case 'H':
				  npr("\r\n\n|15Old Link Hostname: |10" + string(systemp.name) + "|15\r\nNew Link Hostname: |10");
				  s=input(40);
				  if (s.length() > 0) strcpy(systemp.linkHost, s.c_str());
				  break;
			case 'I':
				  npr("\r\n\n|15Old Link Name: |10" + string(systemp.name) + "|15\r\nNew Link Name: |10");
				  s=input(30);
				  if (s.length() > 0) strcpy(systemp.linkName, s.c_str());
				  break;
			case 'A':
				  npr("\r\n\n|15Old Link Handle: |10" + string(systemp.name) + "|15\r\nNew Link Handle: |10");
				  s=input(30);
				  if (stripcolors(s).length() > 15) {
					npr("\r\n"+SYSS+"Length of handle cannot exceed 15 characters"+SYSF);
				  } else if (s.length() > 0) strcpy(systemp.linkHandle, s.c_str());
				  break;
			
			case 'P':
				  npr("\r\n\n|15Old Port: |10" + IntToStr(systemp.port) + "|15\r\nNew Port: |10");
				  s=input(30);
				  if (s.length() > 0) { 
					if (StrToInt(s) > 0) {
						if (StrToInt(s) < 1024) {
							npr("** WARNING ** Setting a port to < 1024 will force m2 to be run as root.\r\n\n");
							if (ny("Are you SURE you want to do this? ")) {	
								systemp.port=StrToInt(s);

							}
							break;
						} else  systemp.port=StrToInt(s);
					}
				  
				  }
				  break;
		}
	}
}

void Connection::SecurityEditor(void) {
string s;
int done=0, currRec=1, totalRecords=totalSecurityRecords();
QSecurityRec temp;
char s2[81];

  SecurityFile.Read(&temp, currRec);
  while ((!done) && (!Finished)) {
    cls();
    cap("S e c u r i t y   E d i t o r");
    snpr("      |08.|07L|08.|15 Level Name   : |12%s\r\n",temp.name);
    snpr("      |08.|07E|08.|15 Level Number : |12%d\r\n\n",temp.security);
    snpr("      |08.|07W|08.|15 Networkable  : |12%s\r\n\n",(temp.network==1)?"Yes":"No");
    snpr("      |08.|07M|08.|15 Moderator menu access: |12%s\r\n",(temp.moderator==1)?"Yes":"No");
    snpr("      |08.|07C|08.|15 Co-Sysop menu access : |12%s\r\n",(temp.cosysop==1)?"Yes":"No");
    snpr("      |08.|07S|08.|15 Sysop menu access    : |12%s\r\n",(temp.sysop==1)?"Yes":"No");
    snpr("      |08.|07A|08.|15 Admin menu access    : |12%s\r\n",(temp.admin==1)?"Yes":"No");
    snpr("      |08.|07U|08.|15 Unique Characters    : |12%c%c\r\n\n", temp.lc, temp.rc);

    npr("               |08.|07[|08. |15Previous Record        |08.|07]|08. |15Next Record\r\n");
    npr("               |08.|07N|08. |15New Security Level     |08.|07Q|08. |15Quit\r\n\n");
    sprintf(s2,"      |04-|12=|15[ |10Security Editor, |15%d|10 of |15%d |15]|12=|04- |07[ ]\b\b",currRec, totalRecords);
    npr(s2);
    setPrompt(string(s2));
    s=input(1);
    s[0]=toupper(s[0]);
    npr("\r\n\n");
    switch(s[0]) {
      case 'Q': setPrompt(""); SecurityFile.Write(&temp, currRec); done=1; break;
      case 'L': npr("New name: ");
                s=input(20);
                if (s[0] != 0) strcpy(temp.name,s.c_str());
                break;
      case 'W': temp.network=(temp.network==0); break;
      case 'M': temp.moderator=(temp.moderator==0); break;
      case 'A': temp.admin=(temp.admin==0); break;
      case 'C': temp.cosysop=(temp.cosysop==0); break;
      case 'S': temp.sysop=(temp.sysop==0); break;
      case 'U': npr("Two unique characters for the class: [  ]\b\b\b");
		s=input(2);
                if (s.length() == 2) {
                  temp.lc = s[0];
                  temp.rc = s[1];
                }
		break;
      case 'E': npr("Level Number: [   ]\b\b\b\b");
		s=input(3);
		if (atoi(s.c_str()) > 255) {
			s="255";
		} else if (atoi(s.c_str()) < 0) {
			s="0";
		}
		temp.security=atoi(s.c_str());
		break;
      case '[': SecurityFile.Write(&temp,currRec);
                currRec--;
		if (currRec < 1) currRec=1;
		SecurityFile.Read(&temp, currRec);
		break;
      case ']': SecurityFile.Write(&temp,currRec);
                currRec++;
                if (currRec > totalRecords) currRec=totalRecords;
		SecurityFile.Read(&temp, currRec);
                break;
      case 'N': if (yn("Create a new record?")) {
		  if (yn("Copy from this record?")) {
		    SecurityFile.Write(&temp, totalRecords++);
		  } else {
		    temp.cosysop=0;
		    temp.sysop=0;
		    temp.admin=0;
		    temp.moderator=0;
		    temp.lc = '(';
		    temp.rc = ')';
		    sprintf(temp.name, "New");
		    SecurityFile.Write(&temp, totalRecords++);
		  }
                }
                
    }
  }

}

void Connection::BanEditor(void) {
	string s;
	int stop=0;
	bool done=false;

	while (!done) {
	
		cap("B a n   E d i t o r");
		npr("|07Note: Regular expressions are allowed. ('man re_syntax' for details)\r\n");
		npr("      Any line starting with a '#' is considered a comment and will be deleted\r\n");
		npr("      Special Characters include: '\\.*(){}?' You must ESCAPE an ip address '.'\r\n");
		npr("      with the \\ character\r\n");
		npr("        For example: '|10199\\.17\\.|07*'\r\n\n");
		npr("|07Current Hostname / IP Address Bans:\r\n");
		for (int i=0; i<BanList.Size(); i++) {
			npr(" |07[|15" + IntToStr(i+1,2) + "|07] |10" + BanList[i] + "\r\n");
			CheckForPause(stop);
		}
		if (BanList.Size() == 0) npr("  |15-none-\r\n\n");
		npr(SYSS+"Choice [QAD]: ");
		int ch=8,z;
		while (ch==8) 
			ch=MenuKey("QAD");
		npr("\r\n\n");
		switch(ch) {
			case 'Q': done=true;
				  break;
			case 'A':
				  npr("Enter a ban expression\r\n:");
				  s=input(81);
				  npr("\r\n\n");
				  if (s.length() == 0) break;
					try {
						RegExp aRE(s);
						BanList.Add(s);
					} catch (REException e) {
						npr(SYSS+"Invalid RE Syntax: |10" + e.toString()+SYSF);
						pause();
					}
				  break;
			case 'D':
				  npr(SYSS+"Which would you like to delete: ");
				  s=input(3);
				  npr("\r\n");
				  z=StrToInt(s);
				  if (z <= 0) break;
				  if ((z > 0) && (z <= BanList.Size())) {
					if (yn("Are you sure you want to remove '" + BanList[z-1] + "'?"))
						BanList.Erase(z);
				  } else {
				  	npr(SYSS+"Invalid Record"+SYSF);
					pause();
				  }
				  break;
		}
	}
	npr(SYSS+"Writing changes to disk"+SYSF);
	BanList.SaveToFile("./etc/banlist");
}

void Connection::UserEditor(void) {
  char s2[81];
  int done=0;
  int inKey=0;
  int currRec=0;
  int total;
  unsigned int i;
  string s;
  QModifiedUserRec myUserRec;
  QSecurityRec sec;
  vector<QModifiedUserRec> ULEdit;
  vector<QModifiedUserRec> UserListEdit;
  vector<QSort> Items;
	QSort myRec;

  writeLocation(LOC_USER_EDITOR);
  npr(SYSS+"Loading UserList..."+SYSF);
  LockMutex(&MUTEX_ACCESSUSERLIST);
	UserList.PreOrder(ConvertToModifiedVector, (void *) &ULEdit);  
  UnlockMutex(&MUTEX_ACCESSUSERLIST);
  
  npr(SYSS+"Sorting UserList..."+SYSF);
  for (i=0; i < ULEdit.size(); i++) {
	myRec.index=i;
	myRec.sortedItem=ULEdit[i].user.created;
  	Items.push_back(myRec);
  }
  SelectionSort(&Items);
  npr(SYSS+"Compacting UserList..."+SYSF);
  for (i=0; i< ULEdit.size(); i++) {
    UserListEdit.push_back(ULEdit[Items[i].index]);
  }
  while (!done && !Finished) {
    total=UserListEdit.size()-1;
    if (!SecurityFile.Read(&sec, UserListEdit[currRec].user.security)) {
      UserListEdit[currRec].user.security = 1;
      SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
    }
    cls();
    cap("U s e r   E d i t o r");
    snpr("      |07.|15L|07. Login Name    : |10%s|15\r\n", UserListEdit[currRec].user.loginname);
    snpr("      |07.|15U|07. User Number   : |10%03d|15\r\n", UserListEdit[currRec].user.number);
    snpr("      |07.|15H|07. Handle        : |10%s|15\r\n", UserListEdit[currRec].user.chathandle);
     npr("      |07.|15P|07. Password      : |08<hidden>|15\r\n");
    snpr("      |07.|15S|07. Security Level: |10%s|15\r\n", sec.name);
    snpr("      |07.|15D|07. Doing         : |10%s|15\r\n", UserListEdit[currRec].user.doing);
    snpr("      |07.|15C|07. Channel       : |03C|11%d|15\r\n", UserListEdit[currRec].user.channel);
    snpr("      |07.|15T|07. Total Logins  : |10%ld|15\r\n", UserListEdit[currRec].user.totallogins);
    snpr("      |07.|15O|07. Points        : |10%ld|15\r\n", UserListEdit[currRec].user.points);
    snpr("      |07.|15E|07. Login Errors  : |10%ld|15\r\n\n", UserListEdit[currRec].user.loginerrors);
    
    npr("         |08.|07G|08. |07Goto User     |08.|07[|08.|07 Previous Record  |08.|07]|08.|07 Next Record\r\n");
    npr("         |08.|07N|08.|07 New user      |08.|07Q|08.|07 Quit             |08.|07X|08.|07 Exit Without Saving\r\n\n");
    npr("       |07.|15*|07. Marked for |12deletion : "+string((UserListEdit[currRec].deleted)?"Yes":"No")+"\r\n");
    sprintf(s2,"      |04-|12=|15[ |10User Editor, |15%d|10 of |15%d ]|12=|04- |07[ ]\b\b",currRec, total);
    npr(s2);
    setPrompt(string(s2));
    s="h";
    s[0]=8;
    while (s[0]==8) 
	    s[0]=MenuKey("LUHPSDCTOEG[]NQX*");
    //s=input(1);
    npr("\r\n\n");
    switch (toupper(s[0])) {
	    case '*': 
		    if (CompareSecurities(UserListEdit[currRec].user.security,getUser()->security)) 
			    UserListEdit[currRec].deleted=(UserListEdit[currRec].deleted==false);
		    else {
			    npr(SYSS+"Insufficient security clearance"+SYSF);
		    	    pause();
		    }
		    break;
		    
	    case 'U': npr(SYSS); snpr("Old Number: %d\r\n-=- New Number: [   ]\b\b\b\b",UserListEdit[currRec].user.number);
		      s=input(3);
		      if (s[0]==0) break;
			  for (i=0; i<UserListEdit.size(); i++) {
				if (atoi(s.c_str()) == UserListEdit[i].user.number) {
				    npr("|04-|12=|15-|07 User Number exists; Select another.\r\n\n");
				    pause();
				    goto theBreak;
				}
			  }
		      	  UserListEdit[currRec].user.number=atoi(s.c_str());
			  UserListEdit[currRec].modified=true;
			  
theBreak:
	      break;
      case 'N': if (yn("Are you sure you want to create a new user?")) {

		  if (ny("Copy the current user record?")) {
        		SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
			  if (sec.admin) {
	  			npr("|04-|12=|15-|07 User is an |10administrator|07; Option not allowed.\r\n"); 
				pause();
				break;
			  }
			  myUserRec.user=UserListEdit[currRec].user;
		    	  UserListEdit.push_back(myUserRec);
			  currRec=total+1;
			  npr(SYSS+"Login Name: ");
		    	  s=input(20);
		          if (!ValidFN(s)) {
				    npr(SYSS+"Login name has invalid special characters.."+SYSF);
				    pause();
				    break;
			  }
			  npr("\r\n\n");
		    	  if (s.length() == 0) {
				    UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
				    npr(SYSS+"Cancelled\r\n\n");
			    	    currRec=total;
				    pause();
				    break;
			  }

			  for (i=0; i<UserListEdit.size()-1; i++) {
				if (s == string(UserListEdit[i].loginname)) {
				    UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
				    npr(SYSS+"Login name exists. Ignoring new user\r\n\n");
			    	    currRec=total;
				    pause();
				    goto breakstatement;
				}
			  }
			  
			  
			  strcpy(UserListEdit[currRec].user.loginname, s.c_str());
                          strcpy(UserListEdit[currRec].user.chathandle, s.c_str());
		    
		          npr(SYSS+"Password: ");
		          s=input(10);
		          npr("\r\n\n");
			  if (s.length() == 0) {
			  	UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
			  	npr("|04-|12=|15-|07 Cancelled\r\n\n");
			  	pause();
			  	currRec=total;
			  	break;
		          }
                          strcpy(UserListEdit[currRec].user.password, s.c_str());
		    	  UserListEdit[currRec].user.monitor=0;
		 	  UserListEdit[currRec].modified=true;
		    	  UserListEdit[currRec].user.created= now();
		    	  setbit(USERBIT_LOGINCHAT,UserListEdit[currRec].user.flags);
			  UserListEdit[currRec].loginname=" ";
		    	  UserListEdit[currRec].loginname[0]=1;
		  	  UserListEdit[currRec].user.number=0; //GetAvailableUserNumber();
		  } else { 
		    //ClearUserrec(&UserListEdit[currRec].user);
		    UserListEdit.push_back(myUserRec);
		    currRec=total+1;
		    npr(SYSS+"Login Name: ");
		    s=input(20);
		    npr("\r\n\n");
		    if (s.length() == 0) {
			    UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
			    npr(SYSS+"Cancelled\r\n\n");
			    currRec=total+1;
			    pause();
			    break;
		    }
			  for (i=0; i<UserListEdit.size()-1; i++) {
				if (s == string(UserListEdit[i].loginname)) {
				    UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
				    npr(SYSS+"Login name exists. Ignoring new user"+SYSF);
			    	    currRec=total;
				    pause();
				    goto breakstatement;
				}
			  }
                    strcpy(UserListEdit[currRec].user.loginname, s.c_str());
                    strcpy(UserListEdit[currRec].user.chathandle, s.c_str());
		    
		    npr(SYSS+"Password: ");
		    s=input(10);
		    npr("\r\n\n");
		    if (s.length() == 0) {
			    UserListEdit.erase(UserListEdit.begin() + UserListEdit.size()-1);
			    npr(SYSS+"Cancelled"+SYSF);
			    currRec=total+1;
			    pause();
			    break;
		    }
                    strcpy(UserListEdit[currRec].user.password, s.c_str());
                    strcpy(UserListEdit[currRec].user.doing, "");
		    UserListEdit[currRec].user.security=0;
		    UserListEdit[currRec].user.channel=1;
		    UserListEdit[currRec].user.totallogins=0;
		    UserListEdit[currRec].user.points=0;
		    UserListEdit[currRec].user.echo=true;
		    UserListEdit[currRec].user.sound=true;
		    UserListEdit[currRec].user.ansi=true;
		    UserListEdit[currRec].user.flags=0;
		    setbit(USERBIT_LOGINCHAT,UserListEdit[currRec].user.flags);
		    UserListEdit[currRec].user.number=0; // genereate new available one!
		    UserListEdit[currRec].user.loginerrors=0;
		    UserListEdit[currRec].modified=true;
		    UserListEdit[currRec].user.monitor=0;
		    UserListEdit[currRec].user.created = now();
		    UserListEdit[currRec].loginname=" ";
		    UserListEdit[currRec].loginname[0]=1;
		    UserListEdit[currRec].user.number=0;//GetAvailableUserNumber();
		  }
                }
breakstatement:
                break;
      case 'Q': done=1;
		setPrompt("");	
		break;
      case 'X': if (yn("Are you SURE you want to exit without saving?")) {
			done=2;
			setPrompt("");
		}
		break;
      case ']': 
		currRec++;
                if (currRec >= total) currRec=total;
                
                break;
      case '[': 
		currRec--; 
                if (currRec < 0) currRec=0;
                
		break;
      case 'G': // goto specific record number
        npr("Goto record number: ");
        s=input(3);
        inKey = atoi(s.c_str());
        if ((inKey >= 0) && (inKey <= total)) {
	  currRec = inKey;
        } 
	break;
	
      /*case 'Z': npr (" This is a temporary command.\r\n");
		if (ny("Are you POSITIVE you want to do this?")) {
			for (i=0; i<currRec; i++) {
				load_user(&UserListEdit[currRec].user, i);
				save_user2(&UserListEdit[currRec].user, i);
			}
		}
        break; */
      case 'D':
        SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
	if (sec.admin) {
	  npr(SYSS+"User is an |10administrator|07; Option not allowed."+SYSF); 
	  pause();
	  break;
	}
        
	npr(SYSS); snpr("Old Doing: %s\r\n     New Doing: ", UserListEdit[currRec].user.doing);
        s=input(59);
	strcpy(UserListEdit[currRec].user.doing,s.c_str());
	UserListEdit[currRec].modified=true;
	break;
      case 'H':
        SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
	if (sec.admin) {
	  npr(SYSS+"User is an |10administrator|07; Option not allowed.\r\n"); 
	  pause();
	  break;
	}
        snpr("Old Chat Handle: %s\r\nNew Chat Handle: ", UserListEdit[currRec].user.chathandle);
        s=input(50);
	  if (stripcolors(s).length() > 15) { 
	  	npr(SYSS+"User is an |10administrator|07; Option not allowed.\r\n");	
		pause();
	  } else {
		strcpy(UserListEdit[currRec].user.chathandle,s.c_str());
		UserListEdit[currRec].modified=true;
          }
	break;
      case 'L':
        SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
	if (sec.admin) {
	  	npr(SYSS+"User is an |10administrator|07; Option not allowed.\r\n");
		pause();
		break;
	} 
	snpr("Old Login Name: %s\tNew Login Name: ", UserListEdit[currRec].user.loginname);
        s=input(20);
        strcpy(UserListEdit[currRec].user.loginname, s.c_str());
        UserListEdit[currRec].modified=true;
        break;
      case 'P': // password
        SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
	if (sec.admin) {
	  npr(SYSS+"User is an |10administrator|07; Option not allowed.\r\n");
	  pause();
        } else {
          snpr("Old Password: <not shown>\tNew Password: ");
          s=input(10);
          strcpy(UserListEdit[currRec].user.password, s.c_str());
          UserListEdit[currRec].modified=true;
        }
	  break;
      case 'S':
        SecurityFile.Read(&sec, UserListEdit[currRec].user.security);
	/*
	if (sec.admin) {
		npr(SYSS+"User is an administrator. Option not allowed.\r\n");
		pause();
		break;
	}
	*/
	i=1;
	while (SecurityFile.Read(&sec, i)) {
         snpr("\r\n%d) %s",i,sec.name);
         i++;
	}
        snpr("\r\n\nOld Security Level: %d\tNew Security Level: [   ]\b\b\b\b", UserListEdit[currRec].user.security);
        s=input(3);
	if ((atoi(s.c_str()) <= 0) && ((unsigned int)atoi(s.c_str()) >= i)) {
	  npr(SYSS+"Invalid option.");
	  pause();
	  break;
	}	
        /*
	if (!CompareSecurities(atoi(s.c_str()), myNodeData.User.security)) {
		npr(SYSS+"Unable to allow. Security level is too high."+SYSF);
		pause();
	} else {
	*/
		UserListEdit[currRec].user.security = atoi(s.c_str());
        	UserListEdit[currRec].modified=true;
	// }
	npr("\r\n\n");
	break;
      case 'C': //channel
        snpr("Old Channel: C%d\tNew Channel: C", UserListEdit[currRec].user.channel);
        s=input(1);
	if ((atoi(s.c_str()) <= 0) && (atoi(s.c_str()) >= 9)) {
	  npr("|04-|12=|15-|07 Invalid option.");
	  pause();
	  break;
	}	
        UserListEdit[currRec].user.channel = atoi(s.c_str());
        UserListEdit[currRec].modified=true;
        break;
      case 'T':
        snpr("Old Total Logins: %d\tNew Total Logins: ", UserListEdit[currRec].user.totallogins);
        s=input(5);
	if (atoi(s.c_str()) < 0) {
	  npr("\r\n\n|04-|12=|15-|07 Invalid option.\r\n\n");
	  pause();
	  break;
	}	
	UserListEdit[currRec].user.totallogins = atoi(s.c_str());
        UserListEdit[currRec].modified=true;
        break;
      case 'O':
        snpr("Old Points: %ld\tNew Points: ", UserListEdit[currRec].user.points);
        s=input(6);
        UserListEdit[currRec].user.points = strtol(s.c_str(), NULL, 10);
        UserListEdit[currRec].modified=true;
        break;
      case 'E':
        snpr("Old Login Errors: %d\tNew Login Errors: ", UserListEdit[currRec].user.loginerrors);
        s=input(7);
	if (atoi(s.c_str()) < 0) {
	  npr("\r\n|04-|12=|15-|07 Invalid input.\r\n\n");
	  pause();
	  break;
	}	
        UserListEdit[currRec].user.loginerrors = atoi(s.c_str());
        UserListEdit[currRec].modified=true;
        break;
      default: break;       
    }
  }
  
  if (done == 1) {
	bool somedeleted=false;
	i=0;
	LockMutex(&MUTEX_ACCESSUSERLIST);
	while (i < UserListEdit.size()) {
		if (UserListEdit[i].modified) {
			if (UserListEdit[i].loginname[0] != 1) {	
				if (UserListEdit[i].loginname != string(UserListEdit[i].user.loginname)) {
					QUserRec usr;
					strcpy(usr.loginname, UserListEdit[i].loginname.c_str());
					npr(SYSS+"Removing occurance of "+string(usr.loginname)+" from the UserList"+SYSF);
					UserList.Delete(usr);
				} 
			}
			npr(SYSS+"Compacting \"|10"+string(UserListEdit[i].user.loginname)+"|07\" into the UserList"+SYSF);
			UserList.Insert(UserListEdit[i].user);
		} else if (UserListEdit[i].deleted) {
			npr(SYSS+"Deleting " + string(UserListEdit[i].user.loginname) + " from the UserList"+SYSF);
			UserList.Delete(UserListEdit[i].user);
			somedeleted=true;
		}
		i++;
	}
	if (somedeleted) {
		char s2[20];
		
		struct tm *l_time;
		time_t now;
		    
		time(&now);
		l_time = localtime(&now);
		strftime(s2, 20, "%m-%d-%y", l_time);
		string z="./tmp/system.dat."+string(s2);
		if (exist(z)) 
			unlink(z.c_str());
		
		link("./data/system.dat",z.c_str());
		unlink("./data/system.dat");
	}
	npr(SYSS+"Saving UserList to disk..."+SYSF);
	UserList.PreOrder(SaveUsersToFile, NULL);
	UnlockMutex(&MUTEX_ACCESSUSERLIST);
  } else {
	 npr(SYSS+"Ommiting changes..."+SYSF+"\r\n");
  }
  return;
}


void Connection::MainMenu(void) {
string s;
char ch=0;
bool done=false;
	while ((!done) && (!Finished)) {
		if (ch != 8) {
			printfile("./etc/mainmenu");
			snpr("|08-|07[  |07]|08- -|07[|15%s|07]|08-\r\n", getSec()->name);
			npr("|08-|07[ |15Main Menu |07-> |10");
			ch=8;
		}
		if (testbit(USERBIT_HOTKEYS, getUser()->flags)) {
			while (ch == 8) 
				ch=OneKey("CMEWOG/");

			npr("|07");
			if (ch != '/') npr("\r\n\n");
			switch(ch) {
				case 'G': 
					  if (yn("Are you sure you want to logout?")) 
					  	logout(FLAG_LOGOUT);
					  break;
				case 'C': done=1;
					  break;
				case 'M': Mail(); //Message System
				case 'E': break; // Email System
				case 'W': whosonline();
					  pause();
					  break;
				case 'O': break; //Online Message
				case '/': 
				  ch=OneKey("/G");
				  switch(ch) {
					case '/':
						s=input(40);
						debug(s);
						npr("|07\r\n\n");
						
						break;
					case 'G': 
						npr("|07\r\n\n");
						logout(FLAG_LOGOUT);
						break;
					case 8:
						npr("\b \b");
						break;
				  }
				  break;
			}
		} else {
			while (s.length() == 0)
				s=input(42);
			ch=toupper(s[0]);
			if (ch != '/') npr("\r\n\n");
			s = "";
			npr("|07");
			switch(ch) {
				case 'G': 
					  if (yn("Are you sure you want to logout?")) 
					  	logout(FLAG_LOGOUT);
					  break;
				case 'C': done=1;
					  break;
				case 'M': Mail(); //Message System
				case 'E': break; // Email System
				case 'W': whosonline();
					  pause();
					  break;
				case 'O': break; //Online Message
				case '/': 
				  if (s.length() > 1) {
					  ch=s[1];
				  	switch(ch) {
						case 'G': 
							npr("|07\r\n\n");
							logout(FLAG_LOGOUT);
							break;
				  	}
				  	break;
				}
			}	

		}
	}
}

void Connection::CommandDebug(int n) {
string s;
QParseRec myParse;
QStringList newFile;

switch (n) {

	case 1:
		npr("-=-: ");
		s=input(100);
		npr("\r\n");
		if (ParseCommand(s,myParse)) {
			npr("-=- TRUE: Start: "+IntToStr(myParse.start)+", finish: "+IntToStr(myParse.finish)+", command: "+myParse.command+", text: "+myParse.text+"\r\n");
			return;
		} else {
			npr("-=- FALSE: Start: "+IntToStr(myParse.start)+", finish: "+IntToStr(myParse.finish)+", command: "+myParse.command+", text: "+myParse.text+"\r\n");
			return;
		}
	case 2:
		if (TextEditor("hi.txt",TE_CREATE, &newFile) <= 0) {
				npr("Not saved\r\n");
		} else newFile.SaveToFile("hi.txt");
		break;
	case 3:
		if (TextEditor("hi.txt",TE_READIN, &newFile) <= 0) {
				npr("Not saved\r\n");
		} else newFile.SaveToFile("hi.txt");

	case 4: 
		 break;
		/*if (GraphicalEditor("hi.txt",TE_READIN, &newFile) <= 0) {
				npr("Not saved\r\n");
		} else newFile.SaveToFile("hi.txt");
		*/
	case 100: Mail(); break;
	default: return;
	}

}

bool ParseCommand(string s, QParseRec &parse) {
int i=0;
string temp;

	if (s.length()==0) return false;
	if ((s[0] != '/') && (s[0] != '.')) return false;
	s.erase(0,1); // get rid of / or .
	while ((!isdigit(s[i]) && (s[i] != '*') && (s[i] != ' ')) && (i < s.length())) { // parse the command out
		temp+=tolower(s[i]);
		i++;
	}
	parse.command=temp;
	if (parse.command == "") return false;
	if (i == s.length()) return true;
	while ((s[i]==' ') && (i < s.length())) { 
		s.erase(i,1);
	}
	if (i == s.length()) return true;
	temp="";
	
	if (isdigit(s[i]) || (s[i]=='*')) {
		if (isdigit(s[i])) {
			
			while  ((isdigit(s[i])) && (i < s.length()) ) {
				temp+=s[i];
				i++;
			}
			
		} else { 
			while  ((s[i]=='*') && (i < s.length())) {
				temp+=s[i];
				i++;
			}
		}
		
		if (temp == "*") parse.start=-1;
		else if (temp.find("*") != string::npos) {
			// a string with *130138 in it"
			while (i < s.length()) {
				temp+=s[i];
				i++;
			}
			parse.text=temp;
			return true;	
		} else if ((StrToInt(temp) == -1)) return false;
		else parse.start=StrToInt(temp);
		if (i==s.length()) return true;
		while ((s[i]==' ') && (i < s.length())) { 
			s.erase(i,1);
		}	
		if (i == s.length()) return true;
		temp="";

		if (s[i]=='-') {
			s.erase(i,1);
			while ((s[i]==' ') && (i < s.length())) { 
				s.erase(i,1);
			}	
			if (i == s.length()) return false;
			temp="";
			while (((isdigit(s[i])) || (s[i]=='*')) && ((i < s.length()))) {
				temp+=s[i];
				i++;
			}
			if (temp=="*") parse.finish=-1;
			else if (StrToInt(temp)==-1) return false;
			else parse.finish=StrToInt(temp);
			if (i==s.length()) return true;
			while ((s[i]==' ') && (i < s.length())) { 
				s.erase(i,1);
			}	
			if (i == s.length()) return true;
			temp="";
			while (i < s.length()) {
				temp+=s[i];
				i++;
			}
			parse.text=temp;
			return true;
		//} else if (isalpha(s[i])) {
		} else {
			temp="";
			while (i < s.length()) {
				temp+=s[i];
				i++;
			}
			parse.text=temp;
			return true;
		}		
	//} else if (isalpha(s[i])) {
	} else {
		while (i < s.length()) {
			temp+=s[i];
			i++;
		}
		parse.text=temp;
		return true;
	}
	return false;
}

void Connection::MessageEditor(void) {
	Directory msgsDir;

	if (!msgsDir.OpenDirectory("./msgs")) {
		npr(SYSS+"Directory ./msgs does not exist, inform the administrator" +SYSF);
		return;
	}
	
	while (!Finished) {
		npr(Center("|15Message Editor")+"\r\n");
		npr("|08-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n|15  ");
		int i=0;
		string fn =msgsDir.Next();
		while (fn.length() > 0) {
			if ((fn != ".") && (fn != "..")) {
				i++;
				if (i==1) npr("  ");
				npr(fn + "  ");
				if (i == 5) {
					npr("\r\n");
					i=0;
				}	
			}
			fn=msgsDir.Next();
		}
		if (i != 0) npr("\r\n");
		npr("|08-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\r\n|07");
		npr(SYSS+"Enter a filename, <ENTER> to exit: ");
		//Test for bad characters, like '.' or '/' or '*' or '?' or any other wildcards you can think of
		fn=input(30);
		npr("\r\n");
		msgsDir.Rewind();
		if (fn.length() == 0) break;
		if (ValidFN(fn)) {
			if (exist("./msgs/"+fn)) {
				QStringList newfile;
				
				if (TextEditor("./msgs/"+fn,TE_READIN, &newfile) > 0) {
					npr(SYSS+ "Saving file ./msgs/"+fn+SYSF);
					newfile.SaveToFile("./msgs/"+fn);
				} 
			} else {
				if (yn("File does not exist, create a new one?")) {
					QStringList newfile;
					if (TextEditor("./msgs/"+fn, TE_CREATE, &newfile) > 0) {
						npr(SYSS+"Saving file ./msgs/"+fn+SYSF);
						newfile.SaveToFile("./msgs/"+fn);
					}
				} 
			}
		} else {
			npr(SYSS+"Invalid characters in filename"+SYSF);
		}
	}

	msgsDir.CloseDirectory();
}


int Connection::TextEditor(string fn, int flag,QStringList *ReadIn) {
QStringList SL;
int cur=0;
bool done=false;
string s;
	if ((fn.length() > 0) && (flag==TE_READIN)) {
		if (!SL.LoadFromFile(fn)) {
			npr(SYSS+"Cannot read from file: " + fn + SYSF);
			return -1;
		}
		cur=SL.Size();
	} else if (ReadIn != NULL) {
		if (flag == TE_READIN) {
			SL=*ReadIn;
			cur=SL.Size();
		}
	}
top:
	cls();
	npr(Center("|15Millenium2 Editor v1.0")+"\r\n");
	if (fn.length()>0) {
		npr(Center("|15[ |07"+fn+" |15]")+"|07\r\n");
	}
	npr(TOPLINE+"|07");
	if (SL.Size() > 0) {
		for (int i=0; i<SL.Size(); i++) {
			npr("|15"+IntToStr(i,2)+"|08:|07"+SL[i] + "|07\r\n");
		}

	}
	while ((!done) && (!Finished)) {
		s=input(240);
		npr("\r\n");
		if ((s.length() > 0) && ((s[0]=='.') || (s[0]=='/'))) {
			QParseRec p;
			if (ParseCommand(s, p)) {
				if (p.command=="?") {
					printfile("./etc/help.texteditor");
				} else if (p.command=="abt") {
					npr(SYSS+"Aborted..."+SYSF);
					pause();
					return 0;
				} else if (p.command=="s") {
					npr(SYSS+"Saved..."+SYSF);
					*ReadIn = SL;
					pause();
					return 1;
				} else if (p.command=="i") {
					if ((p.start < 0) || (p.text.size() == 0) || (p.finish != -2)) {
						npr(SYSS+"Invalid command syntax; See /? for details"+SYSF);
					} else {
						if (p.start < SL.Size()) {
							SL.Insert(p.start,p.text);
							npr(SYSS+"Line inserted at #" +IntToStr(p.start,2)+SYSF);
						} else {
							npr(SYSS+"Line inserted at the end"+SYSF);
						}
						cur++;
					}
				} else if (p.command=="li") {
					goto top;
				} else if (p.command=="d") {
					if (SL.Size() == 0) {
						npr(SYSS+"No lines to delete"+SYSF);
					} else {
						if (p.text.length() > 0) {
							npr(SYSS+"Invalid command syntax; See /? for details"+SYSF);
						} else if (p.start==-2) {
							npr(SYSS+"Last line deleted"+SYSF);
							SL.Erase(cur);
							cur--;
						} else if ((p.start == -1) && (p.finish == -2)) {
							while (SL.Size()) SL.Erase(0);
							npr(SYSS+"All lines have been deleted"+SYSF);
						} else if (p.finish==-2) {
							if (p.start >= SL.Size()) {
								npr(SYSS+"Last line deleted"+SYSF);
								SL.Erase(cur);
								cur--;
							} else {
								npr(SYSS+"Line #"+IntToStr(p.start,2)+" has been deleted"+SYSF);
								SL.Erase(p.start);
								cur--;
							}
						} else {
							if (p.start==-1) p.start=1;
							if (p.finish==-1) p.finish=SL.Size()-1;
							if (p.finish < p.start) {
								npr(SYSS+IntToStr(p.start,2) + "-"+IntToStr(p.finish,2)+ ": Invalid range"+SYSF);

							} else {
								if (p.start >= SL.Size() || p.finish >= SL.Size()) {
									npr(SYSS+IntToStr(p.start,2) + "-"+IntToStr(p.finish,2)+ ": Invalid range"+SYSF);
								} else {
									for (int j=p.finish; j>=p.start; j--) {
										SL.Erase(j);
										cur--;
									}
									npr(SYSS+"Lines " + IntToStr(p.start,2)+"-"+IntToStr(p.finish,2)+" have been deleted"+SYSF);
								}
							}
					
						}
					}
				} else {
					npr(SYSS+"Invalid command; See /? for details"+SYSF);		
				}		
			} else {
				npr(SYSS+"Invalid command syntax, see /? for details"+SYSF);
			}
		} else {
			cur++;
			SL.Add(s);
		}
	}
}


void Connection::UserConfig(void) {
int i;
string prompt,s=" ";
	while (!Finished)  {
		cls();
		cap("U s e r   C o n f i g");
		npr(string("        |04.|12H|04. |15Menu Hotkeys  : ") + ((testbit(USERBIT_HOTKEYS,myNodeData.User.flags)) ? "|10On":"|12Off") + string("\r\n"));
		npr("        |04.|12L|04. |15Login to      : " + string(testbit(USERBIT_LOGINCHAT, myNodeData.User.flags)?"|10Chat":"|12Menu")+"\r\n");
		npr("        |04.|12U|04. |15UID in '/s'   : " + string(testbit(USERBIT_SHOWUID, myNodeData.User.flags)?"|10Enabled":"|12Disabled") + "\r\n");
		npr("        |04.|12D|04. |15Double Spacing: " + string(testbit(USERBIT_DOUBLESPACING, myNodeData.User.flags)?"|10Enabled":"|12Disabled") + "\r\n");
		npr("        |04.|12A|04. |15[Away] Flag   : " + string(testbit(USERBIT_AWAYFLAG, myNodeData.User.flags)?"|10Enabled":"|12Disabled") + "\r\n");
		npr("        |04.|12E|04. |15':' emotes    : " + string(testbit(USERBIT_COLONEMOTE, myNodeData.User.flags)?"|10Enabled":"|12Disabled") + "\r\n");
		npr("        |04.|12T|04. |15Time Stamping : " + string(testbit(USERBIT_TIMESTAMP, myNodeData.User.flags)?"|10Enabled":"|12Disabled") + "\r\n");
		if (testbit(USERBIT_TIMESTAMP, myNodeData.User.flags)) {	
		npr("        |04.|12S|04.|15 TS Type       : |07" + string(testbit(USERBIT_TIMESTAMPTYPE, myNodeData.User.flags)?"Before Messages":"15Minute Intervals") + "\r\n\n");
		} else npr("\r\n");
		prompt="      |04-|12=|15[ |10User Config |15]|12=|04-|07[ ]\b\b";
		setPrompt(prompt);
		npr(prompt);
		if (getFlag(USERBIT_HOTKEYS))  
			s[0]=MenuKey("QHLUASE");
		else
			s=input(1);
		switch(toupper(s[0])) {
			case 'E':
				  if (testbit(USERBIT_COLONEMOTE, myNodeData.User.flags))
					  clearbit(USERBIT_COLONEMOTE, myNodeData.User.flags);
				  else
					  setbit(USERBIT_COLONEMOTE, myNodeData.User.flags);
				  break;
			case 'D':
				  if (testbit(USERBIT_DOUBLESPACING, myNodeData.User.flags))
					  clearbit(USERBIT_DOUBLESPACING, myNodeData.User.flags);
				  else
					  setbit(USERBIT_DOUBLESPACING, myNodeData.User.flags);
				  break;
			case 'H': 
				  if (testbit(USERBIT_HOTKEYS, myNodeData.User.flags))
					  clearbit(USERBIT_HOTKEYS, myNodeData.User.flags);
				  else
					  setbit(USERBIT_HOTKEYS, myNodeData.User.flags);
				  break;
			case 'L':
				  if (testbit(USERBIT_LOGINCHAT, myNodeData.User.flags))
					  clearbit(USERBIT_LOGINCHAT, myNodeData.User.flags);
				  else
					  setbit(USERBIT_LOGINCHAT, myNodeData.User.flags);
				  break;
			case 'U':
				  if (testbit(USERBIT_SHOWUID, myNodeData.User.flags))
					  clearbit(USERBIT_SHOWUID, myNodeData.User.flags);
				  else
					  setbit(USERBIT_SHOWUID, myNodeData.User.flags);
				  break;
			case 'T':
				  if (testbit(USERBIT_TIMESTAMP, myNodeData.User.flags))
					  clearbit(USERBIT_TIMESTAMP, myNodeData.User.flags);
				  else
					  setbit(USERBIT_TIMESTAMP, myNodeData.User.flags);
				  break;
			case 'A':
				  if (testbit(USERBIT_AWAYFLAG, myNodeData.User.flags))
					  clearbit(USERBIT_AWAYFLAG, myNodeData.User.flags);
				  else
					  setbit(USERBIT_AWAYFLAG, myNodeData.User.flags);
				  break;
			case 'S':
				  if (testbit(USERBIT_TIMESTAMPTYPE, myNodeData.User.flags))
					  clearbit(USERBIT_TIMESTAMPTYPE, myNodeData.User.flags);
				  else
					  setbit(USERBIT_TIMESTAMPTYPE, myNodeData.User.flags);
				  break;

			case 'Q': return;
		}

	}
}

int StrToInt(string s) {
int i=0;
int digit=0;
int num=0;

	while (i < s.length()) {
		if (!isdigit(s[i])) return -1;
		digit=s[i]-'0';
		num*=10;
		num+=digit;
		if (num < 0) return -1;
		i++;
	}
	return num;
}
