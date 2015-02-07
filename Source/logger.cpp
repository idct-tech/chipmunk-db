#include "../Headers/logger.h"

string logger::itos(int i) {
	stringstream s;
	s << i;
	return s.str();
}

void logger::log(string message) {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%dT%X", &tstruct);

	cout << '[' << buf << "] " << message << '\n';
}