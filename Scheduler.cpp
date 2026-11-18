// Scheduler.cpp
#include "Scheduler.h"
#include <iostream>
#include <iomanip>
#include <queue>

void Scheduler::printSummary(const std::vector<Process*>& procs,
                             const std::vector<int>& finishTime) const {
    // All processes are assumed to arrive at time 0.
    // Turnaround = finish time,  Waiting = turnaround - burst
    double totalWait = 0, totalTat = 0;
    std::cout << "\n  " << std::left << std::setw(14) << "Process"
              << std::setw(8) << "Burst" << std::setw(12) << "Turnaround"
              << "Waiting\n";
    for (size_t i = 0; i < procs.size(); i++) {
        int tat = finishTime[i];
        int wait = tat - procs[i]->getBurstTime();
        totalTat += tat;
        totalWait += wait;
        std::cout << "  " << std::setw(14) << procs[i]->getName()
                  << std::setw(8) << procs[i]->getBurstTime()
                  << std::setw(12) << tat << wait << "\n";
    }
    std::cout << std::fixed << std::setprecision(2)
              << "  Average turnaround = " << totalTat / procs.size()
              << ", average waiting = " << totalWait / procs.size() << "\n";
}

// ---------------- FCFS ----------------
std::string FCFSScheduler::getName() const { return "FCFS"; }

void FCFSScheduler::run(std::vector<Process*>& readyList) {
    int time = 0;
    std::vector<int> finish(readyList.size());
    for (size_t i = 0; i < readyList.size(); i++) {
        Process* p = readyList[i];
        p->setState(State::RUNNING);
        int start = time;
        time += p->execute(p->getRemainingTime());   // run to completion
        p->setState(State::TERMINATED);
        finish[i] = time;
        std::cout << "  t=" << start << "-" << time << " : " << *p
                  << " runs and finishes\n";
    }
    printSummary(readyList, finish);
}

// ---------------- Round Robin ----------------
RoundRobinScheduler::RoundRobinScheduler(int q) : quantum(q) {}

std::string RoundRobinScheduler::getName() const {
    return "Round Robin (quantum=" + std::to_string(quantum) + ")";
}

void RoundRobinScheduler::run(std::vector<Process*>& readyList) {
    std::queue<int> q;                       // stores indexes into readyList
    for (size_t i = 0; i < readyList.size(); i++) q.push(i);

    int time = 0;
    std::vector<int> finish(readyList.size());
    while (!q.empty()) {
        int idx = q.front();
        q.pop();
        Process* p = readyList[idx];
        p->setState(State::RUNNING);
        int start = time;
        time += p->execute(quantum);
        std::cout << "  t=" << start << "-" << time << " : " << *p << " runs";
        if (p->isFinished()) {
            p->setState(State::TERMINATED);
            finish[idx] = time;
            std::cout << " and finishes\n";
        } else {
            p->setState(State::READY);       // preempted, back of the queue
            q.push(idx);
            std::cout << " (remaining " << p->getRemainingTime() << ")\n";
        }
    }
    printSummary(readyList, finish);
}
