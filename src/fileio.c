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


int GetSecurityByName(string s, securityrec *sec) {
int i=1;
securityrec temp;

	while (load_security(&temp,i)) {
		if (toLower(string(temp.name)) == toLower(s)) {
			load_security(sec,i);
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

void SaveToFile(void *item, size_t size, int which, string filename) {
	FILE *fp;
	size_t pos;

	pos=(which) * (size);
	if (!exist(filename)) {
		if ((fp=fopen(filename.c_str(), "a+b")) == NULL) {
			debug("Cannot write to %s", filename.c_str());
			exit(1);
	 	}
	} else {
		if ((fp=fopen(filename.c_str(), "r+b")) == NULL) {
			debug("Cannot write to %s", filename.c_str());
			exit(1);
		}
	}
	rewind(fp);
	if (pos > 0) fseek(fp, pos, SEEK_SET);
	fwrite(item, size, 1, fp);
	fflush(fp);
	fclose(fp);
}

void SaveLink(LinkRec *item, int which) {
	FILE *fp;
	size_t pos;

	pos=(which) * (sizeof(LinkRec));
	if (!exist("./data/links.dat")) {
		if ((fp=fopen("./data/links.dat", "a+b")) == NULL) {
			debug("Cannot write to ./data/links.dat");
			exit(1);
	 	}
	} else {
		if ((fp=fopen("./data/links.dat", "r+b")) == NULL) {
			debug("Cannot write to ./data/links.dat");
			exit(1);
		}
	}
	rewind(fp);
	if (pos > 0) fseek(fp, pos, SEEK_SET);
	fwrite(item, sizeof(LinkRec), 1, fp);
	fflush(fp);
	fclose(fp);
}

int LoadLink(LinkRec *p, int which) {
long pos;
FILE *fr;

if (which==0) return(0);
     pos=(which) * (sizeof(LinkRec));
     if ((fr=fopen("./data/links.dat", "r+")) == NULL) {
       debug("Can't read from ./data/links.dat");
     }
     rewind(fr);
     fseek(fr, pos, SEEK_SET);
     if (!fread((LinkRec *)p, sizeof(LinkRec), 1, fr)) {
       fclose(fr); 
       return(0);
     }
     fclose(fr);
     return(1);
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

void save_sysinfo(sysinforec *p) {
FILE *fp;

     if (!exist("./data/sysinfo.dat")) {
       if ((fp=fopen("./data/sysinfo.dat", "a+b")) == NULL) {
         debug("Cannot write to ./data/sysinfo.dat");
         exit(1);
       }
     } else {
       if ((fp=fopen("./data/sysinfo.dat", "r+b")) == NULL) {
         debug("Cannot write to ./data/sysinfo.dat");
         exit(1);
       }
     }
     rewind(fp);
     fwrite((sysinforec *)p, sizeof(sysinforec), 1, fp);
     fflush(fp);
     fclose(fp);
}


int load_sysinfo(sysinforec *p) {
FILE *fr;
     if ((fr=fopen("./data/sysinfo.dat", "r+")) == NULL) {
       debug("Can't read from ./data/sysinfo.dat");
     }
     if (!fread((sysinforec *)p, sizeof(sysinforec), 1, fr)) {
       fclose(fr); 
       return(0); 
     }
     fclose(fr);
     return(1);
}

int load_user(userrec *p, string loginname) {
  userrec aUser;
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

int load_user_from_file(userrec *p, int which) {
long pos;
FILE *fr;
     if (which==0) return(0);
     pos=(which) * (sizeof(userrec));
     if ((fr=fopen("./data/system.dat", "r+")) == NULL) {
       debug("Can't read from ./data/system.dat");
     }
     rewind(fr);
     fseek(fr, pos, SEEK_SET);
     if (!fread((userrec *)p, sizeof(userrec), 1, fr)) {
       fclose(fr); 
       return(0); 
     }
     fclose(fr);
     return(1);
}

int load_security(securityrec *p, int which) {
long pos;
FILE *fr;

     if (which==0) return(0);
     pos=(which) * (sizeof(securityrec));
     if ((fr=fopen("./data/security.dat", "r+")) == NULL) {
       debug("Can't read from ./data/security.dat");
     }
     rewind(fr);
     fseek(fr, pos, SEEK_SET);
     if (!fread((securityrec *)p, sizeof(securityrec), 1, fr)) {
       fclose(fr); 
       return(0); 
     }
     fclose(fr);
     return(1);
}

void save_security(securityrec *p, int which) {
long pos;
FILE *fp;

     pos=(which) * (sizeof(securityrec));
     if (!exist("./data/security.dat")) {
       if ((fp=fopen("./data/security.dat", "a+b")) == NULL) {
         debug("Cannot write to ./data/security.dat");
         exit(1);
       }
     } else {
       if ((fp=fopen("./data/security.dat", "r+b")) == NULL) {
         debug("Cannot write to ./data/security.dat");
         exit(1);
       }
     }
     rewind(fp);
     fseek(fp, pos, SEEK_SET);
     fwrite((securityrec *)p, sizeof(securityrec), 1, fp);
     fflush(fp);
     fclose(fp);
}

void save_user2(userrec *p, int which) {
long pos;
FILE *fp;

     pos=(which) * (sizeof(userrec));
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
     fwrite((userrec *)p, sizeof(userrec), 1, fp);
     fflush(fp);
     fclose(fp);
}

void Connection::SaveMe(void) {
	if (!getGuest()) {
		save_user(getUser());
	}
}

void save_user(userrec *p) {
	UserList.Insert(*p);
}

int totalusers(void) {

	return UserList.numLeaves();
}

int TotalLinks(void) {
int i=0;
LinkRec temp;

  while (1) {
    i++;
    if (!LoadLink(&temp, i)) return(i-1);
  }
}
    
int totalSecurityRecords(void) {
int i=0;
securityrec temp;

  while (1) {
    i++;
    if (!load_security(&temp, i)) return(i-1);
  }

}

int Connection::printfile(string filename) {
  FILE *filey;
  char line[2048];
                
  filey = fopen(filename.c_str(),"r");
  if (!filey) {
    return 0;
  }
  while (fgets(line, 2047, filey) != NULL) {
    npr(string(line) + "\r");
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


