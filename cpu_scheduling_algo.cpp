#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
using namespace std;

// Function to parse the input
void parseInput(
    std::string& operation, 
    std::vector<std::pair<int, int>>& algorithms, 
    int& lastInstant, 
    int& processCount, 
    std::vector<std::tuple<std::string, int, int>>& processes) 
{
    // Read operation type ("trace" or "stats")
    std::cin >> operation;

    // Read algorithms and their parameters
    std::string algorithmChunk;
    std::cin >> algorithmChunk;
    std::stringstream algoStream(algorithmChunk);
    std::string token;

    while (std::getline(algoStream, token, ',')) {
        size_t hyphenPos = token.find('-');
        int algorithmId = std::stoi(token.substr(0, hyphenPos));
        int quantum = (hyphenPos != std::string::npos) ? std::stoi(token.substr(hyphenPos + 1)) : -1;
        algorithms.emplace_back(algorithmId, quantum);
    }

    // Read last instant and process count
    std::cin >> lastInstant >> processCount;
    std::cin.ignore(); // Ignore trailing newline

    // Read process details
    for (int i = 0; i < processCount; ++i) {
        std::string processLine;
        std::getline(std::cin, processLine);

        if (processLine.empty()) {
            std::cerr << "Error: Missing process details on line " << (i + 1) << "\n";
            continue;
        }

        size_t pos1 = processLine.find(',');
        size_t pos2 = processLine.find(',', pos1 + 1);

        if (pos1 == std::string::npos || pos2 == std::string::npos) {
            std::cerr << "Error: Invalid format for process line: " << processLine << "\n";
            continue;
        }

        std::string processName = processLine.substr(0, pos1);
        int arrivalTime = std::stoi(processLine.substr(pos1 + 1, pos2 - pos1 - 1));
        int serviceTime_priority = std::stoi(processLine.substr(pos2 + 1));

        processes.emplace_back(processName, arrivalTime, serviceTime_priority);
    }
}

// Function to print parsed input for verification
void printInput(
    const std::string& operation, 
    const std::vector<std::pair<int, int>>& algorithms, 
    int lastInstant, 
    int processCount, 
    const std::vector<std::tuple<std::string, int, int>>& processes) 
{
    std::cout << "Operation: " << operation << "\n";
    std::cout << "Algorithms:\n";
    for (const auto& algo : algorithms) {
        std::cout << "ID: " << algo.first 
                  << ", Quantum: " << (algo.second != -1 ? std::to_string(algo.second) : "N/A") 
                  << "\n";
    }
    std::cout << "Last Instant: " << lastInstant << "\n";
    std::cout << "Process Count: " << processCount << "\n";
    std::cout << "Processes:\n";
    for (const auto& process : processes) {
        std::cout << "Name: " << std::get<0>(process) 
                  << ", Arrival Time: " << std::get<1>(process) 
                  << ", Service Time / Priority: " << std::get<2>(process) 
                  << "\n";
    }
}

int main() {
    std::string operation;
    std::vector<std::pair<int, int>> algorithms; // {algorithm_id, quantum (-1 if not applicable)}
    int lastInstant, processCount;
    std::vector<std::tuple<std::string, int, int>> processes; // {name, arrival_time, service_time}

    parseInput(operation, algorithms, lastInstant, processCount, processes);
    printInput(operation, algorithms, lastInstant, processCount, processes);


    return 0;
}
