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

int GetAvailableUserNumber(void) {
	int avail=0, i=0;
	vector<QUserRec> vecUserList;
	UserList.PreOrder(ConvertToVector, (void *)&vecUserList);
cont:
	i=0;
	while (i < vecUserList.size()) {
		if (vecUserList[i].number==avail) {
			avail++;
			i=-1;
		}
		i++;
	}
	i=0;
	while (i < connection.size()) {
		if (connection[i]->getNodenum() == avail) {
			avail++;
			goto cont;
		}
		i++;
	}
	return avail;
}

int activeUsers(void) {
int i=0,tot=0;
QNodeDataRec nd;
	while (i < connection.size()) {
		nd=connection[i]->getNodeData();
		if (timeDifference(nd.Idle, now()) < 1200) tot++;
		i++;
	}
	return tot;
}

void setbit(int bit, int &num) {
int temp=1;
	temp <<= bit;
	num |= temp;
}

void clearbit(int bit, int &num) {
int temp=1;
	temp <<= bit;
	num &= ~temp;
}

bool testbit(int bit, int &num) {
int temp=1;
	temp <<= bit;
	if (num & temp) return true;
	else return false;
	
}

string toUpper(string s) {
	for (unsigned int i=0; i < s.length(); i++) {
		s[i]=toupper(s[i]);
	}
	return s;
}

string toLower(string s) {
	for (unsigned int i=0; i < s.length(); i++) {
		s[i]=tolower(s[i]);
	}
	return s;
}

void debug(string s) {
#ifdef DEBUG
	cerr << s << endl;
#endif

	debuglog(getDateTime(now())+s+"\r\n");

}
bool CompareSecurities(int s1, int s2) {
	QSecurityRec sec1,sec2;
	SecurityFile.Read(&sec1,s1);
	SecurityFile.Read(&sec2,s2);

	if (sec2.admin) {
		return true;	
	}
	if (sec1.sysop) {
		if (sec2.admin) return true;
		else return false;
	}
	return true;
}

int Connection::isModerator() {
  if ( myNodeData.TempModerator ) return (1);
  else if ( getSec()->moderator ) return(1);
  else if ( getSec()->cosysop ) return(1);
  else if ( getSec()->sysop ) return(1);
  else if ( getSec()->admin ) return(1);
  else return(0);
}

int Connection::isCosysop() {
  if ( getSec()->cosysop ) return(1);
  else if ( getSec()->sysop ) return(1);
  else if ( getSec()->admin ) return(1);
  else return(0);
}

int Connection::isSysop() {
  if ( getSec()->sysop ) return(1);
  else if ( getSec()->admin ) return(1);
  else return(0);
}

int Connection::isOwner() {
  if ( testbit(USERBIT_OWNER,getUser()->flags) )return(1);
  else return(0);
}

int Connection::isAdmin() {
  if ( getSec()->admin ) return(1);
  else return(0);
}

void Connection::cap(char *s) {

	npr(Center(SYSS+s+SYSF));
	npr(Center("|04-=-=|12-=-|15=-=-=|12-=-|04=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=|12-=-|15=-=-=|12-=-|04=-=-")+"\r\n\n");
}

int ChannelCount(int channel) {
	int c=0;
	QNodeDataRec nd;
	for (int i=0; i<connection.size(); i++) {
		nd=connection[i]->getNodeData();
		if (nd.User.channel==channel) c++;
	}
	return c;
}

bool UnInvite(int fromNode, int chan, int toNode) {
	int from=GetFirstConnectionByNode(fromNode);
	vector<int> con;
	int to=GetConnectionByNode(toNode,con);
	

	QNodeDataRec nd;
	if ((from == -1) || (to == -1)) return false;
	nd=connection[from]->getNodeData();
	QInstantMessageRec msg;
	msg.message="|10" + string(nd.User.chathandle) + "|07 has uninvited you from channel |03C|11" + IntToStr(chan);
	for (int i=0; i<con.size(); i++) {
		to=con[i];
		connection[to]->msg.push_back(msg);
	}
	ChannelList[chan].removeInvite(toNode);
}

bool Invite(int fromNode, int chan, int toNode) {
	vector<int> con;
	int from=GetFirstConnectionByNode(fromNode);
	int to=GetConnectionByNode(toNode,con);

	QNodeDataRec nd;
	if ((from == -1) || (to == -1)) return false;
	nd=connection[from]->getNodeData();
	QInstantMessageRec msg;
	msg.message="|10" + string(nd.User.chathandle) + "|07 has invited you to channel |03C|11" + IntToStr(chan);
	for (int i=0; i<con.size(); i++) {
		to=con[i];
		connection[to]->msg.push_back(msg);
	}
	ChannelList[chan].setInvite(toNode);
}

