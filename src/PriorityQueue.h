
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

#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include<vector>
#include<queue>
#include "Lockable.h"

template<class DataType>
class PriorityQueue : Lockable {
	private:
		int mySize;
		vector<queue< DataType> *> myQueues;
	public:
		
		bool pop(DataType &DT) {
			_Lock();
				int i=0;
				while (i <= mySize) {
					if (i == mySize) {
						_Unlock();
						return false;
					}
					
					if (myQueues[i] != NULL) break;
					i++;
				}
				
				DT = myQueues[i]->front();
				myQueues[i]->pop();
				if (myQueues[i]->empty()) {
					delete myQueues[i];
					myQueues[i]=NULL;
				}
			_Unlock();
			return true;
		}		
		
		void push(int prior, const DataType &dt) {
			//cerr << "Locking..." << endl;
			_Lock();
//				cerr << "PUSH: Prior: " << prior << "   Val: " << dt;
				if (myQueues[prior] != NULL)  {
					//cerr << "       != NULL" << endl;
					myQueues[prior]->push(dt);
				} else {
					//cerr << " == NULL" << endl;
					myQueues[prior] = new queue<DataType>;
					myQueues[prior]->push(dt);
				}
		//		cerr << "Unlocking" << endl;
			_Unlock();
		}
		
		PriorityQueue(int size) {
			_Lock();
				queue<DataType> *ptr=NULL;
				for (int i=0; i<size; i++)
					myQueues.push_back(ptr);
				mySize=size;
			_Unlock();
		}

		~PriorityQueue() {
			for (int i=0; i<mySize; i++) 
				if (myQueues[i] != NULL) {
					delete myQueues[i];
					myQueues[i]=NULL;
				}
		}
};
#endif
