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

#include <stdio.h>
#include <string.h>

void strip_file(char *fn, FILE *out, FILE *out2)
{
  FILE *f;
  char s[301],*ss,s2[301],hm[302];

  f=fopen(fn,"r");
  if (!f)
    return;

  fprintf(out,"\n/* File: %s */\n\n",fn);
  fprintf(out2,"\n/* File: %s */\n\n",fn);
  while (fgets(s,250,f)) {
    ss=strchr(s,'\n');
    if (ss)
      *ss=0;
    if ((s[0]) && (strchr("{}\t/# ",s[0])==NULL) && (s[strlen(s)-3]==')') &&
        (s[strlen(s)-1]=='{') && (!strstr(s,"if ") && (!strstr(s,"for(")) &&
	(!strstr(s,"for ("))) && (!strstr(s,"while")) && (!strstr(s,"switch"))) {
      if (strncmp(s,"static",6)) {
        if (!strstr(s, "::")) {
	  sprintf(s2,"%%-%d.%ds",strlen(s)-2,strlen(s)-2);
          sprintf(hm,s2,s);
          fprintf(out,"%s;\n",hm);
	  fprintf(out2,"%s;\n",hm);
	} else {
	  sprintf(s2, "%%-%d.%ds",strlen(s)-2, strlen(s)-2);
	  sprintf(hm,s2,s);
	  fprintf(out2,"%s;\n",hm);	  
	}
      }
    }
  }
  fclose(f);
}


/****************************************************************************/

int main(int argc, char *argv[])
{
  int i,i1;
  FILE *out, *out2, *tmpin;
  char *ss,s[161];

  if (argc != 4) {
    printf("Run the STRIP program only from the makefile.\n\n");
    return(-1);
  }

  out=fopen(argv[1],"w");
  out2=fopen(argv[3],"w");
  
  fprintf(out,"#ifndef _FCNS_H_\n#define _FCNS_H_\n");

  tmpin = fopen(argv[2],"r");
  do {
    i1=fscanf(tmpin,"%s",s);
    if (i1>0) {
      if ((ss=strstr(s,".obj"))!=NULL) {
        *ss=0;
        strcat(s,".c");
        ss=strrchr(s,'\\');
        if (!ss)
          ss=s;
        else
          ++ss;
      } else ss=s;
      strip_file(ss,out,out2);
    }
  } while (i1>0);
  fclose(tmpin);

  fprintf(out,"\n#endif\n");
  fprintf(out2,"\n#endif\n");
  fclose(out);
  fclose(out2);
}
