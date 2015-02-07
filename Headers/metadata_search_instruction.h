#pragma once
#include <iostream>

using namespace std;

class metadata_search_instruction {
	public:
		enum condition_type { EQUALS='=',NOT='!' };
		string value;
		condition_type condition;

};