// Kernel.h - the core that owns processes, memory and the scheduler
#ifndef KERNEL_H
#define KERNEL_H

#include <vector>
#include <string>
#include "Process.h"
#include "MemoryManager.h"
#include "Scheduler.h"

class Kernel {
private:
    MemoryManager memory;                 // COMPOSITION: Kernel HAS-A MemoryManager
    Scheduler* scheduler;                 // POLYMORPHISM: any scheduler type
    std::vector<Process*> processTable;   // all processes

    Process* findProcess(int pid);
    void terminate(Process* p);           // frees memory, marks TERMINATED

public:
    Kernel();
    ~Kernel();
    Kernel(const Kernel&) = delete;            // a kernel must not be copied
    Kernel& operator=(const Kernel&) = delete;

    bool createProcess(const std::string& name, int burst, int memKB, bool isSystem);
    bool killProcess(int pid);
    void setScheduler(Scheduler* s);
    void runScheduler();
    void listProcesses() const;
    void showMemory() const;
    void cleanup();                       // delete TERMINATED processes
};

#endif
