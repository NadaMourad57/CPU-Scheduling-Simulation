# CPU-Scheduling-Simulation

### Nadine:

`Basic Algorithms`:

-FCFS (First Come First Serve)

-RR (Round Robin)

`Complex Algorithms`:

-HRRN (Highest Response Ratio Next)

-Aging

`Input handling`

`Makefile`

lab6/
├── CPU-Scheduling-Simulation/      # Folder containing the simulation code
│   ├── cpu_scheduling_algo.cpp    # Source code for CPU scheduling algorithms
│   ├── cpu_scheduling_algo.o      # Object file generated by the compiler
│   ├── CPU-Scheduling-Simulation  # Executable program
│   ├── Makefile                   # Makefile to compile the program
│   └── README.md                  # README file for this lab
└── ../testcases/                  # Folder containing input and output test cases
    ├── 01a-input.txt             # Input file for testing
    ├── 01a-output.txt            # Output file for expected results
    ├── 02a-input.txt             # Another test case input
    ├── 02a-output.txt            # Expected output for the second test case
    └── ...                       # Additional input/output test cases


to run:
cd ~/Desktop/Operating_Systems/OS\ Labs/lab6/CPU-Scheduling-Simulation
make
./CPU-Scheduling-Simulation < ../testcases/01a-input.txt