int available(int nodenum) {
	QNodeDataRec nd;
	if (GetDataByNode(nodenum, &nd)) {
  		if (testbit(NODEBIT_TYPING, nd.Flags)) return(FLAG_TYPING);
  		if (nd.Location==LOC_CHATROOM) {
    			return(FLAG_AVAILABLE);
 		}
  		return(FLAG_UNAVAILABLE);
	} else {
		debug("available returned a node that didn't exist, killing thread.\r\n");
	}
	return 1000;
}


bool GetDataByNode(int n, QNodeDataRec *nd) {
int c, i;

	c=GetFirstConnectionByNode(n);
	if (c <= -1) {
		return false;
	} else if (c >= 0) {
		if ((connection[c]->getSec()->network==1) && (n != connection[c]->getNodenum())) {
			for (i=0; i<connection[c]->Network.size(); i++) {
				if (connection[c]->Network[i]->Nodenum==n) {
					*nd = *connection[c]->Network[i];
					return true;
				}
			}
		} else {
			*nd = connection[c]->getNodeData();
			return true;
		}
	}
	return false;
}

int GetFirstConnectionByNode(int n) {
int i=0,j;
vector<int> con;
QNodeDataRec nd;
	i=connection.size()-1;
	while (i >= 0) {
		nd=connection[i]->getNodeData();
		if (connection[i]->Network.size() > 0) {
			for (j=0; j<connection[i]->Network.size(); j++) {
				if (n==connection[i]->Network[j]->Nodenum) return(i);
			}
		}
		if (nd.Nodenum == n) return i;
		i--;
	}
	return -1;
}

int GetConnectionByNode(int n, vector<int> &con) {
int i=0,j;
QNodeDataRec nd;

	con.clear();
debug("GCBN: While loop");
	while (i < connection.size()) {
		nd=connection[i]->getNodeData();
		if (connection[i]->Network.size() > 0) {
			for (j=0; j<connection[i]->Network.size(); j++) {
				if (n==connection[i]->Network[j]->Nodenum) con.push_back(i);
			}
		}
		if (nd.Nodenum == n) con.push_back(i);
		i++;
	}
debug("GCBN: Leaving");
	if (con.size() == 0) return -1;
	else return i;
}

string Now(void) {
    char s[21];
    struct tm *l_time;
    time_t now;
					 
    time(&now);
    l_time = localtime(&now);
    strftime(s, 20, "%l:%M%p", l_time);
    s[6]=0;
    s[5]=tolower(s[5]);
    if (s[0]==' ')
      return string(s+1);
    else 
      return string(s);
}


string getTotalTimeShort(double w2) {
int h, min, mon, d, y, s;
string Result;
char str[10];
unsigned long w=(unsigned long)w2;
  
y = (int) (w / 31536000);
  w = w % 31536000;
  mon = (int) (w / 2592000);
  w = w % 2592000;
  d = (int)(w / 86400);
  w = w % 86400;
  h = (int)(w / 3600);
  w = w % 3600;
  min = (int)(w / 60);
  w = w % 60;
  s = (int)w; 

  if (y > 0) {
    sprintf(str,"%2d",y);
    Result = string(str) + ":";
    sprintf(str,"%02d",mon);
    Result += string(str) + "mo";
    return Result;
  } else if (mon > 0) {
    sprintf(str,"%2d",mon);
    Result = string(str) + ":";
    sprintf(str,"%02d",d);
    Result += string(str) + "d";
    return Result;
  } else if (d > 0) {
    sprintf(str,"%2d",d);
    Result = string(str) + ":";
    sprintf(str,"%02d",h);
    Result += string(str) + "h";
    return Result;
  } else if (h > 0) {
    sprintf(str,"%2d",h);
    Result = string(str) + ":";
    sprintf(str,"%02d",min);
    Result += string(str) + "m";
    return Result;
  } else {
      sprintf(str,"%2d",min);
      Result += string(str) + ":";
      sprintf(str,"%02d",s);
      Result += string(str) + "s";
     if (Result.length() != 0)  return Result;
     else return "0s";
  }


}

