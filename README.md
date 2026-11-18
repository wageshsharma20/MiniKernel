# Mini Kernel Simulator: OOP Project Report (C++)

## 1. Project Overview

**Title:** Mini Kernel Simulator: An Object-Oriented Simulation of Process, CPU and Memory Management

**Problem statement:** Operating system ideas such as processes, scheduling and memory allocation are hard to picture because the real kernel hides them. This project builds a small console program that copies these kernel jobs, so a student can watch processes being created, scheduled and terminated, and watch memory being given out and released.

**Objectives**
1. Model the main kernel parts (process, memory manager, scheduler, kernel, shell) as C++ classes.
2. Show encapsulation, abstraction, inheritance, polymorphism, constructors and destructors, overriding, composition, friend functions, operator overloading and static members in a real example.
3. Use the FCFS and Round Robin scheduling algorithms and calculate turnaround and waiting time.
4. Provide a simple command-line interface.

**Scope:** The program is a single-user console simulation. All processes arrive at time 0. Time is counted in abstract "units". Memory is 1024 KB, split into 16 blocks of 64 KB. Out of scope: real hardware, booting, interrupts, threads, files and I/O devices.

**Motivation:** The project connects two subjects, OOP and Operating Systems. Each kernel part has its own job, so it fits naturally as a class.

## 2. OOP Design

| Class | Responsibility | OOP concepts shown |
|---|---|---|
| `Process` (abstract) | Holds the PCB data: pid, name, burst, remaining time, memory and state. Runs for one time slice. | Encapsulation (private data with getters), abstraction (pure virtual `getType`, `canBeKilled`), static member `nextPid`, constructor and **virtual destructor**, friend `operator<<` |
| `UserProcess` | A normal application process that can be killed | Inheritance, function overriding |
| `SystemProcess` | A kernel-level process that **cannot be killed** and shows `[protected]` | Inheritance, overriding `display()` while also calling `Process::display()` |
| `MemoryManager` | Allocates and frees 64 KB blocks; keeps a map of which block belongs to which PID | Encapsulation (the `blocks[]` array is hidden), static const members, constructor and destructor |
| `Scheduler` (abstract) | The common interface `run()` plus the shared statistics helper | Abstraction, a protected helper method |
| `FCFSScheduler`, `RoundRobinScheduler` | The two scheduling policies | Inheritance, runtime polymorphism, parameterised constructor (quantum) |
| `Kernel` | Owns the process table, memory and scheduler. Creates, kills, runs and cleans up processes. | **Composition** (has a `MemoryManager`), polymorphism (`Scheduler*`), destructor that frees all memory, copy disabled |
| `Shell` | Reads commands and calls Kernel functions | **Association** (holds a `Kernel&` but does not own it) |

**Key design decisions**
- **Scheduler as a pointer to an abstract class.** The Kernel can switch policy at runtime (`sched fcfs` / `sched rr 3`) without changing its own code. This is the *Strategy pattern* and is the clearest example of polymorphism in the project.
- **`canBeKilled()` is virtual.** The Kernel never checks "is it a system process?" with if/else on types. It asks the object, and the object answers through its own override.
- **Memory is checked before the process is created**, so a failed request never leaves a half-built process.
- **Terminated processes stay in the table** so `ps` can show the TERMINATED state. `clean` deletes them, and you can see the destructor messages.
- The Kernel is the only owner of the processes and the scheduler. It deletes them in its destructor, so there are no memory leaks.

## 3. Architecture

### Class diagram
```
                 +-----------+   uses (reference)   +----------------------------+
                 |   Shell   |--------------------->|           Kernel           |
                 +-----------+                      |----------------------------|
                                                    | - memory : MemoryManager   |
                                                    | - scheduler : Scheduler*   |
                                                    | - processTable: Process*[] |
                                                    +-------------+--------------+
                          owns 0..*                               | owns 1
          +---------------------------------+          +---------+-----------+
          |       <<abstract>> Process      |          | <<abstract>>        |
          | - pid, name, burst, remaining   |          |   Scheduler         |
          | - memoryKB, state               |          | + run(list) = 0     |
          | + getType() = 0                 |          +---------+-----------+
          | + canBeKilled() = 0             |                ^           ^
          | + display() (virtual)           |                |           |
          +---------------------------------+        FCFSScheduler  RoundRobinScheduler
                 ^                  ^                                 - quantum
                 |                  |
          UserProcess        SystemProcess
                             (overrides display)

   Kernel <>-- MemoryManager (composition: Kernel contains it)
   MemoryManager: - blocks[16] ; + allocate(pid,kb) ; + deallocate(pid) ; + display()
```

