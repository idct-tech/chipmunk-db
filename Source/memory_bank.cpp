#include "../Headers/memory_bank.h"
using namespace std;
memory_bank::memory_bank() {
		count = 0;
		count_lookup = 0;
		count_journal = 0;
	}

map<string, string> memory_bank::parse_metadata(string metadata) {
	//stripping the metadata
	vector<string> metadata_entries = helpers::explode(metadata, '#');
	map<string, string> metadata_map;
	for (std::vector<string>::iterator it = metadata_entries.begin(); it != metadata_entries.end(); ++it) {
		vector<string> metadata_entry = helpers::explode(*it, ':');
		metadata_map.insert(std::pair<string, string>(metadata_entry[0], metadata_entry[1]));
	}

	return metadata_map;
}

vector<string> memory_bank::split_subset(string subset_string) {
	return helpers::explode(subset_string,'#');
}

map<string, metadata_search_instruction> memory_bank::parse_metadata_search(string metadata) {
	//stripping the metadata
	vector<string> metadata_entries = helpers::explode(metadata, '#');
	map<string, metadata_search_instruction> metadata_map;
	for (std::vector<string>::iterator it = metadata_entries.begin(); it != metadata_entries.end(); ++it) {
		vector<string> metadata_entry = helpers::explode(*it, ':');

		metadata_search_instruction instruction;
		instruction.condition = (metadata_search_instruction::condition_type) metadata_entry[1].c_str()[0];
		instruction.value = metadata_entry[2];

		metadata_map.insert(std::pair<string, metadata_search_instruction>(metadata_entry[0], instruction));
	}

	return metadata_map;
}

void memory_bank::journal_action(journal_operation operation, string identifier) {
	journal_entry entry;
	entry.identifier = identifier;
	entry.operation = operation;
	if (operation == ADD) {
		++count_journal;
	}
	else {
		--count_journal;
	}
	journal.push_back(entry);
}

bool memory_bank::subset_contains(vector<string> subset, string element) {
	if (find(subset.begin(), subset.end(), element) != subset.end())
	{
		return true;
	}

	return false;
}

bool memory_bank::add(string identifier, string metadata, byte* data, size_t data_length)
	{
		byte* temporary = new byte[data_length];
		memcpy(temporary, data, data_length);
		memory_entry newdata;

		map<string, string> metadata_map = parse_metadata(metadata);
		newdata.metadata = metadata_map;

		newdata.data = temporary;
		newdata.length = data_length;
		newdata.identifier = identifier;
		itemsAll[identifier] = newdata;
		count++;
		journal_action(ADD, identifier);

		return true;
	}

string memory_bank::find_or(string metadata, string subset)
	{
		map<string, metadata_search_instruction> metadata_map = parse_metadata_search(metadata);
		memory_entry data;
		string response = "";
		map<string, memory_entry>::iterator iter = itemsAll.begin();
		vector<string> subset_elements = split_subset(subset);
		bool use_subset = (subset_elements.size() > 0);
		while (iter != itemsAll.end())
		{
			if (use_subset == false || subset_contains(subset_elements, iter->first)) {
				for (map<string, metadata_search_instruction>::iterator it = metadata_map.begin(); it != metadata_map.end(); ++it)			{

					if (iter->second.metadata.count(it->first)) {
						metadata_search_instruction instruction = it->second;
						switch (instruction.condition) {
						case metadata_search_instruction::EQUALS:
							if (instruction.value == iter->second.metadata[it->first]) {
								response.append("|" + iter->first);
								break;
							}
							break;
						case metadata_search_instruction::NOT:
							if (instruction.value != iter->second.metadata[it->first]) {
								response.append("|" + iter->first);
								break;
							}
						}

					}
				}
			}
			iter++;
		}

		return response;
	}

string memory_bank::find_and(string metadata, string subset)
{
	map<string, metadata_search_instruction> metadata_map = parse_metadata_search(metadata);

	memory_entry data;
	string response = "";
	map<string, memory_entry>::iterator iter = itemsAll.begin();
	vector<string> subset_elements = split_subset(subset);
	bool use_subset = (subset_elements.size() > 0);

	while (iter != itemsAll.end())
	{
		if (use_subset == false || subset_contains(subset_elements, iter->first)) {
			bool valid = true;
			for (map<string, metadata_search_instruction>::iterator it = metadata_map.begin(); it != metadata_map.end(); ++it)			{

				if (iter->second.metadata.count(it->first)) {
					metadata_search_instruction instruction = it->second;
					switch (instruction.condition) {
					case metadata_search_instruction::EQUALS:
						if (instruction.value != iter->second.metadata[it->first]) {
							valid = false;
							break;
						}
						break;
					case metadata_search_instruction::NOT:
						if (instruction.value == iter->second.metadata[it->first]) {
							valid = false;
							break;
						}
					}
				}
				else {
					valid = false;
					break;
				}
			}
			if (valid) {
				response.append("|" + iter->first);
			}
		}

		iter++;
	}
	return response;
}

bool memory_bank::remove(string identifier)
	{
		if (itemsAll.erase(identifier)) {
			count--;
			journal_action(REMOVE, identifier);
		}

		return true;
	}

memory_entry memory_bank::get(string identifier)
	{
		memory_entry data;
		if (itemsAll.count(identifier)>0)
		{
			memory_entry data(itemsAll[identifier]);
			return data;
		}
		memory_entry empty;
		empty.data = NULL;
		empty.length = 0;

		return empty;
	}
