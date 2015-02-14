#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <boost/thread.hpp>
#include "../Headers/memory_dispatcher.h"
#include "../Headers/memory_bank.h"
#include "../Headers/logger.h"

int main(int argc, char *argv[]) {

	logger::log(" ! Started...");
	memory_dispatcher::load();

	if (argc > 0) {
		int port = atoi(argv[1]);
		memory_dispatcher::port = port;
	}

	boost::thread workerThread(&memory_dispatcher::saverFunc);
	boost::thread listenerThread(&memory_dispatcher::listenerFunc, "started");


	listenerThread.join();
	//workerThread.join();

	return 0;
}