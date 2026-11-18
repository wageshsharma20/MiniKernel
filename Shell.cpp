// Shell.cpp
#include "Shell.h"
#include <iostream>
#include <sstream>

Shell::Shell(Kernel& k) : kernel(k), running(true) {}

void Shell::printHelp() const {
    std::cout <<
        "Commands:\n"
        "  create <name> <burst> <memKB> [system]  create a process\n"
        "  ps                                      list all processes\n"
        "  kill <pid>                              terminate a process\n"
        "  sched fcfs | sched rr <quantum>         choose scheduler\n"
        "  run                                     run all READY processes\n"
        "  mem                                     show memory map\n"
        "  clean                                   remove TERMINATED processes\n"
        "  help                                    show this help\n"
        "  exit                                    shut down\n";
}

void Shell::start() {
    std::cout << "\n=== Mini Kernel Simulator === (type 'help')\n";
    std::string line;
    while (running) {
        std::cout << "\nkernel> ";
        if (!std::getline(std::cin, line)) break;   // end of input
        execute(line);
    }
}

void Shell::execute(const std::string& line) {
    std::stringstream ss(line);
    std::string cmd;
    if (!(ss >> cmd)) return;                        // empty line

    if (cmd == "help") {
        printHelp();
    } else if (cmd == "create") {
        std::string name, type;
        int burst, mem;
        if (!(ss >> name >> burst >> mem)) {
            std::cout << "Usage: create <name> <burst> <memKB> [system]\n";
            return;
        }
        ss >> type;
        kernel.createProcess(name, burst, mem, type == "system");
    } else if (cmd == "ps") {
        kernel.listProcesses();
    } else if (cmd == "kill") {
        int pid;
        if (ss >> pid) kernel.killProcess(pid);
        else std::cout << "Usage: kill <pid>\n";
    } else if (cmd == "sched") {
        std::string type;
        ss >> type;
        if (type == "fcfs") {
            kernel.setScheduler(new FCFSScheduler());
        } else if (type == "rr") {
            int q;
            if (ss >> q && q > 0) kernel.setScheduler(new RoundRobinScheduler(q));
            else std::cout << "Usage: sched rr <quantum>  (quantum > 0)\n";
        } else {
            std::cout << "Usage: sched fcfs | sched rr <quantum>\n";
        }
    } else if (cmd == "run") {
        kernel.runScheduler();
    } else if (cmd == "mem") {
        kernel.showMemory();
    } else if (cmd == "clean") {
        kernel.cleanup();
    } else if (cmd == "exit") {
        running = false;
    } else {
        std::cout << "Unknown command '" << cmd << "'. Type 'help'.\n";
    }
}
