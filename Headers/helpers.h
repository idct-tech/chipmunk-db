#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;
class helpers {
	public:
		static vector<string> explode(const string&, const char);
		static string chtos(char* buf);
};