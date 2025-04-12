#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <vector>
#include <unordered_map>
#include <string>

const int MEMORY_SIZE = 65535;

struct Block {
    size_t start;
    size_t size;
    bool is_free;
    int id;

    Block(size_t start, size_t size, bool is_free, int id = -1)
        : start(start), size(size), is_free(is_free), id(id) {}
};

class MemoryManager {
private:
    unsigned char memory[MEMORY_SIZE];
    std::vector<Block> blocks;
    std::unordered_map<int, size_t> id_to_block;
    int next_id;

    size_t nextPowerOfTwo(size_t n);
    int findBestFit(size_t size);
    void mergeBlocks();

public:
    MemoryManager();
    void processCommand(const std::string& command);
    void insert(size_t size, const std::string& data);
    void find(int id) const;
    void deleteBlock(int id);
    void update(int id, const std::string& new_data);
    void dump() const;
};

#endif // MEMORY_MANAGER_H
