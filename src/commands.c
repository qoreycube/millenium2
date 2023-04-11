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

void Connection::ListUsers(void) {
int n=1;
int stop=0;
bool flag=false;
QSecurityRec tmpsec;
char s2[81];
string msg;
	allowMsgs=0;
	if (!ChatClient) {
		snpr("|15%s\r\n",Center("Listing Users").c_str());
		line(LINE_TOP);
		npr(" |03### |11Login|07                |10Chat Handle       |15Last Login\r\n");
	}
	saveOnlineUsers();
	vector<QSort> Items;
	QSort sItem;
	vector<QUserRec> vecUserList;
	LockMutex(&MUTEX_ACCESSUSERLIST);
		UserList.PreOrder(ConvertToVector, (void *)&vecUserList);
	UnlockMutex(&MUTEX_ACCESSUSERLIST);
	for (unsigned int i=0; i < vecUserList.size(); i++) {
		sItem.sortedItem=vecUserList[i].lastlogin;
		sItem.index=i;
		Items.push_back(sItem);
	}
	ReverseSelectionSort(&Items);
	for (unsigned int i=0; i < vecUserList.size(); i++) {
	  SecurityFile.Read(&tmpsec, vecUserList[Items[i].index].security);
	  if (ChatClient) { 
		QInstantMessageRec Msg;
		Msg.lc=tmpsec.lc;
		Msg.rc=tmpsec.rc;
		Msg.chathandle=string(vecUserList[Items[i].index].chathandle);
		Msg.msgType=MSG_LOGOUT;
		Msg.channelFrom=vecUserList[Items[i].index].channel;
		Msg.nodeFrom=0;
		npr(FormatClientMessage(&Msg));
		Msg.msgType=MSG_SETATTR;
		Msg.lc=' ';
		Msg.rc=' ';
		Msg.chathandle=IntToStr(vecUserList[Items[i].index].number,3);
		Msg.message=string(vecUserList[Items[i].index].loginname);
		npr(FormatClientMessage(&Msg));
	  } else {
		  sprintf(s2,"|03%03d",vecUserList[Items[i].index].number);
		  msg = " " + string(s2);
		  msg += " |11" + spacecolors(string(vecUserList[Items[i].index].loginname),20,ALIGN_LEFT);
		  sprintf(s2, "|10%c|07%s|10%c", tmpsec.lc, vecUserList[Items[i].index].chathandle, tmpsec.rc);
		  msg += " " + spacecolors(string(s2), 17, ALIGN_LEFT);
		  msg += "|15 " + getDateTime(vecUserList[Items[i].index].lastlogin);
		  npr(msg + "\r\n");
	          n++;
		  CheckForPause(stop);
		  if (stop==-1) break;
	  }	
	  
	}
	if (!ChatClient) line(LINE_BOTTOM);
	allowMsgs=1;

}

bool Connection::doModeratorCommand(string s, string olds) {
	int n;
	string msg;	
  if (isModerator()) {
	if (s.find("/ct") ==0) {
		if (s=="/ct") {
			npr(SYSS+"|07Current Topic: " +ChannelList[myNodeData.User.channel].getTopic()+SYSF);
			return true;
		}
		olds.erase(1,1);
		if (parseString(olds,&n, &msg, PARSE_STRING)==PARSE_STRING) {
			QInstantMessageRec myMsg;
			ChannelList[myNodeData.User.channel].setTopic(msg);
			ChannelList[myNodeData.User.channel].Save();
			myMsg.message="|03C|11" + IntToStr(myNodeData.User.channel) + "|07's new topic: \"";
			myMsg.message += ChannelList[myNodeData.User.channel].getTopic()+ "|07\"";
			Broadcast(myMsg);
		} else {
			npr(SYSS+ "Usage: /ct|12<s>"+SYSF);
		}
		return(true);
	} else if (s.find("/cn") ==0) {
		if (s=="/cn") {
			npr(SYSS+"|07Current Name: " +ChannelList[myNodeData.User.channel].getName()+SYSF);
			return true;
		}
		olds.erase(1,1);
		if (parseString(olds,&n, &msg, PARSE_STRING)==PARSE_STRING) {
			QInstantMessageRec myMsg;
			ChannelList[myNodeData.User.channel].setName(msg);
			ChannelList[myNodeData.User.channel].Save();
			myMsg.message="|03C|11" + IntToStr(myNodeData.User.channel) + "|07's new name: \"";
			myMsg.message += ChannelList[myNodeData.User.channel].getName()+ "|07\"";
			Broadcast(myMsg);
		} else {
			npr(SYSS+ "Usage: /cn|12<s>"+SYSF);
		}
		return(true);
	} else if (s.find("/li") ==0) {
		if (s != "/li") {
			npr(SYSS+"Usage: |11/li"+SYSF);
			return true;
		}
		if (ChannelList[getUser()->channel].getPublic()) {
			npr(SYSS+"This channel is public, everyone is invited."+SYSF);
			return true;
		} else {
			npr(SYSS+"Invited Nodes: |15");
			for (int i=0; i<64; i++) {
				if (ChannelList[getUser()->channel].getInvite(i)) {
					npr(IntToStr(i) + " ");
				}
			}
			npr(SYSF);
		}
		return true;
	} else if (s.find("/i") ==0) {
		s.erase(1,5);
		if (s=="/i") { npr(SYSS+"Usage: /i|10<n>|07"+SYSF); return true; } 
		if (parseString(olds, &n, &msg, PARSE_NODE) == PARSE_NODE) {
			if (n >= 999) {
				npr(SYSS+"Node number out of range" + SYSF);
			} else {
				int j;
				if (((j=GetFirstConnectionByNode(n)) != -1) && (connection[j]->getLoggedin())) {
					if (ChannelList[myNodeData.User.channel].getInvite(n)) {
						UnInvite(myNodeData.Nodenum, myNodeData.User.channel, n);
						npr(SYSS+"You have uninvited "+connection[j]->getNodeData().User.chathandle+"|07 from channel |03C|11"+IntToStr(myNodeData.User.channel)+SYSF);
					} else { 
						Invite(myNodeData.Nodenum, myNodeData.User.channel, n);
						npr(SYSS+"You have invited "+connection[j]->getNodeData().User.chathandle+"|07 to channel |03C|11"+IntToStr(myNodeData.User.channel)+SYSF);
					}
				} else 
					npr(SYSS+"Node does not exist"+SYSF);
			}
		} else {
			npr(SYSS+"Usage: /i|10<n>|07"+SYSF); 
		}
		return true;
	} else if (s.find("/ck") ==0) {
		olds.erase(1,1);
		if (parseString(olds,&n, &msg, PARSE_NODE)==PARSE_NODE) {
			int c;
			if (getUser()->channel==1) {
				npr(SYSS+"You cannot kick someone from channel |03C|111"+SYSF);
				return true;
			} 
			if ((c=GetFirstConnectionByNode(n)) != -1) {
				if (connection[c]->getUser()->channel == getUser()->channel) {
					QInstantMessageRec q;
					q.msgType=MSG_KICKED;
					connection[c]->msg.push_back(q);
					q.msgType=MSG_SYSTEM;
					q.message = "|10"+string(getUser()->chathandle) + "|07 has kicked |10"+string(connection[c]->getUser()->chathandle) + "|07 from channel |03C|11"+IntToStr(getUser()->channel);
					Broadcast(q);
				} else {
					npr(SYSS+"Node " +IntToStr(n) +" is not in your channel" + SYSF);
				}
			} else {
				npr(SYSS+"Node "+IntToStr(n)+" does not exist"+SYSF);
			}
		} else {
			npr(SYSS+"Usage: /ck|10<n>"+SYSF);
		}
		return true;	
	} else if (s.find("/cp") ==0) {
		if (myNodeData.User.channel == 1) {
			npr(SYSS+"Channel |03C|111|07 cannot be private"+SYSF);
		} else {
			ChannelList[myNodeData.User.channel].setPublic(ChannelList[myNodeData.User.channel].getPublic() == false);
			QInstantMessageRec myMsg;
			ChannelList[myNodeData.User.channel].Save();
			myMsg.message="|03C|11" + IntToStr(myNodeData.User.channel) + "|07 is now ";
			ChannelList[myNodeData.User.channel].clearInvites();
			myMsg.message += (ChannelList[myNodeData.User.channel].getPublic())?"Public.":"Private.";
			Broadcast(myMsg);
			if (!ChannelList[myNodeData.User.channel].getPublic()) {
				for (int i=0; i < connection.size(); i++) {
					QNodeDataRec nd=connection[i]->getNodeData();
					if (testbit(myNodeData.User.channel, nd.User.monitor)) {
						debug("CLEARBIT!!");
						clearbit(myNodeData.User.channel, connection[i]->getUser()->monitor);	
					}
				}
			}
		}
		return true;
	} else if (s.find("/cl") ==0) {
		if (s=="/cls") return false;
		if (myNodeData.User.channel == 1) {
			npr(SYSS+"You cannot lock channel |03C|111|07"+SYSF);
		} else {
			ChannelList[myNodeData.User.channel].setLocked(ChannelList[myNodeData.User.channel].getLocked() == false);
			QInstantMessageRec myMsg;
			ChannelList[myNodeData.User.channel].Save();
			myMsg.message="|03C|11" + IntToStr(myNodeData.User.channel) + "|07 is now ";
			myMsg.message += (ChannelList[myNodeData.User.channel].getLocked())?"locked.":"unlocked.";
			Broadcast(myMsg);
			if (ChannelList[myNodeData.User.channel].getLocked()) {
				for (int i=0; i < connection.size(); i++) {
					QNodeDataRec nd=connection[i]->getNodeData();
					if (testbit(myNodeData.User.channel, nd.User.monitor)) {
						debug("CLEARBIT!!");
						clearbit(myNodeData.User.channel, connection[i]->getUser()->monitor);	
					}
				}
			}
		}
  		return true;
	}
  }
  return(false);
}

