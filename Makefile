
TARGET = CPU-Scheduling-Simulation

# Build rules
all: cpu_scheduling_algo.o
	g++ -o $(TARGET) cpu_scheduling_algo.o

main.o: cpu_scheduling_algo.cpp
	g++ -c cpu_scheduling_algo.cpp

# Clean up
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
