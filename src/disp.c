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
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h> /* close */
#include <cctype>
#include <string.h>
#include <string>
#include <stdarg.h>  /* va_start(); vsprintf(), etc */

using namespace std;

void npr(string message) {

char s[401],num[10],chs[10]; 
int pipeflag=0, slen,i,x,wrote;
slen=message.length();

strcpy(s,"");
  for (i=0,x=0; i<slen; i++) {
    if (pipeflag==2) { 
      pipeflag=0; 
      sprintf(num,"%s%c",num,message[i]); 
      sprintf(chs, "%c",27);
      strcat(s,chs);
           if (!strcmp(num,"01")) strcat(s,"[0;34m");
      else if (!strcmp(num,"02")) strcat(s,"[0;32m");
      else if (!strcmp(num,"03")) strcat(s,"[0;36m");
      else if (!strcmp(num,"04")) strcat(s,"[0;31m");
      else if (!strcmp(num,"05")) strcat(s,"[0;35m");
      else if (!strcmp(num,"06")) strcat(s,"[0;33m");
      else if (!strcmp(num,"07")) strcat(s,"[0;37m");
      else if (!strcmp(num,"08")) strcat(s,"[1;30m");
      else if (!strcmp(num,"09")) strcat(s,"[1;34m");
      else if (!strcmp(num,"10")) strcat(s,"[1;32m");
      else if (!strcmp(num,"11")) strcat(s,"[1;36m");
      else if (!strcmp(num,"12")) strcat(s,"[1;31m");
      else if (!strcmp(num,"13")) strcat(s,"[1;35m");
      else if (!strcmp(num,"14")) strcat(s,"[1;33m");      
      else if (!strcmp(num,"15")) strcat(s,"[1;37m");
      else if ((num[0]=='b') || (num[0]=='B')) {

 // background colors go here :)
      }

    } else if (pipeflag==1) { 
       pipeflag++; 
       sprintf(num,"%c",message[i]); 
       if (message[i]=='|') {
         sprintf(chs,"%c",'|');
         strcat(s,chs);
       }
    } else if (message[i]=='|') { 
      pipeflag++;
    } else { 
       sprintf(chs,"%c",message[i]);
       strcat(s,chs);
    }
  }

  if (pipeflag > 0) {
    sprintf(chs,"%c",'|');
    strcat(s,chs);
    if (pipeflag==2) strcat(s,num);
  }
  slen=strlen(s);
  printf("%s",s);
}

void snpr(char *fmt, ...) {
va_list ap; char s[256]; int i=0;
string s2;
  va_start(ap, fmt);
  vsprintf(s, fmt, ap);
  va_end(ap);
 s2 = s;
 npr(s2);
 
}

void cls() {
  snpr("%c[2J",27);
}


void printfile(string filename) {
  FILE *filey;   
  char line[256];
  char temp[257];
    
  filey = fopen(filename.c_str(),"r");
  if (!filey) {
    printf("printfile(): Cannot read %s\n",filename.c_str());
    return;
  }  
  while (fgets(line, 255, filey) != NULL) {
    line[strlen(line)] = 0;
    sprintf(temp,"%s\r",line);
    npr(temp);
  }
  fclose(filey);
}

int main(int argc, char *argv[]) {
if (argc == 1) {
 printf("%s: Missing filename argument\r\n\n   Usage: %s filename.ext\r\n",argv[0],argv[0]);
} else 
  printfile(argv[1]);

return true;
}


