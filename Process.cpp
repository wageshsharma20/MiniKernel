// Process.cpp
#include "Process.h"
#include <iomanip>

int Process::nextPid = 1;   // static member must be defined once

std::string stateToString(State s) {
    switch (s) {
        case State::READY:      return "READY";
        case State::RUNNING:    return "RUNNING";
        case State::TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

Process::Process(const std::string& n, int burst, int memKB)
    : pid(nextPid++), name(n), burstTime(burst), remainingTime(burst),
      memoryKB(memKB), state(State::READY) {
    std::cout << "  [ctor] Process " << pid << " (" << name << ") created\n";
}

Process::~Process() {
    std::cout << "  [dtor] Process " << pid << " (" << name << ") destroyed\n";
}

int Process::getPid() const           { return pid; }
std::string Process::getName() const  { return name; }
int Process::getBurstTime() const     { return burstTime; }
int Process::getRemainingTime() const { return remainingTime; }
int Process::getMemoryKB() const      { return memoryKB; }
State Process::getState() const       { return state; }
void Process::setState(State s)       { state = s; }
bool Process::isFinished() const      { return remainingTime == 0; }

int Process::execute(int timeSlice) {
    int used = (remainingTime < timeSlice) ? remainingTime : timeSlice;
    remainingTime -= used;
    return used;
}

void Process::display() const {
    std::cout << std::left
              << std::setw(5)  << pid
              << std::setw(12) << name
              << std::setw(8)  << getType()      // virtual call
              << std::setw(12) << stateToString(state)
              << std::setw(7)  << burstTime
              << std::setw(7)  << remainingTime
              << std::setw(8)  << memoryKB;
}

std::ostream& operator<<(std::ostream& out, const Process& p) {
    out << "P" << p.pid << "(" << p.name << ")";   // friend can read private data
    return out;
}

// ---- UserProcess ----
UserProcess::UserProcess(const std::string& n, int burst, int memKB)
    : Process(n, burst, memKB) {}               // calls base constructor
std::string UserProcess::getType() const { return "USER"; }
bool UserProcess::canBeKilled() const    { return true; }

// ---- SystemProcess ----
SystemProcess::SystemProcess(const std::string& n, int burst, int memKB)
    : Process(n, burst, memKB) {}
std::string SystemProcess::getType() const { return "SYSTEM"; }
bool SystemProcess::canBeKilled() const    { return false; }
void SystemProcess::display() const {
    Process::display();                         // reuse base version
    std::cout << "[protected]";
}
