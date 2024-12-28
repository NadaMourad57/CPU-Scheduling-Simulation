#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <stdio.h>
#include <math.h>

struct process {
    std::string name;
    int arrival_time;
    int initial_priority;
    int current_priority;
    int service_time;
    int remaining_service_time;
    int finish_time = 0;
    int turnaround_time = 0;
    float norm_turnaround_time = 0.0;
    bool in_queue = false;
};

struct algorithm {
    int algorithm_id;
    int quantum=-1;
};

//------------------------------------------------------------------------------------------------------------------------//
//utilities 

void parseInput(
    std::string& operation,
    std::vector<algorithm>& algorithms,
    int& lastInstant,
    std::vector<process>& processes,
    int& numProcesses
)
{
    std::cin >> operation;
    std::string algorithmChunk;
    std::cin >> algorithmChunk;
    std::stringstream algoStream(algorithmChunk);
    std::string token;

    while (std::getline(algoStream, token, ',')) {
        size_t hyphenPos = token.find('-');
        int algorithmId = std::stoi(token.substr(0, hyphenPos));
        int quantum = (hyphenPos != std::string::npos) ? std::stoi(token.substr(hyphenPos + 1)) : -1;
        algorithm algo;
        algo.algorithm_id = algorithmId;
        algo.quantum = quantum;
        algorithms.push_back(algo);
    }


    std::cin >> lastInstant;
    std::cin >> numProcesses;
    std::cin.ignore();
    std::string processLine;
    for (int i = 0; i < numProcesses; ++i) {
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
        int serviceTime = std::stoi(processLine.substr(pos2 + 1));

        process proc;
        proc.name = processName;
        proc.arrival_time = arrivalTime;
        proc.service_time = serviceTime;
        proc.remaining_service_time = serviceTime;
        proc.initial_priority = serviceTime;
        proc.current_priority = serviceTime;
        processes.push_back(proc);
    }
}

void updateFinishTimes(std::vector<process>& processes, const std::vector<std::string>& output) {
    for (auto& p : processes) {
        auto it = std::find(output.rbegin(), output.rend(), p.name);
        if (it != output.rend()) {
            p.finish_time = std::distance(output.begin(), it.base()) - 1;
        }
    }
}


void outputTrace( std::vector<process>& processes,  std::vector<std::string>& output,  std::string& algoName) {
    std::cout << std::left;
    std::cout << std::setw(6) << algoName;
    int timelineLength = output.size();

    for (int i = 0; i <= timelineLength; ++i) {
        std::cout << i % 10 << " ";
    }
    std::cout << "\n------------------------------------------------\n";

    // Print each process timeline
    for ( auto& process : processes) {
        if (algoName=="Aging"){
            process.finish_time=timelineLength;
        }
        std::cout << std::setw(6) <<process.name << "|";
        for (int i = 0; i < timelineLength; ++i) {
            if (output[i] == process.name) {
                std::cout << "*|";
            } else if (i < process.finish_time && i >= process.arrival_time) {
                std::cout << ".|";
            } else {
                std::cout << " |";
            }
        }
        std::cout << " ";
        std::cout << "\n";
    }
    std::cout << "------------------------------------------------";
}
void outputStats(const std::vector<process>& processes, const std::string& algoName) {
    std::cout << algoName << "\n";
    int n = processes.size();
    float meanTurnaround = 0.0, meanNormTurnaround = 0.0;

    for (const auto& p : processes) {
        meanTurnaround += p.turnaround_time;
        meanNormTurnaround += p.norm_turnaround_time;
    }
    meanTurnaround /= n;
    meanNormTurnaround /= n;

    // Helper function to format aligned numbers
    auto alignINT = [](int value) -> std::string {
        if (value < 10)
            return "  " + std::to_string(value); // Single-digit numbers with extra spaces
        else
            return " " + std::to_string(value);  // Double-digit numbers with fewer spaces
    };

    // Helper function to align floating-point numbers
    auto alignFloat = [](double value, int width = 5) -> std::string {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << value;
        std::string str = oss.str();
        return std::string(width - str.length(), ' ') + str;
    };

    // Display table header
    std::cout << "Process    |";
    for (const auto& p : processes) {
        std::cout << std::setw(3) << p.name << "  |";
    }
    std::cout << "\n";

    // Display arrival times
    std::cout << "Arrival    |";
    for (const auto& p : processes) {
        std::cout << alignINT(p.arrival_time) << "  |";
    }
    std::cout << "\n";

    // Display service times
    std::cout << "Service    |";
    for (const auto& p : processes) {
        std::cout << alignINT(p.service_time) << "  |";
    }
    std::cout << " Mean|\n";

    // Display finish times
    std::cout << "Finish     |";
    for (const auto& p : processes) {
        std::cout << alignINT(p.finish_time) << "  |";
    }
    std::cout << "-----|\n";

    // Display turnaround times
    std::cout << "Turnaround |";
    for (const auto& p : processes) {
        std::cout << alignINT(p.turnaround_time) << "  |";
    }
    std::cout << alignFloat(meanTurnaround) << "|\n";

    // Display normalized turnaround times
    std::cout << "NormTurn   |";
    for (const auto& p : processes) {
        std::cout << " " << alignFloat(p.norm_turnaround_time, 4) << "|";
    }
    std::cout << alignFloat(meanNormTurnaround) << "|\n";
}




