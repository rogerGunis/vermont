/*
 * Vermont PCAP Exporter
 * Copyright (C) 2010 Vermont Project
 * Copyright (C) 2010 Thomas Hauenstein <sithhaue@users.berlios.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "PCAPExporterPipe.h"

#include "modules/packet/Packet.h"
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <cstring>
#include <boost/filesystem/operations.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <sys/wait.h>
#include <common/SignalHandler.h>

namespace bfs = boost::filesystem;

PCAPExporterPipe::PCAPExporterPipe(const std::string& logfile)
	: logFileName(logfile),
	  fifoReaderCmd(""),
	  workingPath(""),
	  onRestart(false),
	  appenddate(false),
	  restartOnSignal(false),
	  fifoReaderPid(0),
	  dummy(NULL),
	  sigKillTimeout(1),
	  counter(0),
	  last_check(0)
{
}

PCAPExporterPipe::~PCAPExporterPipe()
{
}

void PCAPExporterPipe::setPipeReaderCmd(const std::string& cmd)
{
	fifoReaderCmd = cmd;
}

void PCAPExporterPipe::setWorkingPath(const std::string& path)
{
	workingPath = path;
}

void PCAPExporterPipe::setAppendDate(bool value)
{
	appenddate = value;
}

void PCAPExporterPipe::setSigKillTimeout(int s)
{
	if (s < 0) s *= -1;
	sigKillTimeout = s;
}

void PCAPExporterPipe::setRestartOnSignal(bool b){
	restartOnSignal = b;
}

int PCAPExporterPipe::execCmd(std::string& cmd)
{
	//char *command[] = {"tcpdump","-nr", "-" , (char*)0}; //"-w", "/tmp/pcap.dump",(char*)0};
	char *command[64];
	char tmp[1024];
	if (strlen(cmd.c_str()) > 1023) {
		THROWEXCEPTION("Command too long");
	}
	strcpy(tmp, cmd.c_str());
	command[0] = strtok(tmp, " \t");
	int i = 0;
	//TODO:  handle arguments containg whitespaces
	while (command[i]) {
		i++;
		if (i == 63) {
			THROWEXCEPTION("Argument list too long");
		}
		command[i] = strtok(NULL, " \t");
	}
	command[++i] = (char*)0;
	if (pipe(child_parent_pipe)) {
		THROWEXCEPTION("pipe(child_parent_pipe) failed");
	}
	/* Create a pipe, which allows communication between the child and the parent.
	 * Writing an int value (e.g. errno) into child_parent_pipe[1]
	 * will cause an exception in the parent process.
	 * Throwing exceptions in the child *will not* terminate Vermont!
	 */
	for (int i=0; i<2; i++) {
		int oldflags = fcntl (child_parent_pipe[i], F_GETFD, 0);
		if(-1 == oldflags)
			THROWEXCEPTION("fctnl: %s", strerror(errno));
		oldflags |= FD_CLOEXEC;
		if (-1 == fcntl (child_parent_pipe[i], F_SETFD, oldflags))
			THROWEXCEPTION("fctnl: %s", strerror(errno));
	}

	int pid = fork();
	if (pid == 0) {
		if (dup2(fd[0], STDIN_FILENO) == -1) {
			if (write(child_parent_pipe[1], &errno, sizeof(int)) != sizeof(int))
				THROWEXCEPTION("dup(fd[0]) failed");
				exit(1);
		}
		if (workingPath != "") {
			int res = chdir(workingPath.c_str());
			if (res != 0) {
				THROWEXCEPTION("failed to change to working path '%s'", workingPath.c_str());
				exit(1);
			}
		}
		if (logFileName != "") {
			std::string logfile = logFileName;
			if(appenddate){
				time_t rawtime;
				struct tm * timeinfo;
				char buffer [20];
				time (&rawtime);
				timeinfo = localtime (&rawtime);
				if (strftime(buffer, 20, "%Y-%m-%d_%H:%M:%S", timeinfo) == 0) {
					if (write(child_parent_pipe[1], &errno, sizeof(int)) != sizeof(int))
						THROWEXCEPTION("strftime & write failed");
					exit(1);
				}
				logfile = logFileName + "_" + buffer + ".log";
			}

			if (freopen (logfile.c_str(),"a",stdout) == NULL) {
				if (write(child_parent_pipe[1], &errno, sizeof(int)) != sizeof(int))
					THROWEXCEPTION("freopen & write failed");
				exit(1);
			}
			if (dup2(STDOUT_FILENO, STDERR_FILENO) == -1) {
				if (write(child_parent_pipe[1], &errno, sizeof(int)) != sizeof(int))
					THROWEXCEPTION("dup2 & write failed");
				exit(1);
			}
			/*
			   if (setvbuf(stderr, NULL, _IONBF, 0)) {
			   THROWEXCEPTION("setvbuf failed");
			   }
			   if (setvbuf(stdout, NULL, _IONBF, 0)) {
			   THROWEXCEPTION("setvbuf failed");
			   }
			   */
		}
		close(child_parent_pipe[0]); // close read-end
		if (execvp(command[0], command)<0) {
			if (write(child_parent_pipe[1], &errno, sizeof(int)) != sizeof(int))
				THROWEXCEPTION("exec failed"); //throw the exception only if we couldn't
												//tell the parent that sth. went wrong...
			exit(1);
		}
	} else if (pid > 0) {
		close(child_parent_pipe[1]);
		int buf;
		if (read(child_parent_pipe[0], &buf, sizeof(int)) == sizeof(int)) { //The child actually wrote errno into the pipe
			THROWEXCEPTION("An error occurred in the child: %s", strerror(buf));
		}
		return pid;
	} else {
		THROWEXCEPTION("fork() failed");
	}
	return -1;
}

