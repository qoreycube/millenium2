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

#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

void clearbit(int, int &);
void setbit(int, int &);
bool testbit(int, int &);

struct QMailRec {
	char unusedchar1[30];
	char unusedchar2[30];
	char fn[30];
	char subject[60];
	int from;
	time_t sent;
	int replied;
	int urgent;
	int unusedint1;
	int unusedint2;
	int unusedint3;
	int unusedint4;;
};

struct QInstantMessageRec {
    string chathandle;
    string message;
    
    char   lc,
    	   rc;

    int	   nodeFrom;
    int	   nodeTo;
    char   channelFrom;
    char   channelTo;
    char   msgType;
    char   priority;
	QInstantMessageRec() {
		lc=0;
		rc=0;
		nodeFrom=-1;
		nodeTo=-1;
		message="";
		chathandle="";
		channelTo=-1;
		channelFrom=-1;
		priority=PRIORITY_SYSTEM;
		msgType=MSG_SYSTEM;
	}
};

struct QSort {
	int index;
	int sortedItem;
};

struct QChannelInfo {

	char	name[41];
	char	topic[41];
  
  unsigned char locked;
  
  	char	unusedchar1[31];
  	char	unusedchar2[31];
  	char	unusedchar3[31];
  	char	unusedchar4[31];
  
  	bool	isPublic;

	int	invite;
  	int	invite2;
  	int	unusedint4;

};

class QChannel {
	private:
		QChannelInfo myChannel;
		QFileDatabase *myFile;
		int myRec;
	public:
		QChannel(QFileDatabase *FileObj, int recnum) {
			myRec=recnum;
			myFile=FileObj;
		}
		
		void setName(string newName) { 
			while (newName.length() > 14)
				newName.erase(newName.length()-1,1);
			
			strcpy(myChannel.name, newName.c_str()); 
		}
		
		
		void clearInvites(void) {
			myChannel.invite=0;
			myChannel.invite2=0;
		}
		
		void removeInvite(int node) {
			if (node < 32) {
				clearbit(node, myChannel.invite);
			} else {
				clearbit(node-32, myChannel.invite2);
			}
		}

		void setInvite(int node) {
			if (node < 32) {
				setbit(node, myChannel.invite);
			} else {
				setbit(node-32, myChannel.invite2);
			}
		}
		
		bool getInvite(int node) {
			if (node < 32) {
				return testbit(node, myChannel.invite);
			} else {
				return testbit(node-32, myChannel.invite2);
			}
		}
		
		void setTopic(string newTopic) { 
			while (newTopic.length() > 38)
				newTopic.erase(newTopic.length()-1,1);
			
			strcpy(myChannel.topic, newTopic.c_str()); 
		}
		
		void setPublic(bool pub) { myChannel.isPublic = (pub)?1:0; }
		
		void setLocked(int locked) { myChannel.locked=locked; }
		
		string getTopic() { return string(myChannel.topic); }
		
		string getName() { return string(myChannel.name); }
		
		int getLocked() { return myChannel.locked; }
		
		bool getPublic() { return (bool)myChannel.isPublic; }

		void Save() { myFile->Write(&myChannel, myRec); }

		void Load() { myFile->Read(&myChannel, myRec); }
};

struct QUserRec {
  long           points,           // we may use this, you never know
                 totallogins;      // obvious
		 
  unsigned char  security;         // security level 0-255      

  char		 doing[61],	 
  		 loginname[41],    // 
                 chathandle[61],   // chat handle (61 incasecoloroverkill)
                 unusedother1[21], // recently unused.
		 password[41],     // hmm.. lemme think.
		 unusedchar1,	  // future upgrades.
  		 unusedchar2,
		 unusedother2[99],
		 unusedother3[99],
		 unusedother4[99];
		 
  int		 loginerrors,	   // # of login errors (invalid passwords) 
                                   // since last successful login
                 channel;	   // channel number they were last in

  bool		 ansi,		   // ansi color graphics?
  		 sound,		   // sound?
		 echo;
		 
  time_t         lastlogin,
  		 firstlogin,
		 created,
		 unusedtime2,
		 unusedtime3,
		 unusedtime4;

  long int	 kills,
  		 killed,
  		 totaltime,
  		 unusedlongint1,
		 unusedlongint2,
		 unusedlongint3,
		 unusedlongint4;  // for future upgrades

  int		 monitor,
		 flags,
		 number,
		 unusedint4,
		 unusedint5;
};
	

struct QSecurityRec {
  char		spacer1;
  char          name[21];
  unsigned char security;
  char          lc, rc;
  char 		cosysop,
		sysop,
		admin,
		moderator,
		network,
		unused2,
		unused3,
		spacer2; 
};

struct QLinkRec {
	char spacer1;
	char host[60];
	char loginname[31];
	char password[31];
	char comment[61];
	
	char handle[31];
	char name[21];

	char charunused1[9];
	char charunused2[61];
	char charunused3[61];
	int  type;
	int  port;
	int  intunused1;
	int  intunused2;
	int  intunused3;
	char spacer2;
};

struct QNodeDataRec {
    bool	  TempModerator;
    bool          Guest;
    bool          Away;
    bool          Loggedin;     // have they logged in yet?
    bool	  IsBeingEdited;
    bool	  Delete;
    
    int           Location;   // where user is, chatroom, bbs, etc
    int           Nodenum;
    int		  RemoteNodenum;
    int		  Usernum;
    int   	  Flags;
    int           StayAlive;
    
    char	  FloodRate;

    time_t        Idle;
    time_t        Timeon;
    time_t        Logintime;
    time_t        Awaytime;
    time_t        Starttime;
    time_t	  StartedEditing;
    time_t        LastCommand;
    time_t	  TotalTime;

    string        Awaymsg;
    string        Ip;
    string	  Prompt;
    string	  Hostname;
    QSecurityRec   Sec;                // security level of the current node.
    QUserRec       User;               // TUserRecord for this node

    QNodeDataRec() {
	Delete=false;
    }
};

struct QSysInfoRec {
    int port;
    unsigned long totallogins;
    time_t uptime;
    time_t firststarted;
    unsigned char locked;
    int shutdown;
    int intunsed1;
    int intunsed2;
    int intunsed3;
    int intunsed4;
    int intunsed5;
    char name[31];
    char linkHandle[31];
    char linkHost[41];
    char linkName[31];
    char unusedchar1[61];
    char unusedchar2[61];
    char unusedchar3[61];
    char unusedchar4[61];

};

struct QModifiedUserRec {
	bool modified;
	QUserRec user;
	string loginname;
	bool deleted;

	QModifiedUserRec() {
		modified=false;
		deleted=false;
	}
};


class QStringList {
	private:
		vector<string> myStrings;
	
	public:
		void Add(const string s) { myStrings.push_back(s); }
		void Insert(size_t i, string s) { myStrings.insert(myStrings.begin()+i, s); }
		void Erase(size_t i) {  myStrings.erase(myStrings.begin() + i); }
		void Clear(void) { myStrings.clear(); }
		int SaveToFile(const string fn);
		size_t Size(void) { return myStrings.size(); }
		int LoadFromFile(const string fn);
		string &Strings(size_t i) { return myStrings[i]; }
		string &operator[](size_t i) { return *(myStrings.begin() + i); }
};

struct QParseRec {
	int start, finish;
	string command, text;

	QParseRec() {
		start=-2;
		finish=-2;
	}
};

#endif
