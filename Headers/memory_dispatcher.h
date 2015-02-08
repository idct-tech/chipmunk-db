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
#include <unistd.h>
#include <fcntl.h>
#include "logger.h"
#define GetCurrentDir getcwd
#define INPUT_BUFFER 1024

using namespace std;

class memory_dispatcher {

	public:
		static void saverFunc();
		static void load();
		static int recv_2(int fd, char *buffer, int len, int flags, int to);
		static void send_data(int fd, byte* entry, int len);
		static void userHandler(int sockfd);
		static void listenerFunc(string a);

		static memory_bank main_memory;
};