bool Connection::doCosysopCommand(string s, string olds ) {
int n;
string msg;
  if (isCosysop()) {
	if (s.find("/syslock")==0) {
		if (s=="/syslock") {
			string newstr;
			if (sysinfo.locked > 0) 
				newstr = SYSS+"System is currently |12locked|07 at |10" + IntToStr(sysinfo.locked) + "|07 SL"+SYSF;
			else
				newstr = SYSS+"System is currently |15unlocked" + SYSF;

			npr(newstr);
		} else {
			s.erase(2,6);
			if (parseString(s,&n, &msg, PARSE_NODE) == PARSE_NODE) {
				if (n <= 0) {
					QInstantMessageRec Msg;
					sysinfo.locked=0;
					Msg.message="System is now |15unlocked|07";
					Broadcast(Msg);
				} else if (n > getSec()->security) {
					npr(SYSS+"System cannot be locked > |10"+IntToStr((int)getSec()->security)+"|07 SL" + SYSF);
				} else {

					QInstantMessageRec Msg;
					sysinfo.locked=n;
					Msg.message="System is now |12locked|07 at |10" + IntToStr(n) + "|07 SL";
					Broadcast(Msg);
				}
			} else {
				npr(SYSS+"Usage: |07/syslock |10<n>|07"+SYSF);
			}
		}
		return true;
	} else if (s.find("/cnfix") == 0) {
		int q;
		for (q=0; q<ChannelList.size(); q++) {
			if (q==1) q++;
			ChannelList[q].setName("-unnamed-");
			ChannelList[q].setTopic("-no topic-");
			ChannelList[q].Save();
		}
		npr(SYSS+"Channel names/topics have been fixed"+SYSF);
		return true;
	} else if (s.find("/medit") == 0) {
		if (s != "/medit") { npr(SYSS+"Usage: |11/medit"+SYSF); return true; }
		QInstantMessageRec msg;
		msg.message = "Message Editor";
		msg.msgType=MSG_LEFT;
		msg.lc=getSec()->lc;
		msg.nodeFrom=getNodenum();
		msg.rc=getSec()->rc;
		msg.channelFrom=getUser()->channel;
		msg.chathandle=string(getUser()->chathandle);
		BroadcastAllBut(getNodenum(), msg);
		writeLocation(LOC_MESSAGE_EDITOR);
		MessageEditor();
		npr("\r\n");
		writeLocation(LOC_CHATROOM);
		msg.msgType=MSG_RETURNED;
		msg.message="Message Editor";
		Broadcast(msg);
		return true;
	}
  }
  return false;
}


