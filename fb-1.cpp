#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <stdio.h>
#include <math.h>
// Define the process structure
struct process {

    std::string name;
    int arrival_time;
    int service_time;
    int remaining_service_time;
    int finish_time = 0;
    int turnaround_time = 0;
    float norm_turnaround_time = 0.0;

    process(std::string n, int at, int st)
        : name(n), arrival_time(at), service_time(st), remaining_service_time(st) {}
};


// Function to calculate and run FB-1
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

int main() {
    // Input data for processes
    std::vector<process> processes = {
        {"A", 0, 3},  // name, arrival_time, service_time, remaining_service_time
        {"B", 2, 6},
        {"C", 4, 4},
        {"D", 6, 5},
        {"E", 8, 2}
    };

    int total_time = 20; // Total simulation time
    std::vector<std::string> output;

    // Run FB-1 algorithm
    output = FB_1(processes, output, total_time);

    // Print the output timeline
    std::cout << "Timeline:\n";
    for (int i = 0; i < output.size(); i++) {
        std::cout << output[i] << " ";
    }
    std::cout << "\n";



    return 0;
}
