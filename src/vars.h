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

#ifndef VARS_H
#define VARS_H

#define DEBUG

#include <string.h>
#include <unistd.h> /* close, uname */
#include <cctype>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <vector>

using namespace std;

#include "RegularExpression.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "SocketStream.h"
#include "PriorityQueue.h"
#include "QFileDatabase.h"

#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890!@#$%^&*()_+-=~`/\\|{}[]:\";\'<>,.? "

#define LINK_M2			0
#define LINK_TC			1
#define LINK_STS		2
#define LINK_DDIAL		3

#define FLAG_LOGOUT           	1
#define FLAG_DISCONNECT       	2
#define FLAG_KILLED           	3
#define FLAG_LOGIN           	4
#define FLAG_CONNECT         	5
#define FLAG_RELOG		6
#define FLAG_LEFTCHAT		7
#define FLAG_LOGINBBS		8

#define LINE_TOP		0
#define LINE_BOTTOM		1

#define FLAG_INPUT_NORMAL     	0
#define FLAG_INPUT_MASKED     	1
#define FLAG_INPUT_FX		2

#define FLAG_UNAVAILABLE	0
#define FLAG_AVAILABLE	      	1
#define FLAG_TYPING		2

#define PARSE_ERROR		0
#define PARSE_NODE		1
#define PARSE_STRING		2
#define PARSE_NODESTRING	3


#define ALIGN_LEFT		1
#define ALIGN_RIGHT		2
#define ALIGN_CENTER		3

#define LOC_CHATROOM          	1
#define LOC_BBS               	2
#define LOC_LOGIN             	3
#define LOC_GAMES             	4
#define LOC_SECURITY_EDITOR     5
#define LOC_USER_EDITOR         6
#define LOC_LINK_EDITOR		7
#define LOC_MESSAGE_EDITOR	8
#define LOC_USER_CONFIG		9
#define LOC_SYSTEM_EDITOR	10
#define LOC_BAN_EDITOR		11
#define LOC_MAIL		12

#define LOC_OTHER		30

const int PRIORITY_CHAT		= 0;
const int PRIORITY_SYSTEM	= 1;
const int PRIORITY_PRIVATE	= 2;
const int PRIORITY_PAGE		= 3;
const int PRIORITY_FORCE	= 4;

const string SYSS = "|04-|12=|15[|07 ";
const string SYSF = " |15]|12=|04-|07\r\n";
const string AWAYCONST = "|12[|15Away|12]|07\r";

const int NODEBIT_TYPING	=  1;

const int USERBIT_SYSLOG	=  1;
const int USERBIT_HOTKEYS	=  2;
const int USERBIT_LOGINCHAT	=  3;
const int USERBIT_OWNER		=  4;
const int USERBIT_SHOWUID	=  5;
const int USERBIT_TIMESTAMP	=  6;
const int USERBIT_TIMESTAMPTYPE =  7;
const int USERBIT_DOUBLESPACING =  8;
const int USERBIT_AWAYFLAG 	=  9;
const int USERBIT_COLONEMOTE    = 10;

const int MSG_CHAT		=  0;
const int MSG_FORCE 		=  1;
const int MSG_SYSTEM 		=  2;
const int MSG_LOGIN 		=  3;
const int MSG_LOGOUT 		=  4;
const int MSG_DISCONNECT	=  5;
const int MSG_KILLED 		=  6;
const int MSG_LEFT   		=  7;
const int MSG_RETURNED 		=  8;
const int MSG_PRIVATE		=  9;
const int MSG_AWAY		= 10;
const int MSG_NOTAWAY		= 11;
const int MSG_ACTION 		= 12;
const int MSG_ACTION2		= 13;
const int MSG_CHANNEL		= 14;
const int MSG_SYSLOG		= 15;
const int MSG_SETATTR		= 16;
const int MSG_DIE		= 17;
const int MSG_RECEIVERELOG	= 18;
const int MSG_RELOG		= 19;
const int MSG_KICKED		= 20;
const int MSG_END		= 50;

const int M2MODE_CHAT		=  MSG_CHAT;
const int M2MODE_FORCE		=  MSG_FORCE;
const int M2MODE_SYSTEM		=  MSG_SYSTEM;
const int M2MODE_LEFT		=  MSG_LEFT;
const int M2MODE_RETURNED 	=  MSG_RETURNED;
const int M2MODE_PRIVATE 	=  MSG_PRIVATE;
const int M2MODE_ACTION		=  MSG_ACTION;
const int M2MODE_ACTION2 	=  MSG_ACTION2;
const int M2MODE_CHANNEL 	=  MSG_CHANNEL;
const int M2MODE_SYSLOG		=  MSG_SYSLOG;
const int M2MODE_AWAY		=  MSG_AWAY;
const int M2MODE_NOTAWAY	=  MSG_NOTAWAY;
const int M2MODE_SETATTR	=  MSG_SETATTR;
const int M2MODE_LOGIN		=  MSG_END+1;
const int M2MODE_LOGOUT		=  MSG_END+2;
const int M2MODE_DISCONNECT 	=  MSG_END+3;
const int M2MODE_KILLED		=  MSG_END+4;
const int M2MODE_PING		=  MSG_END+5;
const int M2MODE_VERINFO	=  MSG_END+6;
const int M2MODE_WHOSONLINE	=  MSG_END+7;
const int M2MODE_INFO		=  MSG_END+8;
const int M2MODE_RELOG		=  MSG_END+9;

/*
const int ACCESS_ADMIN = 0;
const int ACCESS_SYSOP = 1;
const int ACCESS_COSYSOP = 2;
const int ACCESS_MODERATOR = 3;
const int ACCESS_LINK = 4;
const int ACCESS_USER = 5;
const int ACCESS_GUEST = 6;
*/

const int TCMODE_PING = 37;
const int TCMODE_VERINFO = 96;
const int TCMODE_CHAT = 18;
const int TCMODE_WHOSONLINE1 = 86;
const int TCMODE_WHOSONLINE = 85;
const int TCMODE_LOGIN = 17;
const int TCMODE_LOGOUT = 99;
const int TCMODE_ACTION = 33;
const int TCMODE_PRIVATE = 34;
const int TCMODE_HANDLECHANGE = 31;

const int TE_QUOTE  = 0;
const int TE_READIN = 1;
const int TE_CREATE = 2;
const string TOPLINE = "|08[|07...|15-|07....|151|07....|15-|07....|152|07....|15-|07....|153|07....|15-|07....|154|07....|15-|07....|155|07....|15-|07....|156|07....|15-|07....|157|07....|15-|07...|08]\r\n"; 

struct argument {
	int nodenum;
	int socketid;
	char ip[20];
	char hostname[61];
};


#include "BST.h"
#include "typedef.h"
#include "Vector.h"
#include "Thread.h"
//#include "VectorThread.h"
#include "fcns.h"
#include "Directory.h"
#include "Connection.h"

extern QUserRec guest;
extern QSysInfoRec sysinfo;
extern Vector<Connection *> connection;
extern Vector<QChannel> ChannelList;
extern BST<QUserRec> UserList;
extern pthread_mutex_t MUTEX_CONNECTION;
extern pthread_mutex_t MUTEX_ACCESSUSERLIST;
extern QFileDatabase LinkFile;
extern QFileDatabase UserFile;
extern QFileDatabase SecurityFile;
extern QFileDatabase SysinfoFile;
extern QFileDatabase ChannelFile;
extern QStringList BanList;
typedef vector<int> vi;
#include "ClientLink.h"
extern Vector<ClientLinkThread *> AllClientThreads;
#endif