bool Connection::doSysopCommand(string s, string olds) {
int n,c;
string msg;
QNodeDataRec nd;
  if (isSysop()) {
	if (s.find("/clink")==0) {
		olds.erase(2,4);
		if (parseString(olds,&n,&msg, PARSE_NODE) == PARSE_NODE) {
			QLinkRec *newLink= new QLinkRec; // memoryleak:)
			if (LinkFile.Read(newLink, n)) {
				ClientLinkThread *CLThread;
				CLThread = new ClientLinkThread;
				CLThread->Start((void *)newLink);
			} else {
				npr("link does not exist!");
			}
		} else {
	
			npr(SYSS+"Usage: |03/clink|10<n>"+SYSF);
		}
		return true;	
	} else if (s.find("/bc") == 0) {
		olds.erase(1,1);
		if (parseString(olds,&n,&msg, PARSE_STRING) == PARSE_STRING) {

		}
	} else if (s.find("/rl")==0) {
		olds.erase(2,1);
		if (parseString(olds, &n, &msg, PARSE_NODE) == PARSE_NODE) {
			if (GetDataByNode(n, &nd)) {
				if (nd.Sec.network==1) {
					npr(SYSS+"You cannot relog a link hub"+SYSF);
				} else if (nd.Sec.security < getSec()->security) {
					c=GetFirstConnectionByNode(n);
					if (c != -1) {
						RelogNode(n);
						return true;
					}
				} else {
					npr(SYSS+"Node is protected"+SYSF);
				}
				return true;
		      	} else {
				npr(SYSS+ "Node |10"+IntToStr(n)+"|07 is currently unavailable"+SYSF);
				return true;
		      	}
		} else {
			npr(SYSS+"Usage: |11/rl|10<n>"+SYSF);
			return(true);
		}

	} else if (s.find("/k")==0)  {
		if (parseString(olds,&n,&msg, PARSE_NODE) == PARSE_NODE) {
			if (GetDataByNode(n, &nd)) {	
				if (nd.Sec.security < getSec()->security) { 
					c=GetFirstConnectionByNode(n);
					if (c != -1) {
						KillNode(n);
						return true;
					}
				} else {
					npr(SYSS+"Node is protected"+SYSF);
				}
				return true;
		      	} else {
				npr(SYSS+ "Node |10"+IntToStr(n)+"|07 is currently unavailable"+SYSF);
				return true;
		      	}
		} else {
			npr(SYSS+"Usage: |11/k|10<n>"+SYSF);
			return(true);
		}
	} else if (s.find("/syslog")==0) {
		if (s=="/syslog") {
			if (testbit(USERBIT_SYSLOG, getUser()->flags)) { 
				clearbit(USERBIT_SYSLOG, getUser()->flags);
				npr(SYSS+"Display system logs: |12disabled"+SYSF);
			} else { 
				setbit(USERBIT_SYSLOG, getUser()->flags);
				npr(SYSS+"Display system logs: |10enabled"+SYSF);
			}
			return true;
		} else {
			npr(SYSS+"Usage: |11/k|10<n>"+SYSF);
			return(true);
		}
	} else if (s.find("/shutdown")==0) {
		if (s == "/shutdown") 
		  sysinfo.shutdown=1;
		else 
			npr(SYSS+"Usage: |03/shutdown"+SYSF);
	  return true;
	  } else if (s.find("/spawn")==0) {
		s.erase(2,4);
		if (parseString(s, &n, &msg, PARSE_NODE) == PARSE_NODE) {
			if (getNodenum() > 0) { 
				showmessage(getNodenum(), FLAG_LOGOUT);
			}
			setNodenum(n);
			if (n > 0) { 
				showmessage(getNodenum(), FLAG_LOGIN);
			}
		} else {
			npr(SYSS+"Usage: |11/spawn|10<n>"+SYSF);
		}
	return(true);
      } else if (s.find("/bedit")==0) {
	   if (s != "/bedit") { npr(SYSS+"Usage: |11/bedit"+SYSF); return true; }
           QInstantMessageRec msg;
	   msg.message = "Ban Editor";
	   msg.msgType=MSG_LEFT;
	   msg.lc=getSec()->lc;
	   msg.nodeFrom=getNodenum();
	   msg.rc=getSec()->rc;
	   msg.channelFrom=getUser()->channel;
	   msg.chathandle=string(getUser()->chathandle);
	   BroadcastAllBut(getNodenum(), msg);
	   writeLocation(LOC_BAN_EDITOR);
           BanEditor();
	   npr("\r\n");
	   writeLocation(LOC_CHATROOM);
           msg.msgType=MSG_RETURNED;
	   msg.message="Ban Editor";
	   Broadcast(msg);
           return(true);
      } else if (s.find("/uedit")==0) {
	   if (s != "/uedit") { npr(SYSS+"Usage: |11/uedit"+SYSF); return true; }
           QInstantMessageRec msg;
	   msg.message = "User Editor";
	   msg.msgType=MSG_LEFT;
	   msg.lc=getSec()->lc;
	   msg.nodeFrom=getNodenum();
	   msg.rc=getSec()->rc;
	   msg.channelFrom=getUser()->channel;
	   msg.chathandle=string(getUser()->chathandle);
	   BroadcastAllBut(getNodenum(), msg);
	   writeLocation(LOC_USER_EDITOR);
           UserEditor();
	   npr("\r\n");
	   writeLocation(LOC_CHATROOM);
           msg.msgType=MSG_RETURNED;
	   msg.message="User Editor";
	   Broadcast(msg);
	   return(true);
      } else if (s.find("/ledit")==0) {
	   if (s != "/ledit") { npr(SYSS+"Usage: |11/ledit"+SYSF); return true; }
           QInstantMessageRec msg;
	   msg.message = "Link Editor";
	   msg.msgType=MSG_LEFT;
	   msg.lc=getSec()->lc;
	   msg.channelFrom=getUser()->channel;
	   msg.nodeFrom=getNodenum();
	   msg.rc=getSec()->rc;
	   msg.chathandle=string(getUser()->chathandle);
	   BroadcastAllBut(getNodenum(), msg);
	   writeLocation(LOC_LINK_EDITOR);
           LinkEditor();
	   npr("\r\n");
	   writeLocation(LOC_CHATROOM);
           msg.msgType=MSG_RETURNED;
	   msg.message="Link Editor";
	   Broadcast(msg);
	   return(true);
      } // else if () {
    } // isAdmin 
  return(false);
}

bool Connection::doOwnerCommand(string s, string olds) {
	if (isOwner()) {
	}
	return false;
}

