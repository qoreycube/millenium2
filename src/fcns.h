#ifndef _FCNS_H_
#define _FCNS_H_

/* File: Socket.c */


/* File: SocketStream.c */

SocketStream &operator<<(SocketStream &out, string s);
SocketStream &operator<<(SocketStream &out, char ch);
SocketStream &operator<<(SocketStream &out, int i);

/* File: ServerSocket.c */


/* File: ClientSocket.c */


/* File: main.c */

void LockMutex(pthread_mutex_t *mtx);
void UnlockMutex(pthread_mutex_t *mtx);
void InitializeGlobalMutex(void);
void LoadUserList(void);
void checkExistFiles();
void setupGuest(void);
void CheckChannels();
void LoadBanList(void);
int main(int argc, char *argv[]);
void CheckAlive(void);
void PackConnectionVector(void );
string FlagName(int flag);
void broken_pipe(int hm);
void SaveOnlineUsers(void);
void destroy(int hm);
int GetAvailableNode(void);
bool FifteenMinuteInterval(time_t n);
void CheckTimeStamps(void);
void CheckKeepAlive(void);
void RoundToThirty(int & secs);

/* File: utl.c */

void SaveUsersToFile(QUserRec *item, int &currNumber, void *obj);
bool operator<(const QUserRec &leftHand, QUserRec &rightHand);
bool operator>(const QUserRec &leftHand, QUserRec &rightHand);
bool operator==(const QUserRec &leftHand, QUserRec &rightHand);
int GetLargestOnlineNode(void);
int UserIsOnline(int usernum);
string GetLocation(int loc);
void securityname(int nodenum, char *s);
void ForceMessage(string s);
void RelogNode(int n);
void KillNode(int n);
int randomnumber(int n);
void ReverseSelectionSort(vector<QSort> *myVec);
void SelectionSort(vector<QSort> *myVec, bool which=true);
void ParseTCNetwork(string s, int &mode, int &minormode, string &parameter, string &text);
string FormatTCMessage(int mode, int minormode, string param, string text);
string FormatM2Message(int mode, int nodeTo, int nodeFrom, int channelTo, int channelFrom, char lc, char rc, string parameter, string text);
bool ParseM2Network(string s, int &mode, string &strMode, int &nodeTo, int &nodeFrom, int &channelTo, int &channelFrom, char &lc, char &rc, string &parameter, string &text);
string StringLinkType(int type);
int parseString(string olds, int *n, string *msg, int mode);
void ConvertToModifiedVector(QUserRec *usr, int &usernum, void *ptr);
void ConvertToVector(QUserRec *usr, int &usernum, void *ptr);

/* File: io.c */

void showmessage(int n, int flag);
bool IsIn(int i, vector<int> vec);
void ShowMessageAllBut(int n, int flag, int noShow);
int GetSecurityByName(string s, QSecurityRec *sec);
void SaveUserListToFile(void);
int SaveToFile(void *item, size_t size, int which, string filename);
int LoadFromFile(void *item, size_t size, int which, string filename);
void getconfigfile(void);
int lineinput(char *s, FILE *in);
int load_user(QUserRec *p, string loginname);
void save_user2(QUserRec *p, int which);
void save_user(QUserRec *p);
int totalusers(void);
int TotalLinks(void);
int totalSecurityRecords(void);
int exist(string filename);
string localinput(int maxlen);

/* File: misccmd.c */

int GetAvailableUserNumber(void);
int activeUsers(void);
void setbit(int bit, int &num);
void clearbit(int bit, int &num);
bool testbit(int bit, int &num);
string toUpper(string s);
string toLower(string s);
void debug(string s);
bool CompareSecurities(int s1, int s2);
int ChannelCount(int channel);
bool UnInvite(int fromNode, int chan, int toNode);
bool Invite(int fromNode, int chan, int toNode);
int available(int nodenum);
bool GetDataByNode(int n, QNodeDataRec *nd);
int GetFirstConnectionByNode(int n);
int GetConnectionByNode(int n, vector<int> &con);
string Now(void);
string getTotalTimeShort(double w2);
string getTotalTime(double w2);
string getDate(time_t w);
string getTotalTime(time_t w);
double timeDifference(time_t s, time_t f);
string getShortTime(time_t when);
string getTime(time_t when);
string getDateTime(time_t when);
string getFullDateTime(time_t when);
string spacecolors(string s, unsigned int i, int mode);
bool isValidColor(int i);
string stripcolors(string s);
bool trailingpipe(string s);
void saveOnlineUsers(void);
time_t now(void);
string GenerateTemporaryFilename(string dir, int len);
bool ValidFN(string s);

/* File: message.c */

void SystemMessageAllBut(int i, string s);
void SystemMessage(string s);
void Broadcast(QInstantMessageRec &mr);
void Broadcastn(QInstantMessageRec &mr);
void BroadcastAllButLinks(int n, QInstantMessageRec &temp);
void BroadcastAllBut(int n, QInstantMessageRec &temp);
void BroadcastTimeStamp(void);
void BroadcastAllButn(int n, QInstantMessageRec &temp, bool links);
string IntToStr(int i, int format=0);
string MsgTypeString(int type);
string FormatClientMessage(QInstantMessageRec *Msg);

/* File: commands.c */

bool HandleAllowed(string s);

/* File: menus.c */

int GetUserByNumber(int num, QUserRec &us);
bool ParseCommand(string s, QParseRec &parse);
int StrToInt(string s);

/* File: log.c */

void log(string s);
void syslog(string s);
void debuglog(string s);
void errorlog(string s);
void appendfile(string fn, string s);
void connectionlog(string s, int flag);

/* File: mail.c */


#endif
