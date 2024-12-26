
TARGET = CPU-Scheduling-Simulation

# Build rules
all: main.o
	g++ -o $(TARGET) main.o

main.o: main.cpp
	g++ -c main.cpp

# Clean up
clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