bool Connection::doAdminCommand(string s, string olds) {
string msg;
int n;
char s2[220];
  if (isAdmin()) {
	  if (s.find("/debug") == 0) {
		s.erase(2,4);
		if (parseString(s,&n, &msg, PARSE_NODE) == PARSE_NODE) {
           		QInstantMessageRec msg;
	 	 	msg.message = "Debug";
			msg.msgType=MSG_LEFT;
	   		msg.lc=getSec()->lc;
	   		msg.nodeFrom=getNodenum();
	   		msg.rc=getSec()->rc;
	   		msg.channelFrom=getUser()->channel;
	   		msg.chathandle=string(getUser()->chathandle);
	   		BroadcastAllBut(getNodenum(), msg);
	   		writeLocation(LOC_OTHER);
			CommandDebug(n);
	   		npr("\r\n");
	   		writeLocation(LOC_CHATROOM);
           		msg.msgType=MSG_RETURNED;
	   		msg.message="Debug";
	   		Broadcast(msg);
	  		 return(true);
		} else {
			npr(SYSS+"Usage: |11/debug |10<n>"+SYSF);
		}
		return true;

      } else if (s.find("/sedit") == 0) {
	   if (s != "/sedit") { 
		   npr(SYSS+"Usage: |11/sedit"+SYSF); 
		   return true; 
	   }
           QInstantMessageRec msg;
	   msg.message = "Security Editor";
	   msg.msgType=MSG_LEFT;
	   msg.lc=getSec()->lc;
	   msg.channelFrom=getUser()->channel;
	   msg.rc=getSec()->rc;
	   msg.nodeFrom=getNodenum();
	   msg.chathandle=string(getUser()->chathandle);
	   BroadcastAllBut(getNodenum(), msg);
	   writeLocation(LOC_SECURITY_EDITOR);
           SecurityEditor();
	   npr("\r\n");
	   writeLocation(LOC_CHATROOM);
           msg.msgType=MSG_RETURNED;
	   msg.message="Security Editor";
	   Broadcast(msg);
	   return(true);

      } else if (s.find("/sysedit") == 0) {
	   if (s != "/sysedit") { 
		   npr(SYSS+"Usage: |11/sysedit"+SYSF); 
		   return true; 
	   }
           QInstantMessageRec msg;
	   msg.message = "System Editor";
	   msg.msgType=MSG_LEFT;
	   msg.lc=getSec()->lc;
	   msg.channelFrom=getUser()->channel;
	   msg.rc=getSec()->rc;
	   msg.nodeFrom=getNodenum();
	   msg.chathandle=string(getUser()->chathandle);
	   BroadcastAllBut(getNodenum(), msg);
	   writeLocation(LOC_SYSTEM_EDITOR);
           SystemEditor();
	   npr("\r\n");
	   writeLocation(LOC_CHATROOM);
           msg.msgType=MSG_RETURNED;
	   msg.message="System Editor";
	   Broadcast(msg);
	   return(true);

     }
  }
  return false;
}

bool HandleAllowed(string s) {

	if (s == "|") return false;
	if (s == "||") return false;
	if (trailingpipe(s)) return false;
	if (stripcolors(s).size()==0) return false;
	
	return true;
}

