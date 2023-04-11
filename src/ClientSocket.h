
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

#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include "Socket.h"

const int TYPE_CLIENTSOCKET = 3;

class ClientSocket : public Socket {
		bool connected;
		int myPort;
	public:
		int Connect(string Hostname, unsigned short port);

		ClientSocket(bool disconnectme=true) {
			myClose=disconnectme;
			connected=false;
			myPort=-1;
		}
		
		ClientSocket(int i) : Socket(i) {
			connected=true;
			myPort=-1;	
		}

		~ClientSocket() {

		}
};

#endif