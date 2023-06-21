# Process Management System
This project was created for a computer systems and networks subject with the aim of scheduling a bunch of simulated processes through memory and CPU allocation. Memory can be allocated assuming no memory limit or with a memory capacity of 2048 MB, in which case the memory can become full. The processes can be scheduled non-preemptively using the shortest-job-first (SJF) algorithm which simply runs the process until completion or the preemptive round-robin (RR) algorithm that switches the processes at the end of each quantum. This quantum of time is inputted into the system which is the length of a single clock cycle. At the start of each clock cycle the system:
1. Identifies whether the currently running process (if any) has completed.
2. Identifies all processes that have been submitted to the system since the last clock cycle.
3. Attempts to allocate memory to all processes in the input queue and if successful moves the allocated process to the ready queue. This is mostly applicable to the best-fit allocation algorithm, as otherwise processes are immediately 'ready' once arrived.
4. Determines the process (if any) that will run in the next cycle.

The following statistics are recorded and printed after execution of all processes:
- Turnoround time - average time between when a process has completed and when it has arrived.
- Time overhead - Maximum and average overhead where overhead for a single process is defined as the turnaound time divided by its service time.
- Makespan - The simulation time when the simulation has ended.

Real processes are created, suspended and terminated as this system runs. These processes are created using the `fork` system call and the new processes image is then replace by the `process` executable (is also built through Makefile). When the simulated process arrives this real process is created and is suspended (for RR) after a clock cycle and then resumed when at front of queue. Once it's service time has elapsed the process is terminated and all of these messages are communicated with the process using Unix signals. 
## Usage
This program can be build using the supplied Makefile and processes are inputted in the following format:
```
0 P4 30 16
29 P2 40 64
99 P1 20 32
```
Where this indicates three processes with the first column being arrival time, the second is process name, the third is total service time and the last column is the amount of memory the process requires. There are a bunch of example inputs with correct outputs included in the `cases` folder.

The scheduling system can be executed by:
```
allocate -f <filename> -s (SJF | RR) -m (infinite | best-fit) -q (1 | 2 | 3)
```
For example:
```
./allocate -f processes.txt -s RR -m best-fit -q 3
```
will execute the system where the processes are stored in the format above in `processes.txt`, the scheduling algorithm used will be round-robin, the memory allocation algorithm will be best-fit and a single clock cycle will be 3 units.