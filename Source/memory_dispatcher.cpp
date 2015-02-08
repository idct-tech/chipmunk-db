#include "../Headers/memory_dispatcher.h"

memory_bank memory_dispatcher::main_memory;

void memory_dispatcher::listenerFunc(string a) {

		int sockfd;
		struct sockaddr_in servaddr, cliaddr;
		socklen_t len;

		char tosend[10];
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
		servaddr.sin_port = htons(8906);
		bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
		logger::log("listening...");
		listen(sockfd, 1);
		for (;;)
		{
			logger::log("accepting...");
			int sock_connect = accept(sockfd, NULL, NULL);
			boost::thread listenerThread(&userHandler, sock_connect);
			//close(sockfd);
			/*
			for (;;)
			{



				logger::log("waiting..." + logger::itos(sock_connect));
				int n;
				char mesg[INPUT_BUFFER];
				string message = "";
				do {
					n = recv(sock_connect, &mesg, 1, 0);
					if (n > 0) {
						logger::log("got block: " + helpers::chtos(mesg));
						message += helpers::chtos(mesg);
					}
				} while (n > 0);

				logger::log("parsing..." + logger::itos(n));

				mesg[n] = 0;
			}
			*/
		}
}

int memory_dispatcher::recv_2(int fd, char *buffer, int len, int flags, int to) {

	fd_set readset, tempset;
	int result, iof = -1;
	struct timeval tv;

	// Initialize the set
	FD_ZERO(&readset);
	FD_SET(fd, &readset);

	// Initialize time out struct
	tv.tv_sec = 0;
	tv.tv_usec = to * 1000;
	// select()
	result = select(fd + 1, &readset, NULL, NULL, NULL);

	// Check status
	if (result < 0)
		return -1;
	else if (result > 0 && FD_ISSET(fd, &readset)) {
		// Set non-blocking mode
		if ((iof = fcntl(fd, F_GETFL, 0)) != -1)
			fcntl(fd, F_SETFL, iof | O_NONBLOCK);
		// receive
		result = recv(fd, buffer, len, flags);
		// set as before
		if (iof != -1)
			fcntl(fd, F_SETFL, iof);
		return result;
	}
	return -2;
}

void memory_dispatcher::send_data(int sockfd, byte* data, int len) {
	//NOTOK
	char sub_mesg[INPUT_BUFFER];

	int max = len;
	int current = 0;
	int finish = 0;
	for (current = 0; current < max; current += INPUT_BUFFER) {
		finish = current + INPUT_BUFFER;
		if (finish > max) {

			finish = max;
		}

		finish -= current;

		bzero(sub_mesg, INPUT_BUFFER);
		memcpy(sub_mesg, &data[current], finish);
		send(sockfd, sub_mesg, finish, 0);
	}
	close(sockfd);
}

void memory_dispatcher::userHandler(int sockfd)
{
	int n;
	char mesg[INPUT_BUFFER];// = new char[INPUT_BUFFER];

	string message = "";

	while (true) {
		n = recv(sockfd, mesg, INPUT_BUFFER, MSG_PEEK | MSG_DONTWAIT);
		if (n < 1)
			break;

		memset(mesg, '\0', INPUT_BUFFER);
		n = memory_dispatcher::recv_2(sockfd, mesg, INPUT_BUFFER, 0, 5000);
		message += string(mesg, n);

		if (n < 1)
			break;

	}
	char* msg = (char*)message.c_str();
	char* t_ident = msg + 1;
	if (msg[0] == 'G')
	{
		logger::log("GET action for: " + helpers::chtos(t_ident));
		memory_entry temp = main_memory.get(t_ident);
		if (temp.data == NULL)
		{
			int m = send(sockfd, "NODATA", 7, 0);
		}
		else
		{
			memory_dispatcher::send_data(sockfd, temp.data,temp.length);
			//NOTOK
			/*
			int max = temp.length;
			int current = 0;
			int finish = 0;
			for (current = 0; current < max; current += INPUT_BUFFER) {
				finish = current + INPUT_BUFFER;
				if (finish > max) {

					finish = max ;
				}

				finish -= current;

				bzero(sub_mesg, INPUT_BUFFER);
				memcpy(sub_mesg, &temp.data[current], finish);
				send(sockfd, sub_mesg, finish, 0);
			}
			close(sockfd);
			*/
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

		string response;
		int len = 0;
		switch (operation) {
		case 'O':
				logger::log("FIND_OR action...");
				response = main_memory.find_or(query, subset);
				len = response.size();
				logger::log("...finished (" + logger::itos(len) + ")");
			break;
		case 'A':
				logger::log("FIND_AND action...");
				response = main_memory.find_and(query, subset);
				len = response.size();
				logger::log("...finished (" + logger::itos(len) + ")");
			break;
		}

		int m = -1;
		if (len > 0) {
			//NOTOK
			memory_dispatcher::send_data(sockfd, (unsigned char*)response.c_str() + 1, len);
			//m = send(sockfd, response.c_str() + 1, len, 0);
			int tmp = errno;
			if (m < 0){
				fprintf(stderr, "socket: %s , errno %d\n", strerror(tmp), tmp);
			}
		}
		else {
			//OK
			m = send(sockfd, "NODATA", 7, 0);
		}
		logger::log("...sent (" + logger::itos(m) + ")");


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
		main_memory.add(ident, (char*)metadata, (byte*)(msg + where + 2), message.size() - where - 2);
		//OK
		send(sockfd, "SAVED", 6, 0);
	}
	else if (msg[0] == 'R'){
		main_memory.remove(t_ident);
		logger::log("DEL action for: " + helpers::chtos(t_ident));

		//OK
		send(sockfd, "REMVD", 6, 0);
	}

	logger::log("Thread dead");
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