void PCAPExporterPipe::performStart()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	time(&last_check);
	if(last_check == (time_t) -1)
		THROWEXCEPTION("time() failed");

	SignalHandler::getInstance().registerSignalHandler(SIGCHLD, this);
	SignalHandler::getInstance().registerSignalHandler(SIGPIPE, this);
	if(restartOnSignal)
		SignalHandler::getInstance().registerSignalHandler(SIGUSR2, this);

	dummy = pcap_open_dead(link_type, snaplen);
	if (!dummy) {
		THROWEXCEPTION("Could not open dummy device: %s", errbuf);
	}

	startProcess();

	msg(MSG_INFO, "Started PCAPExporterPipe with the following parameters:");
	if (fifoReaderCmd != ""){
		msg(MSG_INFO, "  - fifoReaderCmd = %s", fifoReaderCmd.c_str());
		msg(MSG_INFO, "  - fifoReaderPid = %d", fifoReaderPid);
	} else {
		THROWEXCEPTION("No fifoReaderCmd specified!");
	}
	if (logFileName != ""){
		msg(MSG_INFO, "  - logfileBaseName = %s", logFileName.c_str());
		msg(MSG_INFO, "  - appenddate = %s", appenddate ? "true" : "false");
	}
	else
		msg(MSG_ERROR, "No Logfile specified - dumping to stdout!");
	msg(MSG_INFO, "  - sigKillTimeout = %d" , sigKillTimeout);
}
//void PCAPExporterPipe::postReconfiguration(){ msg(MSG_INFO, "postReconfiguration"); }
//void PCAPExporterPipe::onReconfiguration1(){ msg(MSG_INFO, "onReconfiguration1"); }
//void PCAPExporterPipe::onReconfiguration2(){ msg(MSG_INFO, "onReconfiguration1"); }
//void PCAPExporterPipe::preReconfiguration(){ msg(MSG_INFO, "preReconfiguration"); }

void PCAPExporterPipe::performShutdown()
{
	SignalHandler::getInstance().unregisterSignalHandler(SIGCHLD, this);
	SignalHandler::getInstance().unregisterSignalHandler(SIGPIPE, this);
	if(restartOnSignal)
		SignalHandler::getInstance().unregisterSignalHandler(SIGUSR2, this);
	msg(MSG_DEBUG, "Performing shutdown for PID %d", fifoReaderPid);
	sleep(1);
	if (dumper) {
		if (-1 == pcap_dump_flush(dumper)) {
			msg(MSG_FATAL, "PCAPExporterPipe: Could not flush dump file");
		}

		pcap_dump_close(dumper);
		if (fifoReaderPid != 0 ) {
			kill_all(fifoReaderPid);
		}
	}
}

void PCAPExporterPipe::startProcess()
{
	char errbuf[PCAP_ERRBUF_SIZE];
	close(fd[0]);
	close(fd[1]);

	if (pipe(fd)) {
		THROWEXCEPTION("pipe() command failed");
	}
	FILE *f = fdopen(fd[1], "w");
	if (! f) {
		THROWEXCEPTION("fdopen failed");
	}
	dumper = pcap_dump_fopen(dummy, f);
	if (!dumper) {
		THROWEXCEPTION("Could not open dump file: %s", errbuf);
	}
	fifoReaderPid = execCmd(fifoReaderCmd);
	msg(MSG_INFO, "Restarted process with fifoReaderCmd \'%s\' and fifoReaderPid = %d",
						fifoReaderCmd.c_str(), fifoReaderPid);
}

