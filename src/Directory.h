
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

/* Directory.h
 *    Provides an easier way to list files within a given directory.
 */

#include <dirent.h>
#include <errno.h>
#include <string>

class Directory {
	private:
		int myError;
		string myPath;
		DIR *dir;

	public:
		Directory(string path) {
			myPath=path;
			if (!(dir=opendir(path.c_str()))) {
				myError=errno;
			} else myError=0;
		}

		Directory() {
			myError=0;
			dir=NULL;
		}
		
		~Directory() {
			if (dir != NULL) 
				closedir(dir);
			myError=0;
		}

		operator int() {
			return myError;
		}

		int OpenDirectory(string path) {
			if (dir != NULL) {
				return -1;
			} else {
				if (!(dir=opendir(path.c_str()))) {
					myError=errno;
					return 0;
				} else myError=0;
				return 1;
			}
		}

		int CloseDirectory(void) {
			if (dir != NULL)
				closedir(dir);
			else 
				return 0;
			dir=NULL;
			return 1;
		}

		operator bool() {
			if (myError) return false;
			else return true;
		}
		
		// allows you to return the next file with the pattern specified.
		// this does not allow wildcards at this time.
		string Next(string pattern) {
			struct dirent *curFile;
			string Result;
			while (Result.length() == 0) {
				if (curFile=readdir(dir)) {
					if (string(curFile->d_name).find(pattern) != string::npos) Result = string(curFile->d_name);
				} else {
					return "";
				}
			}
			return Result;
		}

		string Next(void) {
			struct dirent *curFile;
			
			if (curFile=readdir(dir)) {
				return string(curFile->d_name);
			} else {
				return "";
			}
		}
		
		int Rewind(void) {
			if (dir != NULL) rewinddir(dir);
			else return 0;	

			return 1;
		}

};
