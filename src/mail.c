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

string Connection::FindUser(string s) {
	vector<QUserRec> vecUserList;
	npr("\r\n");	
	if (StrToInt(s) != -1) {
		QUserRec us;
		if (GetUserByNumber(StrToInt(s), us) != -1) {
			if (ny("Do you mean: "+string(us.chathandle)+" |07("+string(us.loginname)+") #"+s+"?")) {
				return string(us.loginname);
			}
		}
	}
		
	UserList.PreOrder(ConvertToVector, (void *)&vecUserList);
	
	for (int i=0; i < vecUserList.size(); i++) {
		string chathandle = string(vecUserList[i].chathandle);
		string loginname = string(vecUserList[i].loginname);

		if (stripcolors(chathandle).find(stripcolors(s)) != string::npos) {
			if (ny("Do you mean: |10"+string(vecUserList[i].chathandle)+" |07("+string(vecUserList[i].loginname)+") #"+IntToStr(vecUserList[i].number)+"?")) {
				return string(vecUserList[i].loginname);
			}
			continue;
		}
		
		if (loginname.find(s) != string::npos) {
			if (ny("Do you mean: |10"+string(vecUserList[i].chathandle)+" |07("+string(vecUserList[i].loginname)+") #"+IntToStr(vecUserList[i].number)+"?")) {
				return string(vecUserList[i].loginname);
			}
			continue;
		}
	}
	return string("");
}


void Connection::SendMail(void) {
	string s;
	npr("\r\n\n");
	cls();
	npr("|15 Send to whom: |07(|10Chat Handle|07, |10Login Name|07, or |10Number|07): ");
	s=input(40);
	string login = FindUser(s);
	if (login.length() == 0) {
		npr(SYSS+" User not found, aborted."+SYSF);
		pause();
		return;
	}
	
	QUserRec aUser;
	bool found=UserList.Search(aUser);
	if (!found) {
		npr("Username Not Found!\r\n");
		pause();
	} else {
		npr("\r\n     |15To: |10"+string(aUser.chathandle)+ " |07("+string(aUser.loginname)+") #"+IntToStr(aUser.number)+"\r\n");
		npr("|15Subject: ");
		string subject = input(41);

		pause();
	}
}

void Connection::Mail(void) {
	int i=0;
	int rec=-1;
	QFileDatabase mailFile("./mail/mbox"+IntToStr(getUser()->number)+".dat", sizeof(QMailRec));
	QMailRec mal;

	//sorry :(
listM:
	
	npr("##.From....................R.U.Date.....Subject..............................\r\n");
	line(LINE_TOP);
	
	if (exist("./mail/" + IntToStr(getUser()->number))) {
		int i=0;
		while (mailFile.Read((void *)&mal, i)) {
			npr(IntToStr(i,2));
			QUserRec tmpusr;
			if (GetUserByNumber(mal.from, tmpusr) == -1) {
				npr("|08Unknown #-01|07");
			} else {
				npr(spacecolors("|10" + string(tmpusr.chathandle) + " |15#" + IntToStr(tmpusr.number)+"|07", 20, ALIGN_LEFT));
				if (mal.replied) npr("|14R|07 ");
				else npr("  ");

				if (mal.urgent) npr("|12!|07 ");
				else npr(" ");


				npr("|15"+getDate(mal.sent)+"|07 ");
				npr(mal.subject);
				npr("\r\n");
			}
			i++;
		}
	} else {
		npr("|15\r\nYou have 0 messages at this time.|07\r\n\n");
	}
	bool done=false;
	while ((!done) && (!Finished)) {
		string s;
		npr("  |04-|12=|15[ |10Mail Menu |15]|12=|04- |07-> ");
		s=input(21);
		npr("\r\n");
		int inp=StrToInt(s);
		if (inp == -1) {
			if (s.length() == 0) { // they pushed enter
			//	rec++;
			//	if (rec > mailFile.TotalRecs()) {
			//		rec--;
			//	}
			} else {
				switch(toupper(s[0])) {
					case 'Q': done=1;
						  break;
					case 'L': goto listM;
					case 'S': SendMail();
						  break;
					case '?': printfile("./etc/menu.mail");
						  //help_menu();
						  break;
				}
			
			}

		} else {
			if (inp > i) {
				npr(SYSS+"Message " + IntToStr(rec) + " does not exist."+SYSF);
			} else {

			}

	
		}
	}
	
	
}

