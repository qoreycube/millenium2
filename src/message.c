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

void SystemMessageAllBut(int i, string s) {

	QInstantMessageRec msg;
	msg.message=s;
	msg.msgType=MSG_SYSTEM;
	msg.priority=PRIORITY_SYSTEM;

	BroadcastAllBut(i, msg);

}


void SystemMessage(string s) {
	SystemMessageAllBut(-1, s);
}
void Connection::clearmessages(void) {
  msg.clear();
}


void Broadcast(QInstantMessageRec &mr) {
	Broadcastn(mr);
} 

void Broadcastn(QInstantMessageRec &mr) {
	BroadcastAllBut(-1,mr);
}

void BroadcastAllButLinks(int n, QInstantMessageRec &temp) {
	BroadcastAllButn(n, temp, true);
}

void BroadcastAllBut(int n, QInstantMessageRec &temp) {
	BroadcastAllButn(n, temp, false);
}

void BroadcastTimeStamp(void) {
	string s=getFullDateTime(now());
	
	QInstantMessageRec myMsg;
	myMsg.message=s;
	int i=0;

	while (i < connection.size()) {
		QNodeDataRec rec=connection[i]->getNodeData();
		if (testbit(USERBIT_TIMESTAMP, rec.User.flags)) {
			if (!testbit(USERBIT_TIMESTAMPTYPE, rec.User.flags)) { 
				connection[i]->msg.push_back(myMsg);	
			}
		}
		i++;
	}
}

void BroadcastAllButn(int n, QInstantMessageRec &temp, bool links) {
int i,c;
QNodeDataRec nd;
	vector<int> con;
	if (temp.nodeTo != -1) {
		c=GetConnectionByNode(temp.nodeTo,con);
		for (int j=0; j<con.size(); j++) {
			connection[con[j]]->msg.push_back(temp);
		}
		return;
	}
	i=0;
	c=GetConnectionByNode(n,con);
	while (i < connection.size()) {
		if (!IsIn(i,con)) {
			if ((connection[i]->getSec()->network ==1 ) && (links)) {
				;
			} else {
				//debug("Broadcsting to connection %d, network==%d",i,connection[i]->getSec()->network);
				nd=connection[i]->getNodeData();
				 if (nd.Loggedin) {
					switch(temp.priority) {
       						case PRIORITY_CHAT:
	  						
							if (nd.Location == LOC_CHATROOM) 
       								if ((nd.User.channel==temp.channelTo) || (temp.channelTo== -1) || (testbit(temp.channelTo, nd.User.monitor))) { 
									connection[i]->msg.push_back(temp);
            							}
         					 	break;
						case PRIORITY_SYSTEM:
							if (((temp.channelTo==nd.User.channel) || (temp.channelTo==-1)) && (nd.Location == LOC_CHATROOM))
								connection[i]->msg.push_back(temp);
							break;
						case PRIORITY_FORCE:
							connection[i]->msg.push_back(temp);
							break;
						default:
							debug("-=-=- PRIORITY: " + IntToStr(temp.priority));
							debug("    Message: " +  temp.message);
      					}
    			 	}
			}
    		}
		i++;
  	}
}

string IntToStr(int i, int format) {
char s[10];
char charFormat[10];
	if (format>1) {
		sprintf(charFormat, "%%0%dd",format);
		sprintf(s,charFormat,i);
		return string(s);
	}
	sprintf(s,"%d",i);
	return string(s);
}