struct CompareServiceTime {
    bool operator()(const process& a, const process& b) {
        return a.service_time > b.service_time;
    }
};

struct ComparePriority {
    bool operator()(const process& a, const process& b) {
        return a.current_priority > b.current_priority;
    }
};

void printProcess(std::vector<process>& processes) {
    for (int i = 0; i < processes.size(); i++) {
        printf("Process Name: %s\n", processes[i].name.c_str());
        printf("Arrival Time: %d\n", processes[i].arrival_time);
        printf("Service Time: %d\n", processes[i].service_time);
        // printf("Remaining Service Time: %d\n", processes[i].remaining_service_time);
        // printf("Finish Time: %d\n", processes[i].finish_time);
        // printf("Turnaround Time: %d\n", processes[i].turnaround_time);
        // printf("Normalized Turnaround Time: %f\n", processes[i].norm_turnaround_time);
    }
}

void calculateStatsFromOutput(const std::vector<std::string>& output, std::vector<process>& processes) {
    for (auto& p : processes) {
        // Find the first and last occurrences of the process in the output
        auto first_it = std::find(output.begin(), output.end(), p.name);
        auto last_it = std::find(output.rbegin(), output.rend(), p.name);

        if (first_it != output.end()) {
            int first_index = std::distance(output.begin(), first_it);
            int last_index = std::distance(output.begin(), last_it.base()) - 1;

            // Calculate finish time, turnaround time, and normalized turnaround time
            p.finish_time = last_index + 1;  // Finish time is the last occurrence + 1
            p.turnaround_time = p.finish_time - p.arrival_time;
            p.norm_turnaround_time =
                static_cast<float>(p.turnaround_time) / p.service_time;
        }
    }
}



//Algorithms
//------------------------------------------------------------------------------------------------------------------------//
//First Come First Serve
std::vector<std::string> fcfs(std::vector<process>& processes, std::vector<std::string>& output) {
    int n = processes.size();
    std::sort(processes.begin(), processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });
    int currentTime = 0;
    for (int i = 0; i < n; ++i) {
        if (currentTime < processes[i].arrival_time) {
            // Add idle time if the CPU is idle
            for (int idle = currentTime; idle < processes[i].arrival_time; ++idle) {
                output.push_back("-");
            }
            currentTime = processes[i].arrival_time;
        }
        for (int j = 0; j < processes[i].service_time; ++j) {
            output.push_back(processes[i].name); 
        }

        processes[i].finish_time = std::max(currentTime, processes[i].arrival_time) + processes[i].service_time;

        processes[i].turnaround_time = processes[i].finish_time - processes[i].arrival_time;
        processes[i].norm_turnaround_time =
            static_cast<float>(processes[i].turnaround_time) / processes[i].service_time;
        currentTime = processes[i].finish_time;
    }
    return output;
}

//------------------------------------------------------------------------------------------------------------------------//
//Round Robin
std::vector<std::string> RR(std::vector<process>& processes, std::vector<std::string>& output, int quantum) {
    int n = processes.size();
    std::vector<process> backup_processes = processes;
    std::queue<int> ready_queue; 
    int currentTime = 0;
    int j = 0; 
    int old_index = 0;

    std::sort(backup_processes.begin(), backup_processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });

    while (true) {
        while (j < n && backup_processes[j].arrival_time <= currentTime) {
            ready_queue.push(j);
            j++;
        }

        if (ready_queue.empty()) {
            if (j >= n)
                break;
        }

        int processIndex = ready_queue.front();
        ready_queue.pop();

        int timeSlice = std::min(quantum, backup_processes[processIndex].remaining_service_time);

        for (int i = 0; i < timeSlice; i++) {
            output.push_back(backup_processes[processIndex].name);
        }

        currentTime += timeSlice;
        backup_processes[processIndex].remaining_service_time -= timeSlice;

        while (j < n && backup_processes[j].arrival_time <= currentTime) {
            ready_queue.push(j);
            j++;
        }

        if (backup_processes[processIndex].remaining_service_time != 0) {
            ready_queue.push(processIndex);
        }
    }

    return output;
}

