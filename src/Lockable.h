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

#ifndef LOCK_H
#define LOCK_H

#include <pthread.h>

class Lockable {

	private:
		pthread_mutex_t *myMutex;
		
	public:
		
		void _Lock(void) {  // lock so other threads can't use this upcoming portion of the code
			pthread_mutex_lock(myMutex);
		}

		void _Unlock(void) { // unlock mutex so other threads can use it
			pthread_mutex_unlock(myMutex);
		}
		
		Lockable() {  // constructor
			myMutex = new pthread_mutex_t;
			pthread_mutex_init(myMutex, NULL);
			//cout << "Lockable()" << endl;
		}

		~Lockable() { // destructor
			pthread_mutex_destroy(myMutex);
			delete myMutex;
			//cout << "~Lockable()" << endl;
		}
};
#endif
