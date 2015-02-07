#pragma once
#include <iostream>
#include <boost/thread.hpp>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "journal_operation.h"
#include "journal_entry.h"
#include "memory_bank.h"
#include <map>
#define byte unsigned char
#include <dirent.h>
#include <unistd.h>
#include <sys/dir.h>
#include "helpers.h"
#include "logger.h"
#define GetCurrentDir getcwd
#define INPUT_BUFFER 104284

using namespace std;

class memory_dispatcher {

	public:
		static void saverFunc();
		static void load();
		static void userHandler(int sockfd, char* msg, sockaddr_in client, int len, int len2);
		static void listenerFunc(string a);

		static memory_bank main_memory;
};