//------------------------------------------------------------------------------------------------------------------------//
//Highest Response Ratio Next
std::vector<std::string> HRRN(std::vector<process>& processes, std::vector<std::string>& output) {
    int n = processes.size();
    std::vector<bool> completed(n, false); // Track completed processes
    int current_time = 0;

    // Sort processes by arrival time
    std::sort(processes.begin(), processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });

    for (int completed_count = 0; completed_count < n; ++completed_count) {
        float max_response_ratio = -1.0f;
        int selected_process = -1;

        // Find the process with the highest response ratio
        for (int i = 0; i < n; ++i) {
            if (!completed[i] && processes[i].arrival_time <= current_time) {
                int waiting_time = current_time - processes[i].arrival_time;
                float response_ratio = (waiting_time + processes[i].service_time) / static_cast<float>(processes[i].service_time);

                if (response_ratio > max_response_ratio) {
                    max_response_ratio = response_ratio;
                    selected_process = i;
                }
            }
        }

        // If no process is ready, advance time
        if (selected_process == -1) {
            output.push_back("-");
            current_time++;
            continue;
        }

        // Execute the selected process
        process& current_process = processes[selected_process];
        for (int j = 0; j < current_process.service_time; ++j) {
            output.push_back(current_process.name);
        }
        current_time += current_process.service_time;

        // Update process metrics
        current_process.finish_time = current_time;
        current_process.turnaround_time = current_process.finish_time - current_process.arrival_time;
        current_process.norm_turnaround_time = static_cast<float>(current_process.turnaround_time) / current_process.service_time;

        // Mark the process as completed
        completed[selected_process] = true;
    }

    return output;
}

//------------------------------------------------------------------------------------------------------------------------//
//Aging

std::vector<std::string> aging(std::vector<process>& processes, int total_time, int quantum, std::vector<std::string>& output) {
    int current_time = 0;
    std::vector<process*> ready_queue;

    std::sort(processes.begin(), processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });

    while (current_time <total_time) {
        // Add processes that have arrived to the ready queue
        for (auto& process : processes) {
            if (process.arrival_time <= current_time && !process.in_queue) {
                process.current_priority= process.initial_priority+1;
                process.in_queue = true;
                ready_queue.push_back(&process);
            }
        }

        if (ready_queue.empty()) {
            output.push_back("-");
            current_time++;
            continue;
        }
        else{

         auto highest_priority_process = std::max_element(ready_queue.begin(), ready_queue.end(),
            [](const process* a, const process* b) {
                return a->current_priority < b->current_priority || (a->current_priority == b->current_priority && a->arrival_time < b->arrival_time);
            });

        process* current_process = *highest_priority_process;
        ready_queue.erase(highest_priority_process);

        // Execute the process for the quantum
        for (int i = 0; i < quantum; ++i) {
            output.push_back(current_process->name);
            for (auto& process : processes) {
                if (process.arrival_time <= current_time && !process.in_queue) {
                    process.in_queue = true;
                    process.current_priority = process.initial_priority + 1;
                    ready_queue.push_back(&process);
                }
            }

            for (auto& process : ready_queue) {
                process->current_priority++;
            }

            if (current_time > total_time) {
                break;
            }
            current_time++;
        }
        current_process->current_priority = current_process->initial_priority;
        ready_queue.push_back(current_process);
    }
    }

    return output;
}

