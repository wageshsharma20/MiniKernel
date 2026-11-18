// Process.h - Process Control Block (PCB) hierarchy
#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <iostream>

// The three states a process can be in
enum class State { READY, RUNNING, TERMINATED };

std::string stateToString(State s);

// ---------------- Abstract base class ----------------
class Process {
private:                      // ENCAPSULATION: data is hidden
    int pid;
    std::string name;
    int burstTime;            // total CPU time needed
    int remainingTime;        // CPU time still left
    int memoryKB;             // memory requested
    State state;

    static int nextPid;       // STATIC data member shared by all objects

public:
    Process(const std::string& name, int burst, int memKB);   // CONSTRUCTOR
    virtual ~Process();                                       // VIRTUAL DESTRUCTOR

    // Getters (read-only access to private data)
    int getPid() const;
    std::string getName() const;
    int getBurstTime() const;
    int getRemainingTime() const;
    int getMemoryKB() const;
    State getState() const;

    void setState(State s);
    int execute(int timeSlice);      // run for up to timeSlice units
    bool isFinished() const;

    // ABSTRACTION + POLYMORPHISM
    virtual std::string getType() const = 0;   // PURE VIRTUAL -> abstract class
    virtual bool canBeKilled() const = 0;
    virtual void display() const;              // can be overridden

    // OPERATOR OVERLOADING via a FRIEND function
    friend std::ostream& operator<<(std::ostream& out, const Process& p);
};

// ---------------- Derived classes (INHERITANCE) ----------------
class UserProcess : public Process {
public:
    UserProcess(const std::string& name, int burst, int memKB);
    std::string getType() const override;   // FUNCTION OVERRIDING
    bool canBeKilled() const override;
};

class SystemProcess : public Process {
public:
    SystemProcess(const std::string& name, int burst, int memKB);
    std::string getType() const override;
    bool canBeKilled() const override;      // system processes are protected
    void display() const override;          // adds a "[protected]" tag
};

#endif
