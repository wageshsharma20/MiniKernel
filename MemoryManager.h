// MemoryManager.h - simple fixed-block memory allocator
#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

class MemoryManager {
private:
    static const int BLOCK_SIZE = 64;     // KB per block
    static const int TOTAL_BLOCKS = 16;   // 16 x 64 KB = 1024 KB
    int blocks[TOTAL_BLOCKS];             // 0 = free, otherwise owner PID

public:
    MemoryManager();
    ~MemoryManager();
    bool allocate(int pid, int sizeKB);   // returns false if not enough memory
    void deallocate(int pid);
    int blocksNeeded(int sizeKB) const;
    int freeBlocks() const;
    void display() const;
};

#endif