void PCAPExporterPipe::receive(Packet* packet)
{
	if (onRestart){
		 DPRINTF("Dropping incoming packet, as attached process is not ready");
		 return;
	}
	if(fifoReaderPid == 0){
		 msg(MSG_VDEBUG, "fifoReaderPid = 0...this might happen during reconfiguration");
		 return;
	}
	writePCAP(packet);
}
void PCAPExporterPipe::handleSigPipe(int sig)
{
	handleSigChld(SIGCHLD);
}
void PCAPExporterPipe::handleSigChld(int sig)
{
	if(onRestart || exitFlag || isRunning(fifoReaderPid)) return;
	onRestart = true;
	counter++;
	time_t tmp;
	time(&tmp);
	if(tmp== (time_t) -1)
		THROWEXCEPTION("time() failed");

	// reset the counter if the last restart was more than 5 seconds ago
	if((tmp -last_check) > 5) counter = 0;

	if(counter > 5 && (tmp - last_check ) < 5)
		THROWEXCEPTION("Too many restarts in a short time period. Maybe your commandline is erroneous");

	if(!isRunning(fifoReaderPid)){
		//waitpid(fifoReaderPid, NULL, 0);
		msg(MSG_ERROR, "Process of fifoReaderCmd \'%s\' with fifoReaderPid %d is not running!",
				fifoReaderCmd.c_str(), fifoReaderPid);
		startProcess();
	}

	time(&last_check);
	if(last_check == (time_t) -1)
		THROWEXCEPTION("time() failed");
	onRestart = false;
}

void PCAPExporterPipe::handleSigUsr2(int sig)
{
	if (! restartOnSignal) return;
	int pid = fifoReaderPid;
	fifoReaderPid = 0;
	kill_all(pid);
	handleSigChld(sig);
}

void PCAPExporterPipe::kill_pid(int pid)
{
	int i = sigKillTimeout;
	std::string path = "/proc/" + boost::lexical_cast<std::string>(pid);
	kill(pid, SIGTERM);
	while(i--){
		msg(MSG_INFO, "waiting for pid %d, but no longer than %d seconds...", pid, i+1);
		if (!isRunning(fifoReaderPid)) return;
		sleep(1);
	}
	kill(pid, SIGKILL);
	waitpid(fifoReaderPid, NULL, 0);
}

bool PCAPExporterPipe::isRunning(int pid)
{
	int status;
	int result = waitpid(pid, &status, WNOHANG);
	if(result == 0) return true;
	return false;
}

bool PCAPExporterPipe::checkint(const char *my_string)
{
	size_t stringlength = strlen(my_string);
	for (size_t j=0; j<stringlength; j++)
		if((int)my_string[j] < 0x30 || (int)my_string[j] > 0x39)
			return false;

	return true;
}

void PCAPExporterPipe::kill_all(int ppid)
{
	sleep(2); //give the process some time to finish its work
	bfs::path path("/proc/");
	if (!bfs::exists(path) || !bfs::is_directory(path)){
		THROWEXCEPTION("Directory /proc not found...");
	}

	std::string line;
	std::vector<int> my_pids;
	std::vector<int> my_ppids;
	my_ppids.push_back(ppid);
	bfs::directory_iterator dir_iterator = bfs::directory_iterator(path);
	bfs::directory_iterator end_itr;
	while (dir_iterator != end_itr) {
		if (bfs::is_directory(dir_iterator->status())  &&
				this->checkint(dir_iterator->leaf().c_str()))
		{
			std::string filename = dir_iterator->path().file_string() + "/stat";
			std::ifstream myfile(filename.c_str());
			if (myfile.is_open()) {
				getline (myfile,line);
				char *token = strtok (const_cast<char*>(line.c_str())," ");
				int count = 0;
				while (token != NULL) {
					if (count++ == 3) { // field of parent pid
						for (std::vector<int>::iterator it = my_ppids.begin(); it != my_ppids.end(); it++) {
							if (atoi(token) == *it) {
								msg(MSG_DEBUG, "Pid %s  is a child of %d", dir_iterator->leaf().c_str(), *it );
								my_pids.push_back(boost::lexical_cast<int>(dir_iterator->leaf()));
								my_ppids.push_back(boost::lexical_cast<int>(dir_iterator->leaf()));
								break;
							}
						}
					}
					token = strtok (NULL, " ");
				}
				myfile.close();
			}
		}
		dir_iterator++;
	}
	my_pids.push_back(ppid);
	for (std::vector<int>::iterator it = my_pids.begin(); it != my_pids.end(); it++) {
		msg(MSG_DEBUG, "Killing Pid %d", *it);
		kill_pid(*it);
	}
	return;
}


