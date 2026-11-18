// Shell.h - command-line interface to the kernel
#ifndef SHELL_H
#define SHELL_H

#include <string>
#include "Kernel.h"

class Shell {
private:
    Kernel& kernel;          // ASSOCIATION: shell uses a kernel it does not own
    bool running;
    void printHelp() const;
    void execute(const std::string& line);

public:
    explicit Shell(Kernel& k);
    void start();
};

#endif