//------------------------------------------------------------------------------------------------------------------------//
//Shortest Process Next
std::vector<std::string> SPN(std::vector<process>& processes, std::vector<std::string>& output) {
    int current_time = 0;
    std::vector<process> backup_processes = processes;
    std::priority_queue<process, std::vector<process>, CompareServiceTime> ready_queue;

    while (!processes.empty() || !ready_queue.empty()) {
        // Add processes to the ready queue if they have arrived
        for (auto it = processes.begin(); it != processes.end();) {
            if (it->arrival_time <= current_time) {
                // std::cout << it->name << " added to ready queue\n";
                ready_queue.push(*it);
                it = processes.erase(it);
            } else {
                ++it; // Move to the next process
            }
        }

        // If the ready queue is empty, CPU is idle
        if (ready_queue.empty()) {
            output.push_back("-");
            current_time++;
            continue;
        }

        // Process the shortest job
        process current_process = ready_queue.top();
        ready_queue.pop();

        // std::cout << "Processing " << current_process.name << " from time " << current_time << "\n";

        // Execute the process
        for (int i = 0; i < current_process.service_time; ++i) {
            output.push_back(current_process.name);
        } 

        current_time += current_process.service_time;
    }

    // Print final output
    // // std::cout << "Output: ";
    // for (const auto& p : output) {
    //     std::cout << p << " ";
    // }
    // std::cout << "\n";
    processes = backup_processes;

    return output;
}
//------------------------------------------------------------------------------------------------------------------------//

//Shortest Remaining Time
std::vector<std::string> SRT(std::vector<process>& processes, std::vector<std::string>& output, int total_time) {
    for (int current_time = 0; current_time < total_time; ++current_time) {
        auto minProcessIt = std::min_element(
            processes.begin(), processes.end(),
            [current_time](const process& a, const process& b) {
                if (a.remaining_service_time == 0 || a.arrival_time > current_time) return false;
                if (b.remaining_service_time == 0 || b.arrival_time > current_time) return true;
                return a.remaining_service_time < b.remaining_service_time;
            });

        if (minProcessIt == processes.end() || minProcessIt->remaining_service_time == 0) {
            output.push_back("-"); // Idle time
            continue;
        }

        minProcessIt->remaining_service_time -= 1;
        output.push_back(minProcessIt->name);
        if (minProcessIt->remaining_service_time == 0) {
            minProcessIt->finish_time = current_time + 1;
        }
    }
    return output;
}
//------------------------------------------------------------------------------------------------------------------------//
//Feedback-1
std::vector<std::string> FB_1(std::vector<process>& processes, std::vector<std::string>& output, int total_time) {
    int numberofactiveprocesses = 0; // Maximum number of queues
    int n = processes.size();       // Total number of processes
    std::vector<process> backup_processes = processes;
    std::vector<std::queue<process>> feedback_queues(n); // Feedback queues
    int current_time = 0;
    int process_index = 0; // Index to track processes

    // Sort processes by arrival time initially
    std::sort(processes.begin(), processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });

    while (current_time < total_time) {
        while (process_index < n && processes[process_index].arrival_time <= current_time) {
            feedback_queues[0].push(processes[process_index]);
            numberofactiveprocesses++;
            process_index++;
        }

        bool process_executed = false;

        for (int i = 0; i < n; i++) {
            if (!feedback_queues[i].empty()) {
                process current_process = feedback_queues[i].front();
                feedback_queues[i].pop();

                output.push_back(current_process.name);
                current_process.remaining_service_time -= 1;
                current_time += 1;
                process_executed = true;

                  while (process_index < n && processes[process_index].arrival_time <= current_time) {
            feedback_queues[0].push(processes[process_index]);
            numberofactiveprocesses++;
            process_index++;
        }

                if (current_process.remaining_service_time == 0) {
                    numberofactiveprocesses--;
                    current_process.finish_time = current_time;
                }
                else {
                    if (i==0 && numberofactiveprocesses==1)
                    {
                        feedback_queues[i].push(current_process);
                    }

                    else if (i + 1 < n) {
                            feedback_queues[i + 1].push(current_process);
                    }
                    else {
                            feedback_queues[i].push(current_process);
                        }

                }
                break;
            }
        }


        if (!process_executed) {
            current_time++;
        }
    }

    processes = backup_processes; // Restore original process list
    return output;
}


