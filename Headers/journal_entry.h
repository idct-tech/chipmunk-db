#pragma once
#include <iostream>
#include "journal_operation.h"

using namespace std;

struct journal_entry {
	string identifier;
	journal_operation operation;
};