string getTotalTime(double w2) {
int h, min, mon, d, y, s;
string Result;
char str[10];
unsigned long w=(unsigned long)w2;
  y = (int) (w / 31536000);
  w = w % 31536000;
  mon = (int) (w / 2592000);
  w = w % 2592000;
  d = (int)(w / 86400);
  w = w % 86400;
  h = (int)(w / 3600);
  w = w % 3600;
  min = (int)(w / 60);
  w = w % 60;
  s = (int)w; 

  if (y > 0) {
    sprintf(str,"%d",y);
    Result = string(str) + "y ";
    sprintf(str,"%d",mon);
    Result += string(str) + "m ";
    sprintf(str,"%d",d);
    Result += string(str) + "d";
    return Result;
  } else if (mon > 0) {
    sprintf(str,"%d",mon);
    Result = string(str) + "m ";
    sprintf(str,"%d",d);
    Result += string(str) + "d ";
    sprintf(str,"%d",h);
    Result += string(str) + "h";
    return Result;
  } else if (d > 0) {
    sprintf(str,"%d",d);
    Result = string(str) + "d ";
    sprintf(str,"%d",h);
    Result += string(str) + "h ";
    sprintf(str,"%d",min);
    Result += string(str) + "m";
    return Result;
  } else {
    if (h > 0) {
      sprintf(str,"%d",h);
      Result = string(str) + "h ";
    }
    if (min > 0) {
      sprintf(str,"%d",min);
      Result += string(str) + "m ";
    }
    if (s > 0) {
      sprintf(str,"%d",s);
      Result += string(str) + "s";
    }
     if (Result.length() != 0)  return Result;
     else return "0s";
  }


}

string getDate(time_t w) {
char str[10];
int d=0,mon=0,y=0;
struct tm *when;
string Result;
when = localtime(&w);
  strftime(str,10, "%j", when);
  d=atoi(str);
  strftime(str,10, "%m", when);
  mon=atoi(str);
  strftime(str,10, "%y", when);
  y=atoi(str);
  
  Result = IntToStr(mon,2) + "/" + IntToStr(d) + "/" + IntToStr(y);
  return Result;
}

string getTotalTime(time_t w) {
char str[10];
int h=0,min=0,s=0,d=0,mon=0,y=0;
struct tm *when;
when = localtime(&w);
string Result;
  strftime(str,10, "%j", when);
  d=atoi(str);
  strftime(str,10, "%m", when);
  mon=atoi(str);
  strftime(str,10, "%y", when);
  y=atoi(str);
  strftime(str,10, "%M", when);
  min=atoi(str);
  strftime(str,10, "%H", when);
  h=atoi(str);
  strftime(str,10, "%S", when);
  s=atoi(str);
  
  if (y > 0) {
    sprintf(str,"%d",y);
    Result = string(str) + "y ";
    sprintf(str,"%d",mon);
    Result += string(str) + "m ";
    sprintf(str,"%d",d);
    Result += string(str) + "d";
    return Result;
  } else if (mon > 0) {
    sprintf(str,"%d",mon);
    Result = string(str) + "m ";
    sprintf(str,"%d",d);
    Result += string(str) + "d ";
    sprintf(str,"%d",h);
    Result += string(str) + "h";
    return Result;
  } else if (d > 0) {
    sprintf(str,"%d",d);
    Result = string(str) + "d ";
    sprintf(str,"%d",h);
    Result += string(str) + "h ";
    sprintf(str,"%d",min);
    Result += string(str) + "m";
    return Result;
  } else {
    if (h > 0) {
      sprintf(str,"%d",h);
      Result = string(str) + "h ";
    }
    if (min > 0) {
      sprintf(str,"%d",min);
      Result += string(str) + "m ";
    }
    if (s > 0) {
      sprintf(str,"%d",s);
      Result += string(str) + "s";
    }
     if (Result.length() != 0)  return Result;
     else return "0s";
  }
}

double timeDifference(time_t s, time_t f) {
 return difftime(f,s);
}

string getShortTime(time_t when) {
  char s[10];
  struct tm *l_time;
  string f;

  l_time = localtime(&when);
  if (timeDifference(when, now()) > 518400) {
    strftime(s,10, "%e%b%y", l_time);
    //27Nov01
    f=string(s);
    return f;
  } else if (timeDifference(when, now()) > 82800) {
    strftime(s,10, "%a%l%P", l_time);
    
    f=string(s);
    //Sun10p
    //012345
    f.erase(6,1);
    f[5]=tolower(f[5]);
    f[1]=tolower(f[1]);
    f[2]=tolower(f[2]);
    f[0]=toupper(f[0]);
    return f;
  } else {
    strftime(s, 10, "%l:%M%p", l_time);
    s[6]=0;
    s[5]=tolower(s[5]);
    f=string(s);
    if (f[0]==' ') f.erase(0,1);
    return f;
  }
}

