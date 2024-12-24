#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <tuple>
#include <utility>
#include <algorithm>
#include<queue>
#include <iomanip>  // For controlling output format
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

std::vector<std::string>  fcfs(const vector<tuple<string, int, int>>& processes) {
    int n = processes.size();

    // Vectors to store process details
    vector<int> startTime(n), finishTime(n), turnaroundTime(n), waitingTime(n);
    vector<float> normTurnaroundTime(n);

    // Sorting by Arrival Time (if not already sorted)
    vector<tuple<string, int, int>> sortedProcesses = processes;
    sort(sortedProcesses.begin(), sortedProcesses.end(), [](auto& a, auto& b) {
        return get<1>(a) < get<1>(b);
    });

    // Output vector to represent the process at each time slot
    vector<string> timeline;

    // Initialize start time, finish time, turnaround time, and waiting time
    int currentTime = 0;
    for (int i = 0; i < n; ++i) {
        string processName = get<0>(sortedProcesses[i]);
        int arrivalTime = get<1>(sortedProcesses[i]);
        int serviceTime = get<2>(sortedProcesses[i]);

        // Fill the timeline with "IDLE" for gaps before the process starts
        while (currentTime < arrivalTime) {
            timeline.push_back("IDLE");
            ++currentTime;
        }

        // Process starts when available
        startTime[i] = max(currentTime, arrivalTime);
        finishTime[i] = startTime[i] + serviceTime;

        // Fill the timeline with the process name for the duration of its execution
        for (int t = 0; t < serviceTime; ++t) {
            timeline.push_back(processName);
            ++currentTime;
        }

        turnaroundTime[i] = finishTime[i] - arrivalTime;
        waitingTime[i] = turnaroundTime[i] - serviceTime;
        normTurnaroundTime[i] = (float)turnaroundTime[i] / serviceTime;
    }

    // Calculate average turnaround time and average waiting time as float
    float avgTurnaroundTime = 0.0f, avgWaitingTime = 0.0f, avgNormTurnaroundTime = 0.0f;
    for (int i = 0; i < n; ++i) {
        avgTurnaroundTime += turnaroundTime[i];
        avgWaitingTime += waitingTime[i];
        avgNormTurnaroundTime += normTurnaroundTime[i];
    }

    avgTurnaroundTime /= n;
    avgWaitingTime /= n;
    avgNormTurnaroundTime /= n;


    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Norm Turnaround Time: " << avgNormTurnaroundTime << endl;

    return timeline;
}

