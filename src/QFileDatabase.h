#include "Lockable.h"
#include <stdio.h>

class QFileDatabase : Lockable {
	private:
		string myFilename;
		size_t mySize;
		
	public:
		QFileDatabase(string filename, size_t size) {
			myFilename = filename;	
			mySize=size;
		}
		
		int Read(void *obj, size_t rec) {
			FILE *in;
			int ret;
			_Lock();
				in=fopen(myFilename.c_str(), "r+");
				if (!in) {
					in=fopen(myFilename.c_str(),"w+");
					if (!in) {
						_Unlock();
						return 0;
					}	
				}
				if (rec * mySize != 0)
					fseek(in, rec * mySize, SEEK_SET);
				if (!fread(obj, mySize, 1, in)) ret=0;
				else ret=1;
				fclose(in);
			_Unlock();		
			return ret;
		}

		int Write(void *obj, size_t rec) { //size_t size, size_t numElements=1) {
			FILE *in;
			_Lock();
				in=fopen(myFilename.c_str(), "r+");
				if (!in) {
					in=fopen(myFilename.c_str(), "w+");
					if (!in) {
						_Unlock();
						return 0;
					}
				}
				if (rec * mySize != 0)
					fseek(in, rec * mySize, SEEK_SET);
				fwrite(obj, mySize, 1, in);			
				fclose(in);
			_Unlock();		
		}

		int TotalRecords(void) {
			FILE *in;
			_Lock();
				in=fopen(myFilename.c_str(), "r+");
				if (!in) {
					in=fopen(myFilename.c_str(), "w+");
					if (!in) {
						_Unlock();
						return 0;
					}
				}
				fseek(in, 0, SEEK_END);
				int i=ftell(in);
				i/=mySize;
				fclose(in);
			_Unlock();
			return i-1;
		}

};
