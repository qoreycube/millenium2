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

#ifndef THREAD_H
#define THREAD_H

class Thread {
	public:
		Thread() { 
			myCompleted=false; 
		}
		
		int Start(void * arg) {
			Arg(arg); // store user data
			int code = pthread_create(&myThread, NULL, Thread::EntryPoint, this);
			return code;
		}

	pthread_t myThread;
	void setCompleted(bool b) { myCompleted=b; }
	bool getCompleted(void) { return myCompleted; }
   protected:
	bool myCompleted;
	void * Arg_;

	void Run(void * arg) {
	   Setup();
	   Execute( arg );
	   
	}
	
	static void * EntryPoint(void * pthis) {
	   Thread * THREADptr = (Thread*)pthis;
  	   THREADptr->Run( (void *)(THREADptr->Arg()) );
	   THREADptr->setCompleted(true);
	   return NULL;
	}

	virtual void Setup() {}
	
	virtual void Execute(void *arg) { // OVERRIDE ME IN A NEW CLASS
	
	}
      
	void * Arg() const { return Arg_; }
	void Arg(void* a){ Arg_ = a; }

};

#endif