### Workflow
```
main() -> Kernel constructor (boots and creates MemoryManager + default RR scheduler)
       -> Shell.start() loop: read line -> parse -> call Kernel method
            create -> check memory -> new User/SystemProcess -> allocate blocks -> READY
            kill   -> canBeKilled()? -> TERMINATED + free memory
            run    -> collect READY processes -> scheduler->run() (virtual)
                      READY -> RUNNING -> READY ... -> TERMINATED -> free memory
            clean  -> delete TERMINATED objects (destructors run)
       -> exit -> Kernel destructor deletes everything
```

### Process state diagram
```
  create --> READY --(scheduled)--> RUNNING --(burst done / kill)--> TERMINATED
               ^                       |
               +---(quantum expired)---+   (Round Robin only)
```

## 4. Source Files
| File | Contents |
|---|---|
| `Process.h/.cpp` | `Process`, `UserProcess`, `SystemProcess`, `State` enum |
| `MemoryManager.h/.cpp` | Block-based memory allocator |
| `Scheduler.h/.cpp` | `Scheduler`, `FCFSScheduler`, `RoundRobinScheduler` |
| `Kernel.h/.cpp` | The kernel core |
| `Shell.h/.cpp` | The command-line interface |
| `main.cpp` | Entry point (8 lines) |
| `Makefile` | Build script |
| `tests/demo_input.txt` | Scripted demo and test input |
| `tests/expected_output.txt` | The output that the input above should produce |

## 5. How to Use

| Command | Meaning |
|---|---|
| `create <name> <burst> <memKB> [system]` | Create a process (add `system` to make it protected) |
| `ps` | Show the process table |
| `kill <pid>` | Terminate a user process |
| `sched fcfs` / `sched rr <q>` | Choose a scheduler (default: RR with q=2) |
| `run` | Run all READY processes until they finish |
| `mem` | Show the memory map |
| `clean` | Remove terminated processes |
| `help`, `exit` | Show help, or shut down |

### Sample session (Round Robin, q = 2)
```
kernel> create A 6 64
Created USER process P1(A) with 64 KB.
kernel> create B 3 64
kernel> create C 2 64
kernel> run
  t=0-2 : P1(A) runs (remaining 4)
  t=2-4 : P2(B) runs (remaining 1)
  t=4-6 : P3(C) runs and finishes
  t=6-8 : P1(A) runs (remaining 2)
  t=8-9 : P2(B) runs and finishes
  t=9-11 : P1(A) runs and finishes
  Process       Burst   Turnaround  Waiting
  A             6       11          5
  B             3       9           6
  C             2       6           4
  Average turnaround = 8.67, average waiting = 5.00
```
With `sched fcfs`, the same three processes give A 0-6, B 6-9 and C 9-11, with an average waiting time of 5.00.

## 6. Compile and Run
Any C++11 compiler works (g++, clang++, or MinGW on Windows).
```bash
g++ -std=c++11 -Wall main.cpp Process.cpp MemoryManager.cpp Scheduler.cpp Kernel.cpp Shell.cpp -o minikernel
./minikernel
```
Or run `make`, then `./minikernel`. On Windows, run `minikernel.exe`. In Code::Blocks or Dev-C++, add all the `.cpp` files to one project.

**Scripted demo (useful in the viva):** `./minikernel < tests/demo_input.txt`

**Live demo order:** `help`, then create 2 user processes and 1 system process, then `ps`, `mem`, `kill` on the system process (denied), `kill` on a user process, `mem` (memory freed), `run`, `ps`, `clean` (shows the destructors), then switch to `sched fcfs` and compare the averages. Finish with `exit` (shows the destructors).

## 7. Test Cases
| # | Input | Expected result |
|---|---|---|
| 1 | `create editor 5 128` | P1 is created, 2 blocks are used |
| 2 | `create big 10 2000` | "not enough memory" (more than 1024 KB) |
| 3 | `create bad -1 64` | "must be positive" |
| 4 | `create x` | Usage message |
| 5 | `kill 99` | "no process with PID 99" |
| 6 | `kill <system pid>` | "Permission denied" |
| 7 | `kill <pid>` twice | The second time: "already terminated" |
| 8 | `run` with no READY processes | "No READY processes to run" |
| 9 | FCFS: A6, B3, C2 | Waiting 0, 6, 9; average 5.00 |
| 10 | RR q=2: A6, B3, C2 | Finish 11, 9, 6; waiting 5, 6, 4 |
| 11 | `sched rr 0` | Usage message (quantum must be > 0) |
| 12 | `foo` | "Unknown command" |
| 13 | Fill memory, `kill` one, create again | Succeeds because the blocks are reused |

