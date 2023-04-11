#ifndef REGULAREXPRESSION_H
#define REGULAREXPRESSION_H

#include <regex.h>
#include <string>

class REException {
	private:
		int myError;
		regex_t *myREptr;
	public:
		REException(regex_t *REptr, int i) {
			myError=i;
			myREptr=REptr;
		}
		
		~REException() {
			
		}
		
		string toString() {
			char errr[61];
			regerror(myError,myREptr,errr,60);
			return string(errr);
		}
};

class RegularExpression {
	private:
		string myREString;
		regex_t myRE;
		int myFlags;
		
		bool CheckValid(string s) {
			return true;
		}
		
	public:
		RegularExpression(string s, int flags=REG_ICASE | REG_EXTENDED) {
			Compile(s,flags);
		}
		
		void Compile(string s, int flags=REG_ICASE | REG_EXTENDED) {
			if (CheckValid(s)) {
				myREString=s;
				myFlags=flags;
				string newStr="^" + myREString + "$";
				int i=regcomp(&myRE, newStr.c_str(), myFlags);
				
				if (i==0) return;
				throw REException(&myRE,i);
			}
		}
		
		~RegularExpression() {
			regfree(&myRE);
		}
		
		bool Match(string newString, int flags=0) {
			int e=regexec(&myRE,newString.c_str(),0,NULL,flags);
			return (e==0);
		}
};

typedef RegularExpression RegExp;

#endif