string getTime(time_t when) {
  char s[10];
  struct tm *l_time;
  string f;
  l_time = localtime(&when);
  strftime(s, 10, "%l:%M%p", l_time);
  s[6]=0;
  s[5]=tolower(s[5]);
  f=string(s);
  if (f[0]==' ') f.erase(0,1);

  return f;

}

string getDateTime(time_t when) {
   char s[25];
   struct tm *l_time;
   string f;
   l_time = localtime(&when);
   strftime(s, 25, "%a %b%e, %Y %l:%M%p", l_time);
//Wed Oct 1, 2001  x:xxp
   s[22]=0;
   s[21]=tolower(s[21]);
   f = string(s);
   return f;

}

string getFullDateTime(time_t when) {
   char s[50];
   struct tm *l_time;
   string f;
   l_time = localtime(&when);
   strftime(s, 49, "%A %B %e, %Y - %l:%M%p", l_time);
//Wednesday Oct 1, 2001 -  x:xxp
   f = string(s);
   return f;

}


void Connection::writeLocation(int location) {
  setLocation(location);
}

string spacecolors(string s, unsigned int i, int mode) {
  string s2, result;
  result = s;
  s2=stripcolors(s);

  while (s2.length() < i) {
     if (mode==ALIGN_LEFT) {
       result += " ";
       s2 +=" ";
     } else if (mode == ALIGN_RIGHT) {
       result = " " + result;
       s2 = " " + s2;
     }
  }
  return result;
}

bool isValidColor(int i) {
	if (i==0) return false;
	if (i <= 15) return true;
	return false;
}

string stripcolors(string s) {
	string Result=s;
	int i=0;
	while (i < Result.length()) {
		if (Result[i]=='|') {
			if ((i+1 < Result.length()) && (Result[i+1]=='|'))
				Result.erase(i,1);
			else if ((i+2 < Result.length()) && (isdigit(Result[i+1])) && (isdigit(Result[i+2]))) {
				if (!isValidColor( ((Result[i+1]-'0')*10)+(Result[i+2]-'0')) )
					i+=2;
				else { 
					Result.erase(i,3);
					i--;
				}
			}
		}
		i++;
	}
	return Result;

}

bool trailingpipe(string s) {
int i=s.size()-1;
int numpipes=0;

	while (i >= 0 ) {
		if (s[i]=='|') numpipes++;
		else break;
		i--;
	}
	if (numpipes % 2 == 1) return true;
	return false;
}

void saveOnlineUsers(void) {
  for (int i=0; i < connection.size(); i++) {
    if ((!connection[i]->getGuest()) && (connection[i]->getLoggedin()))
	  save_user(connection[i]->getUser());
  }
}

time_t now(void) {
  time_t n;
  time(&n);
  return n;
}

string GenerateTemporaryFilename(string dir, int len) {
	string s;
	int i=0;
	do {
		s="";
		while (s.length()<len) {
			char myRand=char(int('a')+randomnumber(25));
			s+=myRand;
		}

	} while (exist(dir + "/"+s));	
	return s;
}

bool ValidFN(string s) {
	if (s.find("/") != string::npos) return false;
	if (s.find("|") != string::npos) return false;
	if (s.find(".") != string::npos) return false;
	if (s.find("?") != string::npos) return false;
	if (s.find("*") != string::npos) return false;
	if (s.find("&") != string::npos) return false;
	if (s.find("!") != string::npos) return false;
	if (s.find("@") != string::npos) return false;
	if (s.find("#") != string::npos) return false;
	if (s.find("$") != string::npos) return false;
	if (s.find("%") != string::npos) return false;
	if (s.find("^") != string::npos) return false;
	if (s.find("(") != string::npos) return false;
	if (s.find(")") != string::npos) return false;
	if (s.find("{") != string::npos) return false;
	if (s.find("}") != string::npos) return false;
	if (s.find("<") != string::npos) return false;
	if (s.find(">") != string::npos) return false;
	if (s.find("+") != string::npos) return false;
	if (s.find("=") != string::npos) return false;
	if (s.find("-") != string::npos) return false;
	if (s.find("~") != string::npos) return false;
	if (s.find("`") != string::npos) return false;
	if (s.find("\'") != string::npos) return false;
	if (s.find(":") != string::npos) return false;
	if (s.find(";") != string::npos) return false;
	if (s.find("\\") != string::npos) return false;
	if (s.find(" ") != string::npos) return false;
	return true;
}
