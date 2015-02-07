#pragma once
#define byte unsigned char
#include <iostream>
#include <map>

using namespace std;

struct memory_entry {
	byte* data;
	int   length;
	string identifier;
	map<string, string> metadata;
};