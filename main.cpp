#include <iostream>   // For standard input/output
#include <fstream>    // For file input
#include <sstream>    // For string stream processing
#include <string>     // For using std::string
#include "memory_manager.h" // Include the MemoryManager class

/**
 * @brief Main entry point of the program.
 * 
 * The program expects a command-line argument that specifies the path to a .cmmd file.
 * The .cmmd file contains commands (INSERT, READ, DELETE, UPDATE, DUMP) that the memory manager will process.
 * 
 * @param argc Argument count.
 * @param argv Argument vector (list of arguments).
 * @return int Returns 0 if successful, non-zero if an error occurred.
 */
int main(int argc, char* argv[]) {
    // Check if the correct number of arguments is provided
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <commands.cmmd>" << std::endl;
        return 1; // Exit with error if no input file is given
    }

    // Open the input file (cmmd file)
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1; // Exit with error if the file can't be opened
    }

    MemoryManager mem_mgr; // Create a new MemoryManager object

    std::string line;
    // Read the input file line by line
    while (std::getline(file, line)) {
        mem_mgr.processCommand(line); // Process each command through the memory manager
    }

    return 0; // Exit successfully
}
