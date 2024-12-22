#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <algorithm>
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


//Algorithms
// FCFS Scheduling Algorithm
// FCFS Scheduling implementation
void fcfs(const std::vector<std::tuple<std::string, int, int>>& processes) {
    int n = processes.size();

    // Vectors to store process details
    vector<int> startTime(n), finishTime(n), turnaroundTime(n), waitingTime(n);

    // Sorting by Arrival Time (if not already sorted)
    vector<tuple<string, int, int>> sortedProcesses = processes;
    sort(sortedProcesses.begin(), sortedProcesses.end(), [](auto& a, auto& b) {
        return get<1>(a) < get<1>(b);
    });

    // Initialize start time, finish time, turnaround time, and waiting time
    int currentTime = 0;
    for (int i = 0; i < n; ++i) {
        string processName = get<0>(sortedProcesses[i]);
        int arrivalTime = get<1>(sortedProcesses[i]);
        int serviceTime = get<2>(sortedProcesses[i]);

        startTime[i] = max(currentTime, arrivalTime); // start when available
        finishTime[i] = startTime[i] + serviceTime;
        turnaroundTime[i] = finishTime[i] - arrivalTime;
        waitingTime[i] = turnaroundTime[i] - serviceTime;

        currentTime = finishTime[i]; // update current time
    }

    // Calculate average turnaround time and average waiting time as float
    float avgTurnaroundTime = 0.0f, avgWaitingTime = 0.0f;
    for (int i = 0; i < n; ++i) {
        avgTurnaroundTime += turnaroundTime[i];
        avgWaitingTime += waitingTime[i];
    }

    avgTurnaroundTime /= n;
    avgWaitingTime /= n;

    // Output Results
    cout << "FCFS Scheduling:\n";
    for (int i = 0; i < n; ++i) {
        cout << "Process " << get<0>(sortedProcesses[i]) << ": "
             << "Arrival Time = " << get<1>(sortedProcesses[i]) 
             << ", Start Time = " << startTime[i] 
             << ", Finish Time = " << finishTime[i] 
             << ", Turnaround Time = " << turnaroundTime[i] 
             << ", Waiting Time = " << waitingTime[i] << endl;
    }

    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
}

int main() {
    std::string operation;
    std::vector<std::pair<int, int>> algorithms; // {algorithm_id, quantum (-1 if not applicable)}
    int lastInstant, processCount;
    std::vector<std::tuple<std::string, int, int>> processes; // {name, arrival_time, service_time}

    parseInput(operation, algorithms, lastInstant, processCount, processes);
    printInput(operation, algorithms, lastInstant, processCount, processes);


    // Call the FCFS algorithm
    fcfs(processes);

    return 0;
}