std::vector<std::string> roundRobin(const vector<tuple<string, int, int>>& processes, int quantum, int lastInstant) {
    int n = processes.size();
    
    // Ensure variables are cleared before every run
    vector<int> remainingServiceTime(n);
    vector<int> startTime(n, -1), finishTime(n), turnaroundTime(n), waitingTime(n);
    queue<int> readyQueue;
    
    // Output vector to represent the process at each time slot
    vector<string> timeline;

    // Initialize remaining service times
    for (int i = 0; i < n; ++i) {
        remainingServiceTime[i] = get<2>(processes[i]);
    }
    
    // If quantum is invalid, print error and return empty timeline
    if (quantum <= 0) {
        std::cerr << "Error: Invalid quantum value: " << quantum << "\n";
        return {};
    }

    // Sort processes based on arrival time
    vector<tuple<string, int, int>> sortedProcesses = processes;
    sort(sortedProcesses.begin(), sortedProcesses.end(), [](auto& a, auto& b) {
        return get<1>(a) < get<1>(b);
    });
    
    int currentTime = 0, i = 0;
    
    // Process execution loop
    while (i < n || !readyQueue.empty()) {
        // If the current time exceeds lastInstant, stop scheduling
        if (currentTime > lastInstant) {
            break;
        }

        // Add all processes that have arrived to the queue
        while (i < n && get<1>(sortedProcesses[i]) <= currentTime) {
            readyQueue.push(i);
            ++i;
        }
        
        // If queue is empty, jump to next arrival time and add "IDLE" slots
        if (readyQueue.empty()) {
            if (i < n) {
                while (currentTime < get<1>(sortedProcesses[i]) && currentTime <= lastInstant) {
                    timeline.push_back("IDLE");
                    ++currentTime;
                }
            }
            continue;
        }
        
        // Process next in queue
        int idx = readyQueue.front();
        readyQueue.pop();
        
        // Execute process for its quantum or until completion
        int timeSlice = min(quantum, remainingServiceTime[idx]);
        for (int t = 0; t < timeSlice; ++t) {
            timeline.push_back(get<0>(sortedProcesses[idx]));
        }
        remainingServiceTime[idx] -= timeSlice;
        currentTime += timeSlice;
        
        // Update start time if this is the first time the process is being executed
        if (startTime[idx] == -1) {
            startTime[idx] = currentTime - timeSlice;  // First time execution, set start time
        }
        
        // If process finishes, update finish time, turnaround time, and waiting time
        if (remainingServiceTime[idx] == 0) {
            finishTime[idx] = currentTime;
            turnaroundTime[idx] = finishTime[idx] - get<1>(sortedProcesses[idx]);
            waitingTime[idx] = turnaroundTime[idx] - get<2>(sortedProcesses[idx]);
        }
        
        // If process is not done, put it back in the queue
        if (remainingServiceTime[idx] > 0) {
            readyQueue.push(idx);
        }
    }
    
    // Compute average turnaround time and waiting time
    float avgTurnaroundTime = 0.0f, avgWaitingTime = 0.0f;
    for (int i = 0; i < n; ++i) {
        avgTurnaroundTime += turnaroundTime[i];
        avgWaitingTime += waitingTime[i];
    }
    
    avgTurnaroundTime /= n;
    avgWaitingTime /= n;
    
    // Output the results
    cout << "Round Robin Scheduling (Quantum = " << quantum << "):\n";
    for (int i = 0; i < n; ++i) {
        cout << "Process " << get<0>(sortedProcesses[i]) 
             << ": Arrival Time = " << get<1>(sortedProcesses[i]) 
             << ", Start Time = " << startTime[i] 
             << ", Finish Time = " << finishTime[i] 
             << ", Turnaround Time = " << turnaroundTime[i] 
             << ", Waiting Time = " << waitingTime[i] << endl;
    }
    
    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;

    return timeline;
}