// mode$modestring$fromNode$toNode$FromChannel$SpecialChar1SpecialChar2$Parameter$Text
// int $  string  $  int   $ int  $ int       $ char char $string$string
string MsgTypeString(int type) {
	string Result;
	switch(type) {
		case MSG_LOGIN: Result = "LOGIN"; break;
		case MSG_LOGOUT: Result = "LOGOUT"; break;
		case MSG_KILLED: Result = "KILLED"; break;
		case MSG_DISCONNECT: Result = "DISCONNECT"; break;
		case MSG_AWAY: Result = "AWAY"; break;
		case MSG_NOTAWAY: Result = "NOTAWAY"; break;
		case MSG_LEFT: Result = "LEFT"; break;
		case MSG_RETURNED: Result = "RETURNED"; break;
		case MSG_PRIVATE: Result = "PRIVATE"; break;
		case MSG_CHAT: Result = "CHAT"; break;
		case MSG_FORCE: Result = "FORCE?"; break;
		case MSG_ACTION: Result = "ACTION"; break;
		case MSG_ACTION2: Result = "ACTION2"; break;
		case MSG_SYSTEM: Result = "SYSTEM"; break;
		case MSG_CHANNEL: Result = "CHANNEL"; break;
		case MSG_SYSLOG: Result = "SYSLOG"; break;
		case M2MODE_LOGIN: Result = "LOGIN"; break;
		case M2MODE_LOGOUT: Result = "LOGOUT"; break;
		case M2MODE_KILLED: Result = "KILLED"; break;
		case M2MODE_DISCONNECT: Result = "DISCONNECT"; break;
		case M2MODE_WHOSONLINE: Result = "WHOSONLINE"; break;
		case M2MODE_VERINFO: Result = "VERINFO"; break;
		case M2MODE_PING: Result = "PING"; break;
		case M2MODE_SETATTR: Result = "SETATTR"; break;
		case M2MODE_INFO: Result = "INFO"; break;
		case MSG_END: Result = "END"; break;
		default: Result = "UHOH>:o"; break;
	}
	return Result;

}

string FormatClientMessage(QInstantMessageRec *Msg) {
	string Result = "";
	char delim='\x03';
	
	Result = IntToStr(Msg->msgType);
	Result += delim;
	Result += MsgTypeString(Msg->msgType);
	Result += delim;
	if (Msg->nodeFrom != -1)
		Result += IntToStr(Msg->nodeFrom); 
	Result += delim;
	if (Msg->nodeTo != -1)
		Result += IntToStr(Msg->nodeTo); 
	Result += delim;
	if (Msg->channelFrom != -1)
		Result += IntToStr(Msg->channelFrom);
	Result += delim;
	if ((Msg->lc == 0) || (Msg->lc == -1)) 
		Result += " ";
	else 
		Result += Msg->lc;
	if ((Msg->rc == 0) || (Msg->rc == -1)) 
		Result += " ";
	else 
		Result += Msg->rc;
	Result += delim;
	Result += Msg->chathandle;
	Result += delim;
	Result += Msg->message;
	Result += "\r\n";
	return Result;
}