//------------------------------------------------------------------------------------------------------------------------//
//Feedback-2i
std::vector<std::string> FB_2i(std::vector<process>& processes, std::vector<std::string>& output, int total_time) {
    int numberofactiveprocesses = 0; // Maximum number of queues
    int n = processes.size();       // Total number of processes
    std::vector<process> backup_processes = processes;
    std::vector<std::queue<process>> feedback_queues(n); // Feedback queues
    int current_time = 0;
    int process_index = 0; // Index to track processes

    // Sort processes by arrival time initially
    std::sort(processes.begin(), processes.end(), [](const process& a, const process& b) {
        return a.arrival_time < b.arrival_time;
    });

    // Define time quantum for each feedback queue level
    std::vector<int> time_quantum(n);
    for (int i = 0; i < n; i++) {
        time_quantum[i] = (1 << i); // Time quantum doubles at each lower-priority level
    }

    while (current_time < total_time) {
        // Add newly arrived processes to the first feedback queue
        while (process_index < n && processes[process_index].arrival_time <= current_time) {
            feedback_queues[0].push(processes[process_index]);
            numberofactiveprocesses++;
            process_index++;
        }

        bool process_executed = false;

        // Iterate over feedback queues
        for (int i = 0; i < n; i++) {
            if (!feedback_queues[i].empty()) {
                process current_process = feedback_queues[i].front();
                feedback_queues[i].pop();

                // Execute the process for the time quantum or until it finishes
                int quantum = time_quantum[i];
                int time_slice = std::min(quantum, current_process.remaining_service_time);

                for (int t = 0; t < time_slice; t++) {
                    output.push_back(current_process.name);
                    current_process.remaining_service_time--;
                    current_time++;

                    while (process_index < n && processes[process_index].arrival_time <= current_time) {
                        feedback_queues[0].push(processes[process_index]);
                        numberofactiveprocesses++;
                        process_index++;
                    }
                }

                process_executed = true;

                // Check if the process is finished
                if (current_process.remaining_service_time == 0) {
                    numberofactiveprocesses--;
                    current_process.finish_time = current_time;
                } else {
                    if (i==0 && numberofactiveprocesses==1) {
                        feedback_queues[i].push(current_process);
                    } else
                    if (i + 1 < n) {
                        feedback_queues[i + 1].push(current_process);
                    } else {
                        feedback_queues[i].push(current_process);
                    }
                }

                break; 
            }
        }

        if (!process_executed) {
            current_time++;
        }
    }

    processes = backup_processes; // Restore original process list
    return output;
}

// Algorithm Applying
std::vector<std::string> apply_algorithm(const algorithm& algo, std::vector<process>& processes, int total_time) {
    std::vector<std::string> output;
    switch (algo.algorithm_id) {

        case 1:
            output = fcfs(processes,output);
            break;
        case 2:
            output = RR(processes, output, algo.quantum);
            break;
        case 3:
            output = SPN(processes, output);
            break;
        case 4:
            output = SRT(processes, output, total_time);
            break;
        case 5:
            output = HRRN(processes, output);
            break;
        case 6:
            // printf("FB-1\n");

            output = FB_1(processes, output,total_time);
            break;
        case 7:
            output = FB_2i(processes, output,total_time);
            break;
        case 8:
            output = aging(processes, total_time, algo.quantum, output);
            break;


        default:
        std::cout <<"Invalid Algorithm\n";


        break;
    }
    calculateStatsFromOutput(output, processes);
    return output;
}


//------------------------------------------------------------------------------------------------------------------------//
//main
int main() {
    std::string operation;
    std::vector<algorithm> algorithms;
    int lastInstant;
    int numProcesses;
    std::vector<process> processes;
    parseInput(operation, algorithms, lastInstant, processes, numProcesses);

    for (const auto& algo : algorithms) {
        std::string algoName;
      
        switch (algo.algorithm_id) {
            case 1:
                algoName = "FCFS";
                break;
            case 2:
                algoName = "RR";
                break;
            case 3:
                algoName = "SPN";
                break;
            case 4:
                algoName = "SRT";
                break;
            case 5:
                algoName = "HRRN";
                break;
            case 6:
                algoName = "FB-1";
               break;
            case 7:
                algoName = "FB-2i";
                break;
            case 8:
                algoName = "Aging";
                break;
            default:
                algoName = "Unknown";
                break;
        }
        if (algoName == "RR") {
            algoName = algoName + "-" + std::to_string(algo.quantum);
        }
        // printf("lastInstant: %d\n", lastInstant);
        std::vector<std::string> output = apply_algorithm(algo, processes, lastInstant);


        // std::cout << std::left;
        // std::cout << std::setw(6) << algoName;
        // outputTrace(processes, output); 

        if (operation == "trace") {
            outputTrace(processes, output,algoName);
        } else if (operation == "stats") {
            outputStats(processes, algoName);
        } else {
            std::cerr << "Invalid operation. Use 'trace' or 'stats'.\n";
        }
    }

    return 0;
}
