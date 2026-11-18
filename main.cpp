// main.cpp - entry point
#include "Kernel.h"
#include "Shell.h"

int main() {
    Kernel kernel;          // constructor "boots" the kernel
    Shell shell(kernel);
    shell.start();
    return 0;               // kernel destructor runs here automatically
}
