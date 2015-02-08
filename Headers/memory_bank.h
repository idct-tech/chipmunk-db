#pragma once
#define byte unsigned char
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <map>
#include <vector>
#include "memory_entry.h"
#include "journal_entry.h"
#include "metadata_search_instruction.h"
#include <algorithm>
#include "helpers.h"
#include "logger.h"
using namespace std;

class memory_bank {
private:
	int count_journal;
	map<string, memory_entry> itemsAll;
	map<string, string> parse_metadata(string metadata_string);
	map<string, metadata_search_instruction> parse_metadata_search(string metadata_string);
	vector<string> split_subset(string subset_string);
	bool subset_contains(vector<string> subset, string element);
	void journal_action(journal_operation operation, string identifier);

	int count;
	int count_lookup;
public:
	vector<journal_entry> journal;
	memory_bank();
	bool add(string identifier, string metadata, byte* data, size_t data_length);
	string find_or(string metadata, string subset);
	string find_and(string metadata, string subset);
	bool remove(string identifier);
	memory_entry get(string identifier);
};