string Connection::ReformatMessage(QInstantMessageRec *Msg) {
string Result="";
char s[72];

	switch (Msg->msgType) {
	case MSG_SETATTR:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_SETATTR, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else Result="UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			return "";
		}
		break;
	case MSG_LOGIN:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_LOGIN, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_LOGIN, Msg->nodeFrom, Msg->chathandle, "");
			else Result="UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result=SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|15 logged in|07 " + Msg->message + "|07";
			Result += SYSF;
			if (getUser()->sound) Result = ((char)7) + Result;
			return Result;	
		}
		break;
	case MSG_LOGOUT:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_LOGOUT, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_LOGOUT, Msg->nodeFrom, Msg->chathandle, "");
			else Result="UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result=SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) +Msg->chathandle + "|10" + Msg->rc + "|12 logged out|07 " + Msg->message + "|07";
			Result+=SYSF;
			return Result;	
		}
		break;
	case MSG_RELOG:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_RELOG, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_LOGOUT, Msg->nodeFrom, Msg->chathandle, "");
			else Result="UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result=SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) +Msg->chathandle + "|10" + Msg->rc + "|14 relogged|07 " + Msg->message + "|07";
			Result+=SYSF;
			return Result;	
		}
		break;
	case MSG_KILLED:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_KILLED, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_LOGOUT, Msg->nodeFrom, Msg->chathandle, "");
			else return "UNKNOWN LINK TYPE";
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result=SYSS;
			sprintf(s,"|07%03d:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) +Msg->chathandle + "|10" + Msg->rc + " |04killed|07 " + Msg->message + "|07";
			Result += SYSF;
			return Result;	
		}
		break;
	
	case MSG_DISCONNECT:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_DISCONNECT, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_LOGOUT, Msg->nodeFrom, Msg->chathandle, "");
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result=SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) +Msg->chathandle + "|10" + Msg->rc + "|07 |04disconnected |07" + Msg->message + "|07";
			Result += SYSF;
			return Result;	
		}
		break;
		
	case MSG_SYSTEM:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_SYSTEM, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = SYSS;
			if (Msg->chathandle != "") Result += "|10" + Msg->chathandle+" ";
			Result += "|07"+ Msg->message + SYSF;
			return Result;	
		}
		break;
	case MSG_CHAT:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_CHAT, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_CHAT, Msg->nodeFrom, Msg->chathandle, Msg->message);
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			if (Msg->nodeFrom != -1) {
				sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
				Result += string(s);
				//debug("Msg->Chathandle = '%s'", Msg->chathandle.c_str());
				Result += Msg->chathandle;
			
				Result += "|10";
				Result += Msg->rc; 
				Result += "|07 ";
			}
			Result += Msg->message;
			Result += "|07\r\n";
			return Result;	
		}
		break;
	case MSG_PRIVATE:
		if (getSec()->network==1) {
			for (int i=0; i< Network.size(); i++) {
				if (Network[i]->Nodenum == Msg->nodeTo) {
					if (myLink.type == LINK_M2)
						Result=FormatM2Message(M2MODE_PRIVATE, Network[i]->RemoteNodenum, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
					else if (myLink.type==LINK_TC) {
						sprintf(s,"%d:",Network[i]->RemoteNodenum);
						Msg->message = string(s)+Msg->message;
						Result=FormatTCMessage(TCMODE_PRIVATE, Msg->nodeFrom, Msg->chathandle, Msg->message);
					} else return "UNKNOWN LINK TYPE";
					return Result;
				}
			}
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			sprintf(s,"|12P|07%03d:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|07 " + Msg->message + "|07\r\n";
			return Result;	
		}
		break;
	case MSG_ACTION:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_ACTION, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_ACTION, Msg->nodeFrom, Msg->chathandle, Msg->message);
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|07 " + Msg->message + "|10" + Msg->rc + "|07\r\n";
			return Result;
		}
		break;
	case MSG_ACTION2:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_ACTION2, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result=FormatTCMessage(TCMODE_ACTION, Msg->nodeFrom, Msg->chathandle, Msg->message);
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			sprintf(s,"|07%03d:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|07" + Msg->message + "|10" + Msg->rc + "|07\r\n";
			return Result;	
		}
		break;
	case MSG_LEFT:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_LEFT, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|07 has left to |15" + Msg->message + SYSF;
			return Result;	
		}
		break;
	case MSG_RETURNED:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_RETURNED, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|07 has returned from |15" + Msg->message + SYSF;
			return Result;	
		}
		break;
	case MSG_AWAY:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_AWAY, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = SYSS;
			sprintf(s,"|07%03d|07:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|07 is currently away, \"" + Msg->message + "|07\"" + SYSF;
			return Result;	
		}
		break;
	case MSG_NOTAWAY:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_NOTAWAY, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = SYSS;
			sprintf(s,"|07%03d:|03C|11%d|10%c|07",Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10"+ Msg->rc + "|07 has returned from being away" +SYSF;
			return Result;	
		}
		break;
	case MSG_CHANNEL:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_CHANNEL, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			sprintf(s,"|03C|11%d|07:%03d:|03C|11%d|10%c|07",Msg->channelTo,Msg->nodeFrom, Msg->channelFrom, Msg->lc);
			Result += string(s) + Msg->chathandle + "|10" + Msg->rc + "|07 "+ Msg->message + "|07\r\n";
			return Result;	
		}
		break;
	case MSG_SYSLOG:
		if (getSec()->network==1) {
			if (myLink.type == LINK_M2)
				Result=FormatM2Message(M2MODE_SYSLOG, Msg->nodeTo, Msg->nodeFrom, Msg->channelTo, Msg->channelFrom, Msg->lc, Msg->rc, Msg->chathandle, Msg->message);
			else if (myLink.type==LINK_TC)
				Result="";
			else return "UNKNOWN LINK TYPE";
			return Result;
		} else if (ChatClient) {
			return(FormatClientMessage(Msg));
		} else {
			Result = "|03-|11=|15[ |07"+Msg->message+" |15]|11=|03-|07\r\n";
			return Result;	
		}
		break;
	}
		syslog("Case Not Handled");
		debug("Case not handled: "+IntToStr(Msg->msgType) + "\r\n");
		debug("Msg->msgType:     " + IntToStr(Msg->msgType) + "\r\n");
		debug("Msg->priority:    " + IntToStr(Msg->priority) + "\r\n");	
		debug("Msg->nodeTo:      " + IntToStr(Msg->nodeTo) + "\r\n");	
		debug("Msg->nodeFrom:    " + IntToStr(Msg->nodeFrom) + "\r\n");	
		debug("Msg->ChannelTo:   " + IntToStr(Msg->channelTo) + "\r\n");	
		debug("Msg->ChannelFrom: " + IntToStr(Msg->channelFrom) + "\r\n");	
		debug("Msg->message:     " + Msg->message + "\r\n");	
		debug("Msg->chathandle   " + string(Msg->chathandle) + "\r\n");	
		return("Case not handled\r\n");
}