void Connection::doCommand(string s, string olds) {
char s2[220];
string msg;
int n;
QNodeDataRec nd;
	
	if (s=="/mail") {
		if (getGuest()) {
			npr(SYSS+"You must have an account to access mail." + SYSF);
			return;
		} else {
           		QInstantMessageRec msg;
			msg.message = "Mail";
			msg.msgType=MSG_LEFT;
			msg.lc=getSec()->lc;
			msg.nodeFrom=getNodenum();
			msg.rc=getSec()->rc;
	   		msg.channelFrom=getUser()->channel;
	   		msg.chathandle=string(getUser()->chathandle);
	   		BroadcastAllBut(getNodenum(), msg);
	   		writeLocation(LOC_MAIL);
			Mail();
	   		npr("\r\n");
			msg.message = "Mail";
			msg.msgType=MSG_RETURNED;
	   		writeLocation(LOC_CHATROOM);
	   		Broadcast(msg);
		}
	} else if (s=="/c") {
		npr(Center("L i s t i n g   C h a n n e l s") + "\r\n");
		line(LINE_TOP);
			npr("    |03C|11#|07  Name           Pop |15Lock Status|07  Topic\r\n\n");
			int i=1;
			if (isSysop()) i=0;
			for (; i<=9; i++) {
				string s="    |03C|11" + IntToStr(i) + "|07  " + spacecolors(ChannelList[i].getName(),15, ALIGN_LEFT);
				s += " " +IntToStr(ChannelCount(i),2) + "  ";
				s += (ChannelList[i].getLocked())?"|12Yes":"|10No ";
				s += "  ";
				s += (ChannelList[i].getPublic())?"|10Pub|07":"|12Pri|07";
				s+= "|07";
				s+= "    " + ChannelList[i].getTopic() + "\r\n";
				npr(s);
			}
						
		line(LINE_BOTTOM);
		npr("\r\n");
	} else if (s == "/away" ) {
          if (getAway() == false) {
            npr(SYSS+"Usage: |11/away|12<s>"+SYSF);
	  } else {
            QInstantMessageRec Msg;
	    Msg.msgType=MSG_NOTAWAY;
	    Msg.nodeFrom=getNodenum();
	    Msg.chathandle=getUser()->chathandle;
	    Msg.channelFrom=getUser()->channel;
	    Msg.priority=PRIORITY_SYSTEM;
	    Msg.lc=getSec()->lc;
	    Msg.rc=getSec()->rc;
	    Msg.message=""; 
	    Broadcast(Msg);
	    line(LINE_TOP);
	    snpr("|07Away Message: |10%s|07\r\n", getAwaymsg().c_str());
	     npr("   |07Time Away: |10" + getTotalTime(timeDifference(myNodeData.Awaytime, now())) + "|07\r\n\n");
             msg = "./tmp/" + string(getUser()->loginname);
	     if (exist(msg)) {
		if (!ChatClient) {
		    	npr("|07You have received these private messages in your absense:\r\n");
			printfile(msg);
		}
		unlink(msg.c_str());
	     } else {
               npr("|07You haven't received any private messages in your absence.\r\n");
	     }
	    line(LINE_BOTTOM);
	    setAway(false);
	  }
	} else if (s.find("/bbs") == 0) {
		if (s != "/bbs") 
			npr(SYSS+"|07Usage: |11/bbs"+SYSF);
		else {
           		QInstantMessageRec msg;
			msg.message = "BBS";
			msg.msgType=MSG_LEFT;
			msg.lc=getSec()->lc;
			msg.nodeFrom=getNodenum();
			msg.rc=getSec()->rc;
	   		msg.channelFrom=getUser()->channel;
	   		msg.chathandle=string(getUser()->chathandle);
	   		BroadcastAllBut(getNodenum(), msg);
	   		writeLocation(LOC_BBS);
           		MainMenu();
	   		npr("\r\n");
	   		writeLocation(LOC_CHATROOM);
           		msg.msgType=MSG_RETURNED;
	   		msg.message="BBS";
	   		Broadcast(msg);
	   		return;
		}
	} else if (s.find("/away") == 0) {
		if (!getAway()) {
		    olds.erase(2,3); 
		    if (parseString(olds, &n, &msg, PARSE_STRING) == PARSE_STRING) {
		    if (msg.length() >= 42) msg=msg.substr(0, 40);
            	    QInstantMessageRec Msg;
		    Msg.msgType=MSG_AWAY;
		    Msg.chathandle=getUser()->chathandle;
		    Msg.priority=PRIORITY_SYSTEM;
		    Msg.nodeFrom=getNodenum();
	    	    Msg.channelFrom=getUser()->channel;
		    Msg.lc=getSec()->lc;
		    Msg.rc=getSec()->rc;
		    Msg.message=msg; 
		    Broadcast(Msg);
		    setAway(true, msg);
		  } else {
		    npr(SYSS+"Usage: |11/away|12<s>"+SYSF);
		  }
		} else {
		  npr(SYSS+"You are already away. Type |11.away|07 by itself to return"+SYSF);
		}
	} else if (s.find("/config") == 0) {
		if (s=="/config") {
           		QInstantMessageRec msg;
			msg.message = "Config";
			msg.msgType=MSG_LEFT;
			msg.lc=getSec()->lc;
			msg.nodeFrom=getNodenum();
			msg.rc=getSec()->rc;
	   		msg.channelFrom=getUser()->channel;
	   		msg.chathandle=string(getUser()->chathandle);
	   		BroadcastAllBut(getNodenum(), msg);
	   		writeLocation(LOC_USER_CONFIG);
           		UserConfig();
	   		npr("\r\n");
	   		writeLocation(LOC_CHATROOM);
           		msg.msgType=MSG_RETURNED;
	   		msg.message="Config";
	   		Broadcast(msg);
	   		return;
		} else {
        		npr(SYSS+"Usage: |11/config"+SYSF);
		}
	} else if (s.find("/arphaxad") == 0) {
		if (s=="/arphaxad") {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.message="hoot!";
			Msg.channelFrom=getUser()->channel;
			Msg.channelTo=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.nodeFrom=getNodenum();
			Msg.rc=getSec()->rc;
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Broadcast(Msg);
		} else {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
		}
	} else if (s.find("/qorey") == 0) {
		if (s=="/qorey") {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.message="I AM JESUS, GIVE ME OPS!";
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.nodeFrom=getNodenum();
			Msg.rc=getSec()->rc;
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Broadcast(Msg);
		} else {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
		}
	} else if (s.find("/trout") == 0) {
		if (s=="/trout") {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.message="yeargh!";
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.nodeFrom=getNodenum();
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Broadcast(Msg);
		} else {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
		}
	} else if (s.find("/sam") == 0) {
		if (s=="/sam") {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.nodeFrom=getNodenum();
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Msg.message = "master says I am ready, once I confront my inner potato."; 
			Broadcast(Msg);
		} else {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
		}
	} else if (s.find("/gina") == 0) {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
	} else if (s.find("/s0ren") == 0) {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
	} else if (s.find("/clockworkbear") == 0) {
		if (s=="/clockworkbear") {

		} else {

		}
        	npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
	} else if (s.find("/jiny") == 0) {
		if (s == "/jiny") {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelFrom=getUser()->channel;
			Msg.channelTo=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.nodeFrom=getNodenum();
			Msg.rc=getSec()->rc;
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			if (randomnumber(2) == 1) Msg.message = "MR DOOD!"; 
			else Msg.message="what the sam hell!";
			Broadcast(Msg);
		} else {
        		npr("|08-|04=|12- |07Invalid command, type |14/?|07 for a list of commands|07\r\n");
		}
	} else if (s.find("/jeffie") == 0) {
	    if (!getAway()) {
	      olds.erase(2,5);
	      if (parseString(olds, &n, &msg, PARSE_NODE)==PARSE_NODE) {
	        	if (n < 5) n=5;
		        int p;
			int q;
			if (n > 100) n=150;
			
			for (p=0; p<n; p++) {
				q=randomnumber(3);
				switch(q) {
					case 1: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'e') 
								msg+='h';
							else msg+='e';
						} else msg += 'e'; 
						break;
					case 2: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'h') 
								msg+='a';
							else msg+='h';
						} else msg += 'e'; 
						break;
					case 3: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'a') 
								msg+='h';
							else msg+='a';
						} else msg += 'a'; 
						break;
				}
			}
			
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.nodeFrom=getNodenum();
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Msg.message=msg + "!";
			Broadcast(Msg);
	      } else {
	     		n=randomnumber(5)+3;
			for (int p=0; p<n; p++) {
				int q=randomnumber(3);
				switch(q) {
					case 1: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'e') 
								msg+='h';
							else msg+='e';
						} else msg += 'e'; 
						break;
					case 2: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'h') 
								msg+='a';
							else msg+='h';
						} else msg += 'e'; 
						break;
					case 3: 
						if (msg.length()) {
							if (msg[msg.length()-1] == 'a') 
								msg+='h';
							else msg+='a';
						} else msg += 'a'; 
						break;
				}
			}
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.nodeFrom=getNodenum();
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_CHAT;
			Msg.message=msg + "!";
			Broadcast(Msg);
	      }
	    } else {
 	      npr(SYSS+"Command not available in |10away|07 mode"+SYSF);
	    }
	} else if (s.find("/me2") == 0) {
	    if (!getAway()) {
	      olds.erase(2,2);
	      if (parseString(olds, &n, &msg, PARSE_STRING)==PARSE_STRING) {
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelFrom=getUser()->channel;
			Msg.channelTo=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.nodeFrom=getNodenum();
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_ACTION2;
			Msg.message=msg;
			Broadcast(Msg);
	      } else {
		npr(SYSS+"Usage: |11/me2|12<s>"+SYSF);
	      }
	    } else {
 	      npr(SYSS+"Command not available in |10away|07 mode"+SYSF);
	    }
	} else if (s.find("/si") == 0) {
	    if (s != "/si") { npr(SYSS+"Usage: |11/si|07"+SYSF); return; }
	    snpr("|15%s|07\r\n",Center("System Information").c_str());
	    line(LINE_TOP);
#ifdef OPENBSD
	    sprintf(s2,"|15Millenium2 v|101.0.300r|07");
	    snpr("%s\r\n", spacecolors(s2, 77, ALIGN_RIGHT).c_str());
	    snpr("|07   Operating System: |15OpenBSD|07\r\n\n");

#else
	    sprintf(s2,"|15Millenium2 v|10%s|07", VERSION);
	    snpr("%s\r\n", spacecolors(s2, 77, ALIGN_RIGHT).c_str());
	    snpr("|07   Operating System: |15%s|07\r\n\n", OS);
#endif
	    snpr("     |07System Started: |15%s\r\n",getDateTime(sysinfo.firststarted).c_str());
	    snpr("      |07System Uptime: |15%s\r\n", getTotalTime(timeDifference(sysinfo.uptime, now())).c_str());
	    snpr("|07       Users Online: |15%d\r\n", connection.size());
	    snpr("|07       Total Logins: |15%u\r\n", sysinfo.totallogins);
	    snpr("|07       Active Users: |15%d\r\n",activeUsers());
	     npr("|07System is currently: ");
	     if (sysinfo.locked != 0) snpr("|12Locked|07 (|10%d|07)\r\n",sysinfo.locked);
	     else npr("|15Unlocked|07\r\n");
	    line(LINE_BOTTOM);
	} else if (s.find("/me") == 0) {
	    if (!getAway()) {
	      olds.erase(2,1);
	      if (parseString(olds, &n, &msg, PARSE_STRING)==PARSE_STRING) {
	        sprintf(s2, "|08#%02d|07:|03C|11%d|10%c|07%s|07 ",getNodenum(),getUser()->channel,getSec()->lc,getUser()->chathandle);
			QInstantMessageRec Msg;
			Msg.chathandle=getUser()->chathandle;
			Msg.channelTo=getUser()->channel;
			Msg.channelFrom=getUser()->channel;
			Msg.lc=getSec()->lc;
			Msg.rc=getSec()->rc;
			Msg.nodeFrom=getNodenum();
			Msg.priority=PRIORITY_CHAT;
			Msg.msgType=MSG_ACTION;
			Msg.message=msg;
			Broadcast(Msg);
	      } else {
		npr(SYSS+"Usage: |11/me|12<s>"+SYSF);
	      }
	    } else {
 	      npr(SYSS+"Command not available in |10away|07 mode"+SYSF);
	    }
	} else if (s.find("/mon") == 0) {
	    olds.erase(1,2);
	    if (parseString(olds, &n, &msg, PARSE_NODE) != PARSE_ERROR) {
	      if (n < 10) {
		if (n==0) {
		  if (isSysop()) {
			
			if (getMonitor(n)) clearbit(n, myNodeData.User.monitor);
			else if ((ChannelList[n].getPublic() == false) || (ChannelList[n].getLocked())) {
				npr(SYSS+"You cannot monitor locked or private channels"+SYSF);
			} else setbit(n, myNodeData.User.monitor);	
		  } else {
		    npr(SYSS+"Channel does not exist!"+SYSF);
		  }
		} else {
			if (getMonitor(n)) clearbit(n, myNodeData.User.monitor);
			else if ((ChannelList[n].getPublic() == false) || (ChannelList[n].getLocked())) {
				npr(SYSS+"You cannot monitor locked or private channels"+SYSF);
			} else setbit(n, myNodeData.User.monitor);
		}
	      } else {
		npr(SYSS+"Channel does not exist!"+SYSF);
	      }
	    } 
            msg = SYSS+"Currently monitoring channels: |15";
	    unsigned int x = msg.length();
	    for (int j=0; j < 10; j++) {
		    if (getMonitor(j)) { sprintf(s2, "%d ",j); msg += string(s2); }
	    }
	    if (x == msg.length()) msg +="|10(none)";
	    msg += SYSF;
	    npr(msg);
	} else if (s.find("/m")==0) {
          if (s.size() == 2) {
		npr(SYSS+"Usage: |11/m|12<s>|07  |11/m|12list|07 for a list of messages"+SYSF);
	  } else {
	     if (parseString(olds,&n, &msg,PARSE_STRING)==PARSE_STRING) {
               if (ValidFN(msg)) {
	       		if (exist("./msgs/" + msg)) { 
	         		printfile("./msgs/" + msg);
	       		} else {
				 npr(SYSS+"Message |10" + msg + "|07 does not exist"+SYSF);
	       		}
	       } else {
			npr(SYSS+"Illegal characters in message name"+SYSF);
	       }
	     } else {
		 npr(SYSS+"Usage: |11/m|12<s>|07   |11/m|12list|04 for a list of messages"+SYSF);
	     }
	  }
	} else if (s.find("/doing") == 0) {
	   olds.erase(2,4);
	   if (parseString(olds, &n, &msg, PARSE_STRING)==PARSE_STRING) {
                if (toLower(msg) == "clear") {
                  npr(SYSS+"|07Doing |10cleared"+SYSF);
		  strcpy(getUser()->doing, "");
		} else {
		  while (stripcolors(msg).length() > 34) msg.erase(msg.length()-1,1);
		  while (msg.length() > 59) msg.erase(msg.length()-1,1);
		  npr(SYSS+"Doing set to: \"|10" + msg + "|07\""+SYSF);
		  strcpy(getUser()->doing, msg.c_str());
		}
	   } else {
             npr(SYSS+"Usage: |11/doing|12<s>|07"+SYSF);
	   }
	} else if (s.find("/cls") == 0) {
          if (s != "/cls") { npr(SYSS+"Usage: |11/cls"+SYSF); return; }
	  cls();
	} else if (s.find("/echo")==0) {
	  if (s != "/echo") { npr(SYSS+"Usage: |11/echo"+SYSF); return; }
          getUser()->echo = (getUser()->echo == false);
	  if (getUser()->echo)
	    npr(SYSS+"Echoing is |10enabled"+SYSF);
	  else
	    npr(SYSS+"Echoing is |12disabled"+SYSF);

	} else if (s.find("/sound")==0) {
	  if (s != "/sound") { npr(SYSS+"Usage: |11/sound"+SYSF); return; }
          getUser()->sound = (getUser()->sound == false);
	  if (getUser()->sound)
	    npr(SYSS+"|07Sounds are |10enabled"+SYSF);
	  else
	    npr(SYSS+"|07Sounds are |12disabled"+SYSF);

	} else if (s.find("/ansi")==0) {
	  if (s != "/ansi") { npr(SYSS+"Usage: |11/ansi"+SYSF); return; }
          getUser()->ansi = (getUser()->ansi == false);
	  if (getUser()->ansi)
	    npr(SYSS+"|14A|13N|12S|11I |07color graphics |12enabled|07"+SYSF);
	  else
	    npr(SYSS+"ANSI color graphics disabled"+SYSF);
		  
	} else if (s.find("/h") == 0) {
	   if (parseString(olds,&n,&msg,PARSE_STRING)==PARSE_STRING) {
	      if (stripcolors(msg).length() == 0) {
		npr(SYSS+"Invalid choice for a handle, you |10doof"+SYSF);
		return;
	      } 
	      if (stripcolors(msg).length() > 15) {
                npr(SYSS+"Length of handle must be less than |1015|07 characters"+SYSF);
		return;
	      } else if (msg.length() > 50) {
                npr(SYSS+"Length of handle including colors must not exceed |1050|07 characters"+SYSF);
	        return;
	      } else {
		strcpy(getUser()->chathandle, msg.c_str());
                sprintf(s2,"%sYour handle has been changed to \"|10%s|07\"%s",SYSS.c_str(), getUser()->chathandle,SYSF.c_str());
		npr(s2);
              }

	   } else {
	     npr(SYSS+"Usage: |11/h|12<s>"+SYSF);
	   }
      } else if (s.find("/passwd")==0) {
      	olds.erase(2,5);
        if (parseString(olds,&n,&msg, PARSE_STRING) == PARSE_STRING) {
          if (stripcolors(msg).length() <= 20) {
            sprintf(s2, "%sYour password has been changed to '|08%s|07'%s",SYSS.c_str(),stripcolors(msg).c_str(),SYSF.c_str());
	    npr(s2);
	    strcpy(getUser()->password, stripcolors(msg).c_str());
	  } else npr(SYSS+"Your password cannot exceed |1020|07 characters"+SYSF); 
	} else npr(SYSS+"Usage: |11/passwd|12<s>"+SYSF);
      } else if (s.find("/p") == 0) {
	if (!getAway()) {
	      	if (parseString(olds,&n,&msg, PARSE_NODESTRING)) {
			sprintf(s2, "|12P%s ",chatheader().c_str());
	       		string newmsg = s2 + msg + "\r\n";
			vi con;
			int conn=GetConnectionByNode(n,con);
			
			if (conn != -1) {
			  for (int z=0; z< con.size(); z++) {
				conn=con[z];
				debug("(/p) conn=" + IntToStr(conn));
				nd = connection[conn]->getNodeData();
			
				if ((nd.Sec.network == 1) && (nd.Nodenum == n)) {
					npr(SYSS+"You can't send a private message to a Link"+SYSF);
					return;
				} else if (nd.Sec.network == 1) {
					for (int j=0; j<connection[conn]->Network.size(); j++) {
						if (connection[conn]->Network[j]->Nodenum==n) {
							QInstantMessageRec Msg;
							Msg.msgType=MSG_PRIVATE;
							Msg.priority=PRIORITY_PRIVATE;
							Msg.message=msg;
							Msg.chathandle=getUser()->chathandle;
							Msg.lc = getSec()->lc;
							Msg.rc = getSec()->rc;
							Msg.nodeFrom=getNodenum();
							Msg.nodeTo=n;
							Msg.channelFrom=getUser()->channel;
							Broadcast(Msg);
							npr(SYSS+"Private message sent to '|15"+connection[conn]->Network[j]->User.chathandle+"|07'"+SYSF);
							break;
						}
					}
				} else if ((nd.Loggedin) && (nd.Location == LOC_CHATROOM)) {
					QInstantMessageRec Msg;
					Msg.msgType=MSG_PRIVATE;
					Msg.priority=PRIORITY_PRIVATE;
					Msg.message=msg;
					Msg.chathandle=getUser()->chathandle;
					Msg.lc = getSec()->lc;
					Msg.rc = getSec()->rc;
					Msg.nodeFrom=getNodenum();
					Msg.nodeTo=n;
					Msg.channelFrom=getUser()->channel;
					Broadcast(Msg);
					sprintf(s2,"%sPrivate message sent to '|15%s|07'%s",SYSS.c_str(), nd.User.chathandle, SYSF.c_str());
					npr(s2);
					break;
				} else {
					sprintf(s2,"%sNode %d is unavailable%s",SYSS.c_str(),n,SYSF.c_str());
					npr(s2);
				}
			  }
			} else {
				sprintf(s2,"%sNode %d does not exist%s",SYSS.c_str(),n,SYSF.c_str());
				npr(s2);
			}
      		} else {
			npr(SYSS+"Usage: |11/p|10<n>|12<s>"+SYSF);
		}
         } else {
           npr(SYSS+"Command not available in |10away|07 mode"+SYSF);
         }

      } else if (s.find("/l")==0) {
	if (s != "/l") { npr(SYSS+"Usage: |11/l"+SYSF); return; }
        ListUsers();
      } else if (s.find("/q")==0) {
	if (s != "/q") { npr(SYSS+"Usage: |11/q"+SYSF); return; }
	logout(FLAG_LOGOUT);
      } else if (s.find("/ver")==0) {
	if (s != "/ver") { npr(SYSS+"Usage: |11/ver"+SYSF); return; }
	snpr("|15%s\r\n",Center("Version").c_str());
	line(LINE_TOP);
#ifdef OPENBSD
	snpr("|15Millenium2|07 Chat Server Software |15v|101.0.300r\r\n");
	npr("|15      (c) |07Cory Kratz 1999-2003\r\n");
	snpr("\r\n|07Operating System: |10OpenBSD|07\r\n");
#else
	snpr("|15Millenium2|07 Chat Server Software |15v|10%s\r\n",VERSION);
	npr("|15      (c) |07Cory Kratz 1999-2003\r\n");

#endif
#ifdef OS
	snpr("\r\n|07Operating System: |10%s|07\r\n",OS);
#endif
	line(LINE_BOTTOM);
	npr("|07");
      } else if (s.find("/?")==0) {
		if (s != "/?") { npr(SYSS+"Usage: |11/?"+SYSF); return; }
        	int stop=0;
		allowMsgs=0;
		if (isAdmin()) printfilepause("./etc/admin.commandlist", stop);
		if (isSysop()) printfilepause("./etc/sysop.commandlist", stop);
		if (isCosysop()) printfilepause("./etc/cosysop.commandlist", stop);
		if ((isModerator()) || (myNodeData.TempModerator)) printfilepause("./etc/moderator.commandlist", stop);
		if (!getGuest()) printfilepause("./etc/user.commandlist", stop);
		else printfilepause("./etc/guest.commandlist", stop);
		npr("|07");
	allowMsgs=1;
      } else if (s.find("/c")==0) {
	if (!getAway()) {
          msg = olds.substr(2,olds.length()-2);
	  if (msg.length() == 0) {
            npr(SYSS+"Usage: |11/c|10<n> |07.|12<s>|07."+SYSF);
	  } else if (msg.length() == 1) {
	    if (!isdigit(msg[0])) {
              npr(SYSS+"Usage: |11/c|10<n> |07.|12<s>|07."+SYSF);
	    } else {
              if (atoi(msg.c_str()) == getUser()->channel) {
 		npr(SYSS+"You are already in that channel, you |10doof|07!"+SYSF);
	      } else {
	        if (atoi(msg.c_str()) > 9) {
                  npr(SYSS+"Channels are only one digit (|100-9|07)"+SYSF);
	        } else {
		  int v=StrToInt(msg);
		  if (ChannelList[v].getLocked()) {
			npr(SYSS+"That channel is currently locked."+SYSF);
			return;
		  } else if (ChannelList[v].getPublic()==false) {
			if (!ChannelList[v].getInvite(myNodeData.Nodenum)) {
				npr(SYSS+"That channel is private, and you were not invited to join it"+SYSF);
				return; 
			}
		  }
		  if (ChannelCount(getUser()->channel)) {
			  ChannelList[getUser()->channel].setPublic(true);
			  ChannelList[getUser()->channel].setLocked(false);
		  }
		  myNodeData.TempModerator=false;
		  QInstantMessageRec Msg;
		  Msg.chathandle=getUser()->chathandle;
		  Msg.message="has left channel |03C|11" + IntToStr(getUser()->channel);
	          Msg.channelFrom=getUser()->channel;
		  Msg.channelTo=getUser()->channel;
		  Msg.lc=getSec()->lc;
		  Msg.nodeFrom=getNodenum();
		  Msg.rc=getSec()->rc;
		  Msg.priority=PRIORITY_SYSTEM;
		  Msg.msgType=MSG_SYSTEM;
		  BroadcastAllBut(getNodenum(),Msg);
	          getUser()->channel = atoi(msg.c_str());
                  Msg.message="|07is now in channel |03C|11"+msg;
		  Msg.channelFrom=getUser()->channel;
		  Msg.channelTo=getUser()->channel;
	          Broadcast(Msg);
		  if ((getUser()->channel != 1) && (ChannelCount(getUser()->channel) == 1) && (!getGuest())) {
		 	QInstantMessageRec m;
			m.message="You are the temporary channel moderator for channel |03C|11" + IntToStr(getUser()->channel);
			this->msg.push_back(m);
			myNodeData.TempModerator=true;

		  }
                }
	      }
	    }
	  } else {
            if (parseString(olds,&n,&msg, PARSE_NODESTRING) == PARSE_NODESTRING) {
	      if (n < 10) {
	        if (n == getUser()->channel) {
                  npr(SYSS+"You are already in that channel, you |10doof|07!"+SYSF);
	        } else {
		  QInstantMessageRec Msg;
		  Msg.chathandle=getUser()->chathandle;
		  Msg.message=msg;
		  Msg.channelFrom=getUser()->channel;
	          Msg.channelTo=n;
		  Msg.lc=getSec()->lc;
		  Msg.rc=getSec()->rc;
		  Msg.nodeFrom=getNodenum();
		  Msg.priority=PRIORITY_CHAT;
		  Msg.msgType=MSG_CHANNEL;
	          Broadcast(Msg);
		  sprintf(s2,"%sChannel message sent to channel |03C|11%d%s",SYSS.c_str(),n,SYSF.c_str());
		  npr(s2);
	        }
	      } else {
	        npr(SYSS+"Channels are only one digit. (|100-9|07)"+SYSF);
	      }
	    } else {
	      if (n > 9) 
                npr(SYSS+"Channels are only one digit. (|100-9|07)"+SYSF);
	      else 
                npr(SYSS+"Usage: |11/c|10<n> |07.|12<s>|07."+SYSF);
	    }
	  }
        } else {
          npr(SYSS+"Command not available in |10away|07 mode"+SYSF);
        }
      } else if (s.find("/whoami")==0) {
	if (s != "/whoami") { npr(SYSS+"Usage: |11/whoami"+SYSF); return; }
        snpr("|08-=|07-=|15-=[ Who Am I ]|=-|07=-|08=-=-=-=-=-=-=-=|08-|15[|07 Node #|15%02d |15]|07-|07=|08-\r\n",getNodenum());
	securityname(getNodenum(), s2);
        snpr("|07      Login Name: %s|07\r\n",getUser()->loginname);
        snpr("|07     Chat Handle: |07%s |07%s|14 #%03d|07\r\n\n",getUser()->chathandle,s2,getUser()->number);
        snpr("|07Chatroom Channel: |02C|10%d\r\n",getUser()->channel);
        line(LINE_BOTTOM);
      } else if (s.find("/whois")==0) {
        s.erase(2,4);
	if (parseString(s,&n,&msg, PARSE_NODE) == PARSE_NODE) {
		if (GetDataByNode(n, &nd) == false) {
			sprintf(s2,"%sNode |10%d|07 is not online.%s",SYSS.c_str(),n,SYSF.c_str());
			npr(s2);
    	  	} else {
	  		if (!nd.Loggedin) {
				sprintf(s2,"%sNode |10%d|07 is unavailable.%s",SYSS.c_str(),n,SYSF.c_str());
	  		} else {
        			snpr("-=|08-=|07-=|15-=[ Who is ]=-|07=-|08=-=-=-=-=|07-=|15-=[ |07Node #|15%02d ]=-|07=-|08=-\r\n",n);
	        		securityname(n, s2);
				snpr("|07      Login Name: %s|07\r\n",nd.User.loginname);
				snpr("|07     Chat Handle: |07%s |07%s|15 #%03d|07\r\n\n",nd.User.chathandle,s2,nd.User.number);
				snpr("|07Chatroom Channel: |02C|10%d\r\n",nd.User.channel);
				snpr("|07           Doing: |10%s\r\n", nd.User.doing);
				if (nd.Away) snpr("   |07Away Message: |10%s\r\n",nd.Awaymsg.c_str());
	        		line(LINE_BOTTOM);
			}
		}
	} else {
		npr(SYSS+"Usage: |11/whois|10<n>"+SYSF);
	}

      } else if ((s.find("/sec") == 0) || (s.find("/who") == 0)) {
		if (s != "/sec") { npr(SYSS+"Usage: |11/sec"+SYSF); return; }
      		int i=1;
		QSecurityRec sectemp;
		QSort sTemp;
		vector<QSort > sortVec;
		debug("Loading all securitys...");
		while (SecurityFile.Read(&sectemp, i)) {
			sTemp.index=i;
			sTemp.sortedItem=(int)sectemp.security;
			sortVec.push_back(sTemp);
			i++;
		}
		SelectionSort(&sortVec);
		snpr("|15%s\r\n",Center("Security Level List").c_str());	
		line(LINE_TOP);
		npr(" |15Name                 |10CC  |14Level\r\n");
		while (sortVec.size()) {
			SecurityFile.Read(&sectemp,sortVec[sortVec.size()-1].index);
			snpr(" |15%s |10%c%c  |14%03d\r\n",spacecolors(sectemp.name,20,ALIGN_LEFT).c_str(), sectemp.lc,sectemp.rc, sectemp.security);
			sortVec.erase(sortVec.begin()+(sortVec.size()-1));
		}
		line(LINE_BOTTOM);
      
      } else if (s.find("/s") == 0) {
        if (s == "/s") whosonline();
	else if (parseString(s,&n,&msg, PARSE_STRING) == PARSE_STRING) {
		if (isCosysop())
			if (toLower(msg) == "-h") whosonline(2);
			else npr(SYSS+"Usage: |11/s |15[|09-h|15]"+SYSF);
		else 
			npr(SYSS+"Usage: |11/s"+SYSF);
	} else {
		if (isCosysop()) npr(SYSS+"Usage: |07/s |15.|09-h|15."+SYSF);
		else npr(SYSS+"Usage: |11/s"+SYSF);
	}
      } else {
         npr(SYSS+"Invalid command, type |14/?|07 for a list of commands"+SYSF);
      }

}	
