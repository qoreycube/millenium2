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

void log(string s) {
  FILE *in;

  s+='\n';
  in=fopen("m2.log", "a");
  fputs(s.c_str(), in);
  fclose(in);  
}

void syslog(string s) {

	int i=0;
	QNodeDataRec nd;

	while (i < connection.size()) {
		nd=connection[i]->getNodeData();
		if (testbit(USERBIT_SYSLOG, nd.User.flags)) {
			QInstantMessageRec Msg;
			Msg.message=s;
			Msg.msgType=MSG_SYSLOG;
			Msg.nodeTo=nd.Nodenum;
			Broadcast(Msg);
		}
		i++;
	}
	appendfile("./log/system.log","["+getDateTime(now())+"] " + s + '\n');
}

void debuglog(string s) {
  FILE *in;
//  s+='\n';
  in=fopen("./log/debug.log", "a");
  if (!in) { debug("Cannot write to ./tmp/error.log"); return; }
  fputs(s.c_str(), in);
  fclose(in);
}

void errorlog(string s) {
  FILE *in;
  s+='\n';
  in=fopen("./log/error.log", "a");

  if (!in) { 

    debug("Cannot write to ./tmp/error.log - %s"); 
    
    return; 
  }
  fputs(s.c_str(), in);
  fclose(in);

}

void appendfile(string fn, string s) {
  FILE *in;

  in=fopen(fn.c_str(), "a");
  fputs(s.c_str(), in);
  fclose(in);

}


void connectionlog(string s, int flag) {
	s = "[" + getDateTime(now()) + "] " +FlagName(flag) +" -> " + s + "\n";
	appendfile("./log/connection.log",s);
}