void Connection::checkForMessages(void) {
int x;
char killed[10];
sprintf(killed,"%c%c%c",1,2,3);
bool displayed=false;

	if (Finished) return;

	if (!msg.empty()) {              // this node has some messages waiting in their queue

		x=0;
		while ((x < msg.size()) && (!Finished)) {
			if (x < 0) {
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				debug("OOOOHSHIT: x < 0, x=" + IntToStr(x));
				return;
			}
			//if (msg[x].msgType != MSG_SETATTR)
			//debug("MsgType==%d",msg[x].msgType);
			if (msg[x].msgType==MSG_KICKED) {
				getUser()->channel=1;
				msg.erase(x);
				x--;
				return;
			} else if (msg[x].msgType==MSG_DIE) {    
				debug("logout(FLAG_KILLED)");
				msg.erase(x);
				logout(FLAG_KILLED);
				return; 
			} else if (msg[x].msgType==MSG_RECEIVERELOG) {
				debug("logout(FLAG_RELOG)");
				msg.erase(x);
				logout(FLAG_RELOG);
				return;
		        }
		        if (msg[x].priority == PRIORITY_FORCE) {
			    	string res=ReformatMessage(&msg[x]);
				if (res.length() > 0)
			    		npr(res);
				//npr("|07");
				displayed=true;
	                	msg.erase(x);
		        	x--;
				continue;
		        }
		        if (available(getNodenum()) != FLAG_TYPING) {   // if s/he's not in the middle of typing, or otherwise unavailable
				if (available(getNodenum())==FLAG_AVAILABLE) {
					if ((msg[x].msgType==MSG_PRIVATE) && (getAway())) 
						appendfile("./tmp/"+string(getUser()->loginname), "|15" + Now() + " " + ReformatMessage(&msg[x]));
		        		string res=ReformatMessage(&msg[x]);
					if (res.length() > 0) {
						if (testbit(USERBIT_TIMESTAMP, myNodeData.User.flags))
							if (testbit(USERBIT_TIMESTAMPTYPE, myNodeData.User.flags))
								res = "|15[|07"+getShortTime(now())+"|15]|07 " + res; 
				   		if (testbit(USERBIT_DOUBLESPACING, myNodeData.User.flags)) res +="\r\n";
						npr(res);
						if ((myNodeData.Away == true) && (testbit(USERBIT_AWAYFLAG, myNodeData.User.flags))) {
							npr(AWAYCONST);
						}
					}
					//npr("|07");
	             			msg.erase(x);
	              			x--;
		      			displayed=true;
		    		} else if (available(getNodenum()) == FLAG_UNAVAILABLE) {
					if (msg[x].priority == PRIORITY_FORCE || msg[x].priority == PRIORITY_PAGE) {
		          			if ((msg[x].msgType==MSG_PRIVATE) && (getAway())) 
							appendfile("./tmp/"+string(getUser()->loginname), "|15" + Now() + " " + ReformatMessage(&msg[x]));
		        			string res=ReformatMessage(&msg[x]);
						if (res.length() > 0)
			   				npr(res);
					//npr("|07");
			  			displayed=true;
	                  			msg.erase(x);
		          			x--;
       	         			}
       	     			} else {
					debug("available(i) returned "+IntToStr(available(getNodenum())));
				}
				if ((x == -1) && (msg.size() == 0)) break; 
			} 
			x++;
        	}
	}
	if (displayed && (getPrompt() != "")) npr(getPrompt());
} 
