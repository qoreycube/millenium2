#include "Lockable.h"
#include "typedef.h"

struct QMessage {
	QMessageHeader myHeader;
	QStringList    myMessage;
}

class MessageBase : public Lockable {
	QMessageBase myBase;
	public:
		MessageBase() {

		}

		int setBase(int i) {
			if (LoadFromFile(&myBase, sizeof(QMessageBase),i,"./data/mbase.dat")) return 1;
			else return 0;
		}
		
		void Lock() {
			_Lock();
		}

		void Unlock() {
			_Unlock();
		}

		int newMessage(QMessageHeader myHeader) {
			strcpy(filename, GenerateTemporaryFilename("./msgbase",15));

		}

		QMessageRec getMessage(int i) {

		}
		
};


struct QMessageBase {
	
	bool force;	     // force the user to see new posts (news/Op information)
	int minSecurity;     // Minimum security level
	char name[31];
	int usernum;         // -1 if not moderated by a user.
	char moderator[31];  // Name of moderator, or blank if not moderated by a user.
	char filename[20];   // temporary filename that keeps track of messageheaders
	
};

struct QMessageHeader {
	char filename[20];	// temporary filename
	char from[31];		// who is it from
	int reply;		// in reply to what message, -1 is no replies
	int replies;		// total number of replies
	
	
};

struct QMessageRec {
	QMessageHeader myHeader;
	QStringList Strings;
};
