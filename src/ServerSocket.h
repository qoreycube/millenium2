#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

#include "Vector.h"
#include "Socket.h"
#include<signal.h>

const int TYPE_SERVERSOCKET = 2;

class ServerSocket : public Socket {
		
	private:
		int myListenPort;
		bool myListening;	
		unsigned int myMaxClients;
	public:
		class AcceptedSocket  {
			protected:
				ServerSocket *myServerPtr;
				Socket *mySocket;
			
			public:	

				Socket &getSocket(void) {
						return *mySocket;
				}
			
				AcceptedSocket(ServerSocket *msp, int SockFD) {
					myServerPtr=msp;
					mySocket = new Socket(SockFD);
				}
	
				~AcceptedSocket() {
					int i=0;
					myServerPtr->_Lock();			
					while (i < myServerPtr->Size()) {
						if ((*myServerPtr)[i].getSocketID() == mySocket->getSocketID()) {		
							myServerPtr->Erase(i);
							delete mySocket;
							myServerPtr->_Unlock();
							return;
						}
						i++;
					}
					myServerPtr->_Unlock();
				}

				void setHost(string s) {
					mySocket->setRemoteHost(s);
				}

				void setIP(string s) {
					mySocket->setRemoteIP(s);
				}
			};
		
		Vector<AcceptedSocket *> mySockets;
		
		ServerSocket() {
			myListening=false;
			myListenPort=-1;
			myMaxClients = 700;
			//cerr << "ServerSocket()" << endl;
			ResetSignals();
		}

		ServerSocket(int i) {
			myListening=false;
			myMaxClients = 700;
			myListenPort=i;
			//cerr << "ServerSocket(int)" << endl;
		}
		
		~ServerSocket() {
			//cerr << "~ServerSocket()" << endl;
		}
		void setMaxClients(unsigned int i) { myMaxClients=i; }
		
		void Erase(Socket *ptr) {
			int i=0;
			while (i < mySockets.size()) {
				if (ptr == &mySockets[i]->getSocket()) {
					mySockets.erase(i);
					return;
				}
				i++;
			}
		}
		void Erase(int i) {
			if (i >= mySockets.size()) throw Exception("ServerSocket::Erase", "Array Out of Bounds", (void *)this, TYPE_SERVERSOCKET);
			mySockets.erase(i);
			return;
		}
		
		int getListenPort(void) {
			return myListenPort;
		}
		
		void Listen(unsigned short port);

		bool CheckForConnection(int mili, int secs=0);
		
		Socket &operator[](unsigned int i) {
			Socket *sptr;
			  if (i >= mySockets.size()) throw Exception("operator[]","Array out of bounds", (void *)this, TYPE_SERVERSOCKET);
			  sptr=&(mySockets[i]->getSocket());
			return *sptr;
		}
		
		unsigned int Size(void) { return mySockets.size(); }
		Socket &Accept(void);
		
		void ResetSignals(void) {
			  signal(SIGPIPE, ServerSocket::HandleSignal);

		}
		
		static void HandleSignal(int sig) {
			  signal(SIGPIPE, ServerSocket::HandleSignal);
			  //cerr << "Signal: " << sig << "   BrokenPipe " << endl;
		}
};


#endif

