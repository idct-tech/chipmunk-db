#pragma once
#include <iostream>
#include <time.h>
#include <sstream>

using namespace std;

class logger {
	public:
		static void log(string message);
		static string itos(int i);
};