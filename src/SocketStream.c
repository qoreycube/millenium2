#include "SocketStream.h"

SocketStream &operator<<(SocketStream &out, string s) {
	out.Send(s);
	return out;
}

SocketStream &operator<<(SocketStream &out, char ch) {
	string st;
	st+=ch;
	out.Send(st);
	return out;
}

SocketStream &operator<<(SocketStream &out, int i) {
	string st = SocketStream::LongToStr(i);
	out.Send(st);
	return out;
}

