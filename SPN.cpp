#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct process {
    std::string name;
    int arrival_time;
    int service_time;
    int remaining_service_time;
    int finish_time = 0;
    int turnaround_time = 0;
    float norm_turnaround_time = 0.0;
};

struct algorithm {
    int algorithm_id;
    int quantum;
    int mode;
};

void updateFinishTimes(std::vector<process>& processes, const std::vector<std::string>& output) {
    for (std::vector<process>::iterator p = processes.begin(); p != processes.end(); ++p) {
        std::vector<std::string>::const_reverse_iterator it = std::find(output.rbegin(), output.rend(), p->name);
        if (it != output.rend()) {
            p->finish_time = std::distance(output.begin(), it.base()) - 1;
        }
    }
}

void outputTrace(std::vector<process>& processes, std::vector<std::string>& output) {
    int timelineLength = output.size();
    std::cout << "SPN   ";
    for (int i = 0; i <= timelineLength; ++i) {
        std::cout << i % 10 << " ";
    }
    std::cout << "\n------------------------------------------------\n";

    for (const auto& process : processes) {
        std:: string processName = process.name;
        std::cout << processName << "     |";
        for (int i = 0; i < timelineLength; ++i) {
            if (output[i] == processName) {
                std::cout << "*|";
            } else if (i < process.finish_time and i >= process.arrival_time) {
                std::cout << ".|";
            }
            else {
                std::cout << " |";
            }
        }
        std::cout << "\n";
    }

   std:: cout << "------------------------------------------------\n";
}


// SPN Algorithm (Shortest Process Next)
std::vector<std::string> SPN(std::vector<process>& processes, std::vector<std::string>& output) {
    int n = processes.size();
    // Sort processes by shortest service time
    std::sort(processes.begin(), processes.end(),
              [](const process& a, const process& b) {
                  return a.service_time < b.service_time;
              });

    int current_time = 0;

    for (auto& p : processes) {
        if (p.arrival_time > current_time) {
            current_time = p.arrival_time;
        }
        p.finish_time = current_time + p.service_time;
        p.turnaround_time = p.finish_time - p.arrival_time;
        p.norm_turnaround_time = static_cast<float>(p.turnaround_time) / p.service_time;

        current_time += p.service_time;

        for (int i = 0; i < p.service_time; ++i) {
            output.push_back(p.name);
        }
    }

    return output;
}




process getMRTprocess(std::vector<process>& processes) {
    auto minProcessIt = std::min_element(
        processes.begin(), processes.end(),
        [](const process& a, const process& b) {
            if (a.remaining_service_time == 0) return false;
            if (b.remaining_service_time == 0) return true;
            return a.remaining_service_time < b.remaining_service_time;
        });

    if (minProcessIt == processes.end() || minProcessIt->remaining_service_time == 0) {
        throw std::runtime_error("No process with remaining service time > 0 found.");
    }

    return *minProcessIt;
}

std::vector<std::string> SRT(std::vector<process>& processes, std::vector<std::string>& output, int total_time) {
    for (int i = 0; i < total_time; i++) {
        auto minProcessIt = std::min_element(
            processes.begin(), processes.end(),
            [](const process& a, const process& b) {
                if (a.remaining_service_time == 0) return false; // Skip processes with 0 remaining time
                if (b.remaining_service_time == 0) return true;  // Favor valid processes
                return a.remaining_service_time < b.remaining_service_time;
            });

        if (minProcessIt == processes.end() || minProcessIt->remaining_service_time == 0) {
            std::cerr << "No process with remaining service time > 0 found at time " << i << "\n";
            break;
        }

        minProcessIt->remaining_service_time -= 1;
        std::cout << "Process " << minProcessIt->name << " is running\n";
        std::cout << "Remaining time for process " << minProcessIt->name << " is " << minProcessIt->remaining_service_time << "\n";

        // Add the process name to the output
        output.push_back(minProcessIt->name);
    }

    return output;
}


// Function to apply the selected algorithm
std::vector<std::string> apply_algorithm(const algorithm& algo, std::vector<process>& processes, std::vector<std::string>& output,int total_time) {
    switch (algo.algorithm_id) {
        case 1:
            // FCFS(processes);
            break;
        case 2:
            // RoundRobin(processes, algo.quantum);
            break;
        case 3:
            // SPN(processes, output);
            break;
        case 4:
            SRT(processes, output,total_time);
            break;
        case 5:
            // HRRN(processes);
            break;
        case 6:
            // FB_1(processes);
            break;
        case 7:
            // FB_2i(processes);
            break;
        case 8:
            // Aging(processes, algo.quantum);
            break;
        default:
            std::cerr << "Error: Invalid algorithm ID: " << algo.algorithm_id << "\n";
    }
    return output;
}

int main() {
    // Define the processes
    process p1 ;
    process p2 ;
    process p3 ;
    process p4;
    int total_time = 20;
    p1.name = "A";
    p1.arrival_time = 0;
    p1.service_time = 1;
    p1.remaining_service_time = p1.service_time;

    p2.name = "B";
    p2.arrival_time = 1;
    p2.service_time = 9;
    p2.remaining_service_time = p2.service_time;

    p3.name = "C";

    p3.arrival_time = 2;
    p3.service_time = 1;
    p3.remaining_service_time = p3.service_time;

    p4.name = "D";
    p4.arrival_time = 3;
    p4.service_time = 9;
    p4.remaining_service_time = p4.service_time;

    std::vector<process> processes = {p1, p2, p3, p4};

    // Define the algorithm
    algorithm algo;
    algo.algorithm_id = 4; // SRT
    algo.quantum = 0;

    // Output vector
    std::vector<std::string> output;

    // Apply the algorithm
    apply_algorithm(algo, processes, output,total_time);
    updateFinishTimes(processes, output);
    // Print the results
    outputTrace(processes, output);
    // outputStats(processes);

    return 0;
}
