#include "../Headers/memory_dispatcher.h"

memory_bank memory_dispatcher::main_memory;

void memory_dispatcher::listenerFunc(string a) {

	for (;;)
	{
		int sockfd, n;
		struct sockaddr_in servaddr, cliaddr;
		socklen_t len;
		char mesg[INPUT_BUFFER];
		char tosend[10];
		sockfd = socket(AF_INET, SOCK_DGRAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(8906);
		bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		for (;;)
		{
			len = sizeof(cliaddr);
			n = recvfrom(sockfd, mesg, INPUT_BUFFER, 0, (struct sockaddr *)&cliaddr, &len);
			boost::thread listenerThread(&userHandler, sockfd, mesg, cliaddr, n, len);
			mesg[n] = 0;
		}
	}
}

void memory_dispatcher::userHandler(int sockfd, char* msg, sockaddr_in client, int len, int len2)
{
	char* t_ident = msg + 1;
	if (msg[0] == 'G')
	{
		logger::log("GET action for: " + helpers::chtos(t_ident));
		memory_entry temp = main_memory.get(t_ident);
		if (temp.data == NULL)
		{
			int m = sendto(sockfd, "NODATA", 7, 0, (struct sockaddr *)&client, len2);
		}
		else
		{
			sendto(sockfd, temp.data, temp.length, 0, (struct sockaddr *)&client, len2);
		}
	}
	else if (msg[0] == 'O' || msg[0] == 'A') {

		char operation = msg[0];
		char* msg_ptr = msg + 1;

		vector<string> msg_parts = helpers::explode(msg_ptr, '\n');
		string query = msg_parts[0];

		string subset = "";
		if (msg_parts.size() > 1) {
			subset = msg_parts[1];
		}

		char* response;
		switch (operation) {
		case 'O':
				logger::log("FIND_OR action...");
				response = main_memory.find_or(query, subset);
				logger::log("...finished");
			break;
		case 'A':
				logger::log("FIND_AND action...");
				response = main_memory.find_and(query, subset);
				logger::log("...finished");
			break;
		}


		int len = 0;
		len = strlen(response);
		if (len > 0) {
			int m = sendto(sockfd, response + 1, len - 1, 0, (struct sockaddr *)&client, len2);
		}
		else {
			int m = sendto(sockfd, "NODATA", 7, 0, (struct sockaddr *)&client, len2);
		}

	}
	else if (msg[0] == 'S'){
		//getting the ID
		int where = 0;
		while (t_ident[where] != '|')
			++where;
		char* ident = new char[where];
		ident[where] = '\0';

		where = 0;
		while (t_ident[where] != '|')
		{
			ident[where] = t_ident[where];
			++where;
		}
		logger::log("SET action for: " + helpers::chtos(ident));

		//getting the METADATA
		int whereAfterIdent = where;
		while (t_ident[++where] != '|')
		{
		}
		char* metadata = new char[where - whereAfterIdent + 2];

		where = whereAfterIdent;

		while (t_ident[++where] != '|') {
			metadata[where - whereAfterIdent - 1] = t_ident[where];
		}
		metadata[where - whereAfterIdent - 1] = '\0';
		main_memory.add(ident, (char*)metadata, (byte*)(msg + where + 2), len - where - 2);
		sendto(sockfd, "SAVED", 6, 0, (struct sockaddr *)&client, len2);
	}
	else if (msg[0] == 'R'){
		main_memory.remove(t_ident);
		logger::log("DEL action for: " + helpers::chtos(t_ident));
		sendto(sockfd, "REMVD", 6, 0, (struct sockaddr *)&client, len2);
	}
}

void memory_dispatcher::saverFunc() {
	do
	{
		boost::this_thread::sleep(boost::posix_time::seconds(60));
		vector<journal_entry> journal(main_memory.journal);
		main_memory.journal.clear();
		int i = 0;
		int max = journal.size();
		logger::log("Journal(" + logger::itos(max) + ") saving...");
		for (; i<max; i++)
		{
			{
				char path[256];
				strcpy(path, "data/");
				strcat(path, journal[i].identifier.c_str());
				if (journal[i].operation == REMOVE)
				{
					remove(path);
				}
				else {
					FILE *pFile = fopen(path, "w+");
					if (pFile == NULL)
					{
						perror("Error opening file");
					}
					else
					{
						int max = main_memory.get(journal[i].identifier).length;
						byte* data = main_memory.get(journal[i].identifier).data;
						for (int i = 0; i < max; i++)
						{
							fputc(data[i], pFile);
						}
						fclose(pFile);
					}
				}
			}
			{
				char path[256];
				strcpy(path, "metadata/");
				strcat(path, journal[i].identifier.c_str());
				if (journal[i].operation == REMOVE)
				{
					remove(path);
				}
				else {
					FILE *pFile = fopen(path, "w+");
					if (pFile == NULL)
					{
						perror("Error opening file");
					}
					else
					{
						int max = main_memory.get(journal[i].identifier).length;
						map<string, string> temp_metadata_entry = main_memory.get(journal[i].identifier).metadata;

						typedef std::map<std::string, std::string>::iterator it_type;
						bool first = true;
						for (it_type iterator = temp_metadata_entry.begin(); iterator != temp_metadata_entry.end(); iterator++) {

							char* key = (char*)iterator->first.c_str();
							char* value = (char*)iterator->second.c_str();

							int key_len = strlen(key);
							int value_len = strlen(value);

							if (first == true) {
								first = false;
							}
							else {
								fputc('#', pFile);
							}

							for (int i = 0; i < key_len; i++) {
								fputc(key[i], pFile);
							}

							fputc(':', pFile);

							for (int i = 0; i < value_len; i++) {
								fputc(value[i], pFile);
							}


						}
						fclose(pFile);
					}
				}
			}
		}
		logger::log("...saved");
	} while (1);
}

void memory_dispatcher::load()
{
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir("data")) != NULL) {
		int entries = 0;
		while ((ent = readdir(dir)) != NULL) {
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;

			}
			char path[256];
			char path_metadata[256];
			strcpy(path, "data/");
			strcpy(path_metadata, "metadata/");
			strcat(path, ent->d_name);
			strcat(path_metadata, ent->d_name);

			//int pos = strlen(ent->d_name);

			int size = 0;
			int size_metadata = 0;
			char c;
			{
				{
					//calculate size
					FILE *pFile = fopen(path, "r");
					if (pFile == NULL)
					{
						perror("Error opening file");
					}
					else
					{
						do {
							size++;
							c = fgetc(pFile);
						} while (c != EOF);
						fclose(pFile);
					}
					//read file
				}
			}
			{
				{
					//calculate size (metadata)
					FILE *pFile = fopen(path_metadata, "r");
					if (pFile == NULL)
					{
						perror("Error opening file");
					}
					else
					{
						do {
							size_metadata++;
							c = fgetc(pFile);
						} while (c != EOF);
						fclose(pFile);
					}
					//read file
				}
			}
			byte* tempdata = new byte[size];
			byte* tempdata_metadata = new byte[size_metadata];
			{
				int pos = 0;
				FILE *pFile = fopen(path, "r");
				if (pFile == NULL)
				{
					perror("Error opening file");
				}
				else
				{
					do {
						c = fgetc(pFile);
						tempdata[pos] = c;
						pos++;
					} while (c != EOF);
					fclose(pFile);
				}
			}
			{
				int pos = 0;
				FILE *pFile = fopen(path_metadata, "r");
				if (pFile == NULL)
				{
					perror("Error opening file");
				}
				else
				{
					do {
						c = fgetc(pFile);
						tempdata_metadata[pos] = c;
						pos++;
					} while (c != EOF);
					fclose(pFile);
				}
			}

			memory_dispatcher::main_memory.add(ent->d_name, helpers::chtos((char*)tempdata_metadata), tempdata, size - 1);
			++entries;
		}
		logger::log("Loaded: " + logger::itos(entries) + " entries from disk.");
	}
}