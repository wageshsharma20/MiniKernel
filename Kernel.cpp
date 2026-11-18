// Kernel.cpp
#include "Kernel.h"
#include <iostream>
#include <iomanip>

Kernel::Kernel() : scheduler(new RoundRobinScheduler(2)) {
    std::cout << "  [ctor] Kernel booted. Default scheduler: "
              << scheduler->getName() << "\n";
}

Kernel::~Kernel() {
    std::cout << "Kernel shutting down...\n";
    for (Process* p : processTable) delete p;   // virtual destructor called
    delete scheduler;
}

Process* Kernel::findProcess(int pid) {
    for (Process* p : processTable)
        if (p->getPid() == pid) return p;
    return nullptr;
}

void Kernel::terminate(Process* p) {
    p->setState(State::TERMINATED);
    memory.deallocate(p->getPid());
}

bool Kernel::createProcess(const std::string& name, int burst, int memKB, bool isSystem) {
    if (burst <= 0 || memKB <= 0) {
        std::cout << "Error: burst time and memory must be positive.\n";
        return false;
    }
    if (memory.blocksNeeded(memKB) > memory.freeBlocks()) {
        std::cout << "Error: not enough memory for '" << name << "' ("
                  << memKB << " KB requested).\n";
        return false;
    }
    Process* p;                                   // base-class pointer
    if (isSystem) p = new SystemProcess(name, burst, memKB);
    else          p = new UserProcess(name, burst, memKB);

    memory.allocate(p->getPid(), memKB);
    processTable.push_back(p);
    std::cout << "Created " << p->getType() << " process " << *p
              << " with " << memKB << " KB.\n";
    return true;
}

bool Kernel::killProcess(int pid) {
    Process* p = findProcess(pid);
    if (p == nullptr) {
        std::cout << "Error: no process with PID " << pid << ".\n";
        return false;
    }
    if (p->getState() == State::TERMINATED) {
        std::cout << "Process " << *p << " is already terminated.\n";
        return false;
    }
    if (!p->canBeKilled()) {                      // polymorphic decision
        std::cout << "Permission denied: " << *p << " is a SYSTEM process.\n";
        return false;
    }
    terminate(p);
    std::cout << "Killed " << *p << ". Memory released.\n";
    return true;
}

void Kernel::setScheduler(Scheduler* s) {
    delete scheduler;                             // free the old policy
    scheduler = s;
    std::cout << "Scheduler set to " << scheduler->getName() << ".\n";
}

void Kernel::runScheduler() {
    std::vector<Process*> ready;
    for (Process* p : processTable)
        if (p->getState() == State::READY) ready.push_back(p);

    if (ready.empty()) {
        std::cout << "No READY processes to run.\n";
        return;
    }
    std::cout << "Running " << ready.size() << " process(es) using "
              << scheduler->getName() << ":\n";
    scheduler->run(ready);                        // virtual call

    for (Process* p : ready) terminate(p);        // release their memory
    std::cout << "All processes finished. Memory released.\n";
}

void Kernel::listProcesses() const {
    if (processTable.empty()) {
        std::cout << "Process table is empty.\n";
        return;
    }
    std::cout << std::left << std::setw(5) << "PID" << std::setw(12) << "NAME"
              << std::setw(8) << "TYPE" << std::setw(12) << "STATE"
              << std::setw(7) << "BURST" << std::setw(7) << "LEFT"
              << std::setw(8) << "MEM(KB)" << "\n";
    for (Process* p : processTable) {
        p->display();                             // virtual: may be overridden
        std::cout << "\n";
    }
}

void Kernel::showMemory() const { memory.display(); }

void Kernel::cleanup() {
    int removed = 0;
    for (size_t i = 0; i < processTable.size(); ) {
        if (processTable[i]->getState() == State::TERMINATED) {
            delete processTable[i];
            processTable.erase(processTable.begin() + i);
            removed++;
        } else {
            i++;
        }
    }
    std::cout << "Removed " << removed << " terminated process(es).\n";
}
