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

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

using namespace std;

int main(int argc, char *argv[]) {
char s[20];
FILE *in;
int vermax;
int vermin;
int build;
int totbuild;
bool incMax=false;
bool incMin=false;
bool release=false;
	if (argc > 1) {
		for (int i=0; i<argc; i++)
			if (!strcmp(argv[i], "max")) incMax=true;
			else if (!strcmp(argv[i],"release")) release=true;
			else if (!strcmp(argv[i], "min")) incMin=true;
	}

	in = fopen(".m2version","rt");
	if (!in) { 
		vermax=1;
		vermin=0;
		build=0;
		totbuild=0;
	} else {
		fgets(s,15,in);
		vermax=atoi(s);
		fgets(s,15,in);
		vermin=atoi(s);
		fgets(s,15,in);
		build=atoi(s);
		fgets(s,15,in);
		totbuild=atoi(s);
		fclose(in);
	}
	
	if (incMax) {
		vermax++;
		build=-1;
	
	}
	if (incMin) {
		vermin++;
		build=-1;
		
	}
	build++;
	totbuild++;
	in = fopen(".m2version","wt");
	sprintf(s,"%d\r\n",vermax);
	fputs(s,in);
	sprintf(s,"%d\r\n",vermin);
	fputs(s,in);
	sprintf(s,"%d\r\n",build);
	fputs(s,in);
	sprintf(s,"%d\r\n",totbuild);
	fputs(s,in);
	fclose(in);
	if (release) sprintf(s,"%d.%d.%dr",vermax,vermin,build);
	else  sprintf(s,"%d.%d.%d",vermax,vermin,build);
	
	cout << s << endl;
}