std::vector<std::string> hrrn(std::vector<std::tuple<std::string, int, int>>& processes) {
    int n = processes.size();
    std::vector<bool> completed(n, false); // Track completed processes
    std::vector<int> startTime(n), finishTime(n), turnaroundTime(n), waitingTime(n);
    std::vector<float> normTurnaroundTime(n); // Normalized Turnaround Time
    int currentTime = 0;
    std::vector<std::string> timeSlotProcess; // Vector to store process running at each time slot

    // Track total turnaround and waiting times for average calculation
    float totalTurnaroundTime = 0.0f, totalWaitingTime = 0.0f, totalNormTurnaroundTime = 0.0f;

    for (int completedCount = 0; completedCount < n; ++completedCount) {
        // Calculate response ratios for all ready processes
        float maxResponseRatio = -1.0f;
        int selectedProcess = -1;

        std::cout << "Current Time: " << currentTime << "\n";
        for (int i = 0; i < n; ++i) {
            if (!completed[i] && std::get<1>(processes[i]) <= currentTime) {
                int arrivalTime = std::get<1>(processes[i]);
                int serviceTime = std::get<2>(processes[i]);
                int waitingTime = currentTime - arrivalTime;
                float responseRatio = (waitingTime + serviceTime) / (float)serviceTime;

                // Print the response ratio for the process
                std::cout << "Process " << std::get<0>(processes[i])
                          << ": Response Ratio = " << responseRatio << "\n";

                if (responseRatio > maxResponseRatio) {
                    maxResponseRatio = responseRatio;
                    selectedProcess = i;
                }
            }
        }

        // If no process is ready, advance time
        if (selectedProcess == -1) {
            currentTime++;
            continue;
        }

        // Execute the selected process
        int arrivalTime = std::get<1>(processes[selectedProcess]);
        int serviceTime = std::get<2>(processes[selectedProcess]);

        startTime[selectedProcess] = currentTime;
        finishTime[selectedProcess] = currentTime + serviceTime;
        turnaroundTime[selectedProcess] = finishTime[selectedProcess] - arrivalTime;
        waitingTime[selectedProcess] = turnaroundTime[selectedProcess] - serviceTime;

        // Calculate normalized turnaround time
        normTurnaroundTime[selectedProcess] = (float)turnaroundTime[selectedProcess] / (float)serviceTime;

        // Add process to the timeSlotProcess for each time unit it runs
        for (int t = 0; t < serviceTime; ++t) {
            timeSlotProcess.push_back(std::get<0>(processes[selectedProcess]));
        }

        currentTime += serviceTime;
        completed[selectedProcess] = true;

        // Update total turnaround and waiting times for averages
        totalTurnaroundTime += turnaroundTime[selectedProcess];
        totalWaitingTime += waitingTime[selectedProcess];
        totalNormTurnaroundTime += normTurnaroundTime[selectedProcess];

        std::cout << "Selected Process: " << std::get<0>(processes[selectedProcess])
                  << " (Max Response Ratio = " << maxResponseRatio << ")\n\n";
    }

    // Calculate average turnaround, waiting, and normalized turnaround times
    float avgTurnaroundTime = totalTurnaroundTime / n;
    float avgWaitingTime = totalWaitingTime / n;
    float avgNormTurnaroundTime = totalNormTurnaroundTime / n;


    cout << "\nAverage Turnaround Time: " << avgTurnaroundTime << endl;
    cout << "Average Waiting Time: " << avgWaitingTime << endl;
    cout << "Average Norm Turnaround Time: " << avgNormTurnaroundTime << endl;

    return timeSlotProcess; // Return the time slot process history
}




int main() {
    std::string operation;
    std::vector<std::pair<int, int>> algorithms; // {algorithm_id, quantum (-1 if not applicable)}
    int lastInstant, processCount;
    std::vector<std::tuple<std::string, int, int>> processes; // {name, arrival_time, service_time}

    parseInput(operation, algorithms, lastInstant, processCount, processes);
    printInput(operation, algorithms, lastInstant, processCount, processes);


    

     // If Round-Robin algorithm is selected
    for (const auto& algo : algorithms) {
        if (algo.first == 1) { // Assuming '1' corresponds to fcfs
            std::vector<std::string> timeSlotProcess = fcfs(processes);

            // Print the timeSlotProcess vector
            std::cout << "\nProcess Running at Each Time Slot:\n";
            for (const auto& process : timeSlotProcess) {
                std::cout << process << " ";
        } std::cout << endl;
        
        }
        else if (algo.first == 2) { // Assuming '2' corresponds to Round-Robin
            std::vector<std::string> timeSlotProcess = roundRobin(processes, algo.second,lastInstant);

            // Print the timeSlotProcess vector
            std::cout << "\nProcess Running at Each Time Slot:\n";
            for (const auto& process : timeSlotProcess) {
                std::cout << process << " ";}
            
        }
        else if (algo.first == 5) { // Assuming '5' corresponds to hrrn
            std::vector<std::string> timeSlotProcess = hrrn(processes);

            // Print the timeSlotProcess vector
            std::cout << "\nProcess Running at Each Time Slot:\n";
            for (const auto& process : timeSlotProcess) {
                std::cout << process << " ";
            }
            std::cout << endl;
        }
    }

    return 0;
}