**Common errors**
- *Undefined reference*: one of the `.cpp` files is missing from the compile command.
- *`override`/`enum class` errors*: add `-std=c++11`.
- A missing virtual destructor would mean the derived destructors never run, so it is kept on purpose.
- Typing a letter where a number is expected: the shell shows the usage message instead of crashing.

## 8. Viva Preparation

### Explaining the code simply
- A **Process** is like a file card (a PCB) that holds an ID, a name, the time needed and the memory needed. Its data is private, so only its own methods can change it.
- The **MemoryManager** is a row of 16 boxes. Each box holds the PID of its owner, or 0 if it is free.
- A **Scheduler** decides who uses the CPU next. FCFS lets each process finish before the next one starts. Round Robin gives each process `q` units and then sends it to the back of the queue.
- The **Kernel** is the manager. It owns everything and connects the parts together.
- The **Shell** is the receptionist. It reads your typing and passes the request to the kernel.

### Where each OOP concept is used
| Concept | Exact location |
|---|---|
| Class and object | Every class; `Kernel kernel;` in `main` |
| Encapsulation | Private fields in `Process` and `blocks[]` in `MemoryManager` |
| Abstraction | `Process` and `Scheduler` have pure virtual functions |
| Inheritance | `UserProcess`/`SystemProcess` from `Process`; FCFS/RR from `Scheduler` |
| Runtime polymorphism | `scheduler->run()`, `p->canBeKilled()`, `p->display()` |
| Overriding | `SystemProcess::display()` calls the base version and then adds more |
| Constructor / destructor | All the `[ctor]`/`[dtor]` messages; `Process` uses an initializer list |
| Virtual destructor | `Process::~Process()`, so `delete basePtr` calls the right destructor |
| Composition | `Kernel` contains `MemoryManager memory;` |
| Association | `Shell` holds `Kernel&` |
| Static member | `Process::nextPid` gives each process a unique PID |
| Friend + operator overloading | `operator<<` prints `P1(editor)` |
| Const member functions | All the getters and `display() const` |

### Likely viva questions
1. **Why is this a simulation and not a real kernel?** It runs as an ordinary user program on top of Windows, Linux or macOS. It does not control real hardware, has no bootloader, no interrupts, no CPU privilege modes, and no real memory addresses. "Time" is just an integer counter. It copies the *logic* of kernel jobs so you can study them.
2. **Why make `Process` abstract?** A "generic process" should never exist. Every process is either a user process or a system process, and the pure virtual functions force each derived class to define its own behaviour.
3. **Why is the destructor virtual?** We delete objects through a `Process*` pointer. Without `virtual`, only the base destructor would run, which is undefined behaviour.
4. **What is the difference between composition and association here?** Kernel *owns* the MemoryManager, so it is created and destroyed together with the Kernel. Shell only *refers to* the Kernel, which lives on after the Shell.
5. **How does polymorphism help with the scheduler?** The Kernel calls `scheduler->run()` without knowing which algorithm it is. A new algorithm such as SJF can be added as a new class, and the Kernel does not change (the open/closed principle).
6. **Overloading vs overriding?** Overloading means the same function name with different parameters, decided at compile time (for example `operator<<`). Overriding means a derived class redefines a virtual function, decided at runtime.
7. **How is the PID unique?** The static `nextPid` is shared by all objects and is incremented in the constructor.
8. **FCFS vs Round Robin?** FCFS is simple but short jobs wait behind long ones (the convoy effect). RR is fair and responsive, but has more context switches. If the quantum is very large, RR behaves like FCFS.
9. **How are turnaround and waiting time calculated?** Turnaround = completion − arrival (arrival is 0 here). Waiting = turnaround − burst.
10. **What happens when memory is full?** `createProcess` checks `freeBlocks()` first and refuses the request. Killing or finishing a process frees its blocks for reuse.
11. **What is internal fragmentation in your memory manager?** A request for 100 KB gets 2 blocks (128 KB), so 28 KB inside the last block is wasted.
12. **Why are the getters `const`?** They promise not to change the object, and they can be called on const objects.
13. **Why delete the copy constructor of Kernel?** Copying would copy raw pointers, and then two kernels would delete the same processes (a double free).
14. **Can it be extended?** Yes. Possible additions are a priority or SJF scheduler, arrival times, a BLOCKED/WAITING state, or templates for a generic queue.

### Honest limitations to mention
All processes arrive at time 0. `run` executes processes until they finish, not tick by tick. There is no I/O or BLOCKED state. Memory blocks do not need to be contiguous.
