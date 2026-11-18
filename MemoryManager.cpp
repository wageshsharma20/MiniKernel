// MemoryManager.cpp
#include "MemoryManager.h"
#include <iostream>

MemoryManager::MemoryManager() {
    for (int i = 0; i < TOTAL_BLOCKS; i++) blocks[i] = 0;
    std::cout << "  [ctor] Memory Manager ready: " << TOTAL_BLOCKS << " blocks x "
              << BLOCK_SIZE << " KB = " << TOTAL_BLOCKS * BLOCK_SIZE << " KB\n";
}

MemoryManager::~MemoryManager() {
    std::cout << "  [dtor] Memory Manager shut down\n";
}

int MemoryManager::blocksNeeded(int sizeKB) const {
    return (sizeKB + BLOCK_SIZE - 1) / BLOCK_SIZE;   // round up
}

int MemoryManager::freeBlocks() const {
    int count = 0;
    for (int i = 0; i < TOTAL_BLOCKS; i++)
        if (blocks[i] == 0) count++;
    return count;
}

bool MemoryManager::allocate(int pid, int sizeKB) {
    int need = blocksNeeded(sizeKB);
    if (need > freeBlocks()) return false;           // check first, then allocate
    for (int i = 0; i < TOTAL_BLOCKS && need > 0; i++) {
        if (blocks[i] == 0) { blocks[i] = pid; need--; }
    }
    return true;
}

void MemoryManager::deallocate(int pid) {
    for (int i = 0; i < TOTAL_BLOCKS; i++)
        if (blocks[i] == pid) blocks[i] = 0;
}

void MemoryManager::display() const {
    std::cout << "Memory map (each cell = " << BLOCK_SIZE << " KB, '.' = free):\n  ";
    for (int i = 0; i < TOTAL_BLOCKS; i++) {
        if (blocks[i] == 0) std::cout << "[ . ]";
        else std::cout << "[P" << blocks[i] << (blocks[i] < 10 ? " ]" : "]");
    }
    std::cout << "\n  Free: " << freeBlocks() * BLOCK_SIZE << " KB / "
              << TOTAL_BLOCKS * BLOCK_SIZE << " KB\n";
}
