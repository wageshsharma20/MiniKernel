// Scheduler.h - CPU scheduling policies
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "Process.h"

// Abstract base class: "what" a scheduler does, not "how"
class Scheduler {
public:
    virtual ~Scheduler() {}
    virtual std::string getName() const = 0;
    virtual void run(std::vector<Process*>& readyList) = 0;   // PURE VIRTUAL
protected:
    // shared helper used by every derived scheduler
    void printSummary(const std::vector<Process*>& procs,
                      const std::vector<int>& finishTime) const;
};

class FCFSScheduler : public Scheduler {
public:
    std::string getName() const override;
    void run(std::vector<Process*>& readyList) override;
};

class RoundRobinScheduler : public Scheduler {
private:
    int quantum;
public:
    explicit RoundRobinScheduler(int q);
    std::string getName() const override;
    void run(std::vector<Process*>& readyList) override;
};

#endif
