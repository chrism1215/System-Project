#include <iomanip>  // for std::setw, std::setfill, std::hex, std::dec



#include "memory_manager.h"
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream>
#include <iomanip> // (NEW) Needed for formatting hex addresses nicely

/**
 * @brief Constructor initializes the memory manager with one large free block.
 */
MemoryManager::MemoryManager() : next_id(0) {
    std::memset(memory, 0, MEMORY_SIZE);
    blocks.emplace_back(0, MEMORY_SIZE, true);
}

/**
 * @brief Calculates the next highest power of two for a given size.
 * @param n Size to find the next power of two for.
 * @return The next power of two greater than or equal to n.
 */
size_t MemoryManager::nextPowerOfTwo(size_t n) {
    size_t power = 1;
    while (power < n) {
        power <<= 1;
    }
    return power;
}

/**
 * @brief Finds the best fitting free block for allocation.
 * @param size Size needed.
 * @return Index of the best fitting block, or -1 if not found.
 */
int MemoryManager::findBestFit(size_t size) {
    int best_index = -1;
    size_t best_size = MEMORY_SIZE + 1;

    for (size_t i = 0; i < blocks.size(); ++i) {
        if (blocks[i].is_free && blocks[i].size >= size && blocks[i].size < best_size) {
            best_index = static_cast<int>(i);
            best_size = blocks[i].size;
        }
    }
    return best_index;
}

/**
 * @brief Merges adjacent free blocks to maintain buddy system.
 */
void MemoryManager::mergeBlocks() {
    for (size_t i = 0; i < blocks.size() - 1;) {
        if (blocks[i].is_free && blocks[i + 1].is_free) {
            blocks[i].size += blocks[i + 1].size;
            blocks.erase(blocks.begin() + i + 1);
        } else {
            ++i;
        }
    }
}

/**
 * @brief Processes a command read from the input file.
 * @param command The command string (INSERT, READ, DELETE, UPDATE, DUMP).
 */
void MemoryManager::processCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string op;
    iss >> op;

    if (op == "INSERT") {
        size_t size;
        std::string data;
        iss >> size;
        std::getline(iss, data);
        if (!data.empty() && data[0] == ' ') {
            data = data.substr(1);
        }
        insert(size, data);
    } else if (op == "READ") {
        int id;
        iss >> id;
        find(id);
    } else if (op == "DELETE") {
        int id;
        iss >> id;
        deleteBlock(id);
    } else if (op == "UPDATE") {
        int id;
        std::string new_data;
        iss >> id;
        std::getline(iss, new_data);
        if (!new_data.empty() && new_data[0] == ' ') {
            new_data = new_data.substr(1);
        }
        update(id, new_data);
    } else if (op == "DUMP") {
        dump();
    } else {
        std::cout << "Unknown command: " << op << std::endl;
    }
}

/**
 * @brief Allocates memory and stores the given data.
 * @param size Requested size.
 * @param data Data to store.
 */
void MemoryManager::insert(size_t size, const std::string& data) {
    size = nextPowerOfTwo(size);
    int index = findBestFit(size);

    if (index != -1) {
        Block& block = blocks[index];
        block.is_free = false;
        block.id = next_id;

        size_t copy_len = std::min(data.size(), block.size);
        std::memcpy(&memory[block.start], data.c_str(), copy_len);

        id_to_block[next_id] = index;
        std::cout << "Inserted ID " << next_id << std::endl;
        next_id++;
    } else {
        std::cout << "No suitable block found for insertion." << std::endl;
    }
}

/**
 * @brief Finds and prints the data at the specified ID.
 * @param id Identifier of the memory block.
 */
void MemoryManager::find(int id) const {
    auto it = id_to_block.find(id);
    if (it != id_to_block.end()) {
        const Block& block = blocks[it->second];
        if (!block.is_free) {
            std::string data(reinterpret_cast<const char*>(&memory[block.start]), block.size);
            std::cout << "Data at ID " << id << ": " << data << std::endl;
            return;
        }
    }
    std::cout << "Nothing at " << id << std::endl;
}

/**
 * @brief Deletes the block with the specified ID.
 * @param id Identifier of the block to delete.
 */
void MemoryManager::deleteBlock(int id) {
    auto it = id_to_block.find(id);
    if (it != id_to_block.end()) {
        Block& block = blocks[it->second];
        block.is_free = true;
        block.id = -1;
        mergeBlocks();
        id_to_block.erase(it);
        std::cout << "Deleted ID " << id << std::endl;
    } else {
        std::cout << "Nothing at " << id << std::endl;
    }
}

/**
 * @brief Updates the data of a block with the specified ID.
 * @param id Identifier of the block.
 * @param new_data New data to update.
 */
void MemoryManager::update(int id, const std::string& new_data) {
    auto it = id_to_block.find(id);
    if (it != id_to_block.end()) {
        Block& block = blocks[it->second];
        if (block.size >= new_data.size()) {
            std::memcpy(&memory[block.start], new_data.c_str(), new_data.size());
            std::cout << "Updated ID " << id << std::endl;
        } else {
            deleteBlock(id);
            insert(new_data.size(), new_data);
        }
    } else {
        std::cout << "Nothing at " << id << std::endl;
    }
}

/**
 * @brief Dumps the status of all memory blocks.
 * 
 * Displays each block with:
 * - Start and End address in hexadecimal
 * - Allocation status
 * - ID if allocated
 * - Size in bytes
 * - Stored data if allocated
 */
void MemoryManager::dump() const {
    std::cout << "--- Memory Dump ---" << std::endl;
    for (const auto& block : blocks) {
        // Print Start and End address in 4-digit hexadecimal
        std::cout << "0x" 
                  << std::setw(4) << std::setfill('0') << std::hex << block.start
                  << " - 0x" 
                  << std::setw(4) << std::setfill('0') << std::hex << (block.start + block.size)
                  << ": ";

        // Print if block is FREE or ALLOCATED
        if (block.is_free) {
            std::cout << "FREE ";
        } else {
            std::cout << "ALLOCATED (ID: " << block.id << ") ";
        }

        // Print Size of the block
        std::cout << "(Size: " << std::dec << block.size << " bytes)" << std::endl;

        // If block is allocated, also print Data
        if (!block.is_free) {
            std::cout << "Data: ";
            for (size_t i = 0; i < block.size; ++i) {
                if (memory[block.start + i] == 0) break; // Stop at null character
                std::cout << static_cast<char>(memory[block.start + i]);
            }
            std::cout << "\n" << std::endl; // Newline after data
        }
    }
}

