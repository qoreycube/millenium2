#ifndef SOCKETSTREAM_H
#define SOCKETSTREAM_H

#include "Socket.h"

class SocketStream {
	private:
		Socket *mySocket;
	public:
		static string LongToStr(long i) {
			string st;
			char digit;	

			bool neg = (i<0) ? true : false;
			while (i > 0) {
				digit=i % 10;
				st= digit + st;
				i /= 10; 
			}
			if (neg) st= '-' + st;
			return st;
		}
		
		Socket *getSocket(void) { return mySocket; }
		SocketStream(Socket *aSock) {
			mySocket=aSock;
		}

		~SocketStream() {
		
		}
		void PutChar(char ch) {
			mySocket->PutChar(ch);
		}
		
		void Send(string s) {
			mySocket->Send(s);
		}

		int GetChar(int mili, int secs) {
			return mySocket->GetChar(mili, secs);
		}
};

#